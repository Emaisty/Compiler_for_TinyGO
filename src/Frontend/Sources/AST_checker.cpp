#include "AST.h"

AST::Context::Context() {
    for (auto &i: base_types) {
        if (i == "int8")
            addType(i, std::make_unique<IntType>(8));
        if (i == "int32")
            addType(i, std::make_unique<IntType>(32));
        if (i == "int64")
            addType(i, std::make_unique<IntType>(64));
        if (i == "bool")
            addType(i, std::make_unique<BoolType>());
        if (i == "float")
            addType(i, std::make_unique<FloatType>());
    }
}

void AST::Context::checkIfNameExist(std::string name) {
    for (auto &i: name_space)
        if (i.name == name)
            throw std::invalid_argument("ERROR. This name already exists");
}

void AST::Context::checkIfTypeExist(std::string name) {
    for (auto &i: type_space)
        if (i.name == name)
            throw std::invalid_argument("ERROR. This type already exists");
}

void AST::Context::goDeeper(bool go_in_loop = false, bool go_in_switch = false) {
    level += 1;
    pr_loop_status.push(in_loop);
    pr_switch_status.push(in_switch);
    in_loop = go_in_loop || in_loop;
    in_switch = go_in_switch || in_switch;
}

void AST::Context::goUp() {
    for (int i = name_space.size() - 1; i >= 0; --i)
        if (name_space[i].level == level)
            name_space.erase(name_space.end() - 1, name_space.end());

    for (int i = type_space.size() - 1; i >= 0; --i)
        if (type_space[i].level == level)
            type_space.erase(type_space.end() - 1, type_space.end());

    level -= 1;
    in_loop = pr_loop_status.top();
    in_switch = pr_switch_status.top();
}

Type *AST::Context::getTypeByTypeName(std::string name) {
    if (name == "int")
        name = "int32";
    for (auto &i: type_space)
        if (i.name == name)
            return i.type.get();
    return nullptr;
}

Type *AST::Context::getTypeByVarName(std::string name) {
    for (auto &i: name_space)
        if (i.name == name)
            return i.type;
    return nullptr;
}

Type *AST::Context::addType(std::string new_name, std::unique_ptr<Type> &&new_type) {
    if (!new_name.empty()) {
        // add Alias created by user like "type [name] [type]"
        checkIfTypeExist(new_name);
        type_space.emplace_back(new_name, std::move(new_type), level);
        return type_space.back().type.get();
    } else {
        // add type which uses by program but user havent define by himself

        for (auto &i: type_space) {
            if (i.type->compareSignatures(new_type.get()))
                return i.type.get();
        }

        // if not -- add with unreachable name
        type_space.emplace_back("", std::move(new_type), 0);
        return type_space.back().type.get();
    }
}

void AST::Context::addIntoNameSpace(std::string new_name, Type *new_type) {
    checkIfNameExist(new_name);
    name_space.emplace_back(new_name, new_type, level);
}

Type *AST::Context::getPointer(Type *base_type) {
    auto pointer = pointers.find(base_type);
    if (pointer != pointers.end())
        return pointer->second;

    auto new_pointer = std::make_unique<PointerType>(base_type);
    pointers[base_type] = new_pointer.get();

    addType("", std::move(new_pointer));
    return pointers.find(base_type)->second;
}

bool AST::Context::isInt(const Type *other) {
    if (getTypeByTypeName("int8") == other || getTypeByTypeName("int32") == other ||
        getTypeByTypeName("int64") == other)
        return true;
    return false;
}

Type *AST::Context::greaterInt(const Type *TypeA, const Type *TypeB) {
    if (TypeA == getTypeByTypeName("int64") || TypeB == getTypeByTypeName("int64"))
        return getTypeByTypeName("int64");

    if (TypeA == getTypeByTypeName("int32") || TypeB == getTypeByTypeName("int32"))
        return getTypeByTypeName("int32");

    return getTypeByTypeName("int8");
}

bool AST::Context::isFloat(const Type *other) {
    if (getTypeByTypeName("float") == other)
        return true;
    return false;
}

bool AST::Context::isBool(const Type *other) {
    if (getTypeByTypeName("bool") == other)
        return true;
    return false;
}

void AST::Context::addForLater(std::string new_name, ASTType *new_type, UnNamedStruct *unNamedStruct) {
    unfinishedDecl.emplace_back(new_name, new_type, unNamedStruct);
}

void AST::Context::fillLaterStack() {
    for (auto &i: unfinishedDecl)
        i.struc->addNewField(i.name, i.type->checker(*this));
}

bool AST::ASTNode::hasAddress() {
    return false;
}

bool AST::ASTNode::isConst() {
    return false;
}

Type *AST::ASTTypePointer::checker(Context &ctx) {
    return ctx.getPointer(type->checker(ctx));
}

std::set<std::string> AST::ASTTypePointer::getDependencies() {
    return {};
}

Type *AST::ASTTypeStruct::checker(Context &ctx) {
    auto res = std::unique_ptr<UnNamedStruct>();


    for (auto &i: fileds) {
        try {
            res->addNewField(i.first, i.second->checker(ctx));
        } catch (std::invalid_argument e) {
            if (ctx.GlobalInit) {
                // TODO what if res will be deleted?????
                ctx.addForLater(i.first, i.second.get(), res.get());
            } else
                throw e;
        }
    }

    return ctx.addType("", std::move(res));
}

std::set<std::string> AST::ASTTypeStruct::getDependencies() {
    std::set<std::string> res;
    for (auto &i: fileds)
        res.merge(i.second->getDependencies());
    return res;
}

Type *AST::ASTTypeNamed::checker(Context &ctx) {
    auto res = ctx.getTypeByTypeName(name);
    if (res)
        return res;
    throw std::invalid_argument("ERROR. Unknown type.");
}

std::set<std::string> AST::ASTTypeNamed::getDependencies() {
    if (std::find(AST::Context::base_types.begin(), AST::Context::base_types.end(), name) ==
        AST::Context::base_types.end())
        return {name};
    return {};
}

bool AST::ASTBinaryOperator::isConst() {
    if (left->isConst() && right->isConst())
        return true;
    return false;
}

Type *AST::ASTBinaryOperator::checker(AST::Context &ctx) {
    auto LType = left->checker(ctx);
    auto RType = right->checker(ctx);
    if ((op == BINAND || op == BINOR || op == MOD) && ctx.isInt(LType) && ctx.isInt(RType))
        return ctx.greaterInt(LType, RType);

    if ((op == LT || op == LE || op == GT || op == GE) &&
        (ctx.isInt(LType) || ctx.isFloat(LType)) && (ctx.isInt(RType) || ctx.isFloat(RType)))
        return ctx.getTypeByTypeName("bool");

    if ((op == EQ || op == NE) && LType->canConvertToThisType(RType))
        return ctx.getTypeByTypeName("bool");

    if ((op == PLUS || op == MINUS || op == MUL || op == DIV) &&
        (ctx.isInt(LType) || ctx.isFloat(LType)) && (ctx.isInt(RType) || ctx.isFloat(RType))) {

        if (ctx.isInt(LType) && ctx.isInt(RType))
            return ctx.greaterInt(LType, RType);
        return ctx.getTypeByTypeName("float");

    }

    if ((op == AND || op == OR) && ctx.isBool(LType) && ctx.isBool(RType))
        return ctx.getTypeByTypeName("bool");

    throw std::invalid_argument("ERROR. Not allowed operation above types.");
}

std::set<std::string> AST::ASTBinaryOperator::getVarNames() {
    auto res = left->getVarNames();
    res.merge(right->getVarNames());
    return res;
}

bool AST::ASTUnaryOperator::hasAddress() {
    if (op == DEREFER || op == REFER)
        return true;
    return false;
}

bool AST::ASTUnaryOperator::isConst() {
    return value->isConst();
}

Type *AST::ASTUnaryOperator::checker(AST::Context &ctx) {
    auto Type = value->checker(ctx);
    if ((op == PLUS || op == MINUS) && (ctx.isInt(Type) || ctx.isFloat(Type)))
        return Type;

    if ((op == POSTDEC || op == POSTINC || op == PREDEC || op == PREINC) && (ctx.isInt(Type) || ctx.isFloat(Type)) &&
        value->hasAddress())
        return Type;

    if (op == DEREFER) {
        auto pointType = dynamic_cast<PointerType *>(Type);
        if (!pointType)
            throw std::invalid_argument("ERROR. Attempt to deref not pointer type.");

        return pointType->getBase();
    }

    if (op == REFER) {
        if (!value->hasAddress())
            throw std::invalid_argument("ERROR. Attempt to take pointer from non-addressable var.");
        return ctx.getPointer(Type);
    }

    if (op == NOT && ctx.isBool(Type))
        return Type;

    throw std::invalid_argument("ERROR. Not allowed operation above type.");
}

std::set<std::string> AST::ASTUnaryOperator::getVarNames() {
    return value->getVarNames();
}


Type *AST::ASTFunctionCall::checker(AST::Context &ctx) {
    auto func_type = dynamic_cast<FunctionType *>(name->checker(ctx));
    if (!func_type)
        throw std::invalid_argument("ERROR. Attempt to call function call on non-function.");

    std::vector<Type *> args;
    for (auto &i: arg)
        args.emplace_back(i->checker(ctx));

    if (!func_type->compareArgs(args))
        throw std::invalid_argument("ERROR. Mismatch in types between passed argument type and expected type.");

    return func_type->getReturn();
}

std::set<std::string> AST::ASTFunctionCall::getVarNames() {
    std::set<std::string> res;
    for (auto &i: arg)
        res.merge(i->getVarNames());
    return res;
}

bool AST::ASTMemberAccess::hasAddress() {
    return this->name->hasAddress();
}

Type *AST::ASTMemberAccess::checker(AST::Context &ctx) {
    auto type = name->checker(ctx);

    StructType *struct_type;
    if (dynamic_cast<StructType *>(type))
        struct_type = dynamic_cast<StructType *>(type);
    else if (dynamic_cast<PointerType *>(type))
        struct_type = dynamic_cast<StructType *>(dynamic_cast<PointerType *>(type)->getBase());

    if (!struct_type)
        throw std::invalid_argument("ERROR. Field does not exist.");

    auto field = struct_type->getField(member);
    if (!field)
        throw std::invalid_argument("ERROR. Field does not exist.");
    return field;
}

std::set<std::string> AST::ASTMemberAccess::getVarNames() {
    return name->getVarNames();
}

bool AST::ASTIntNumber::isConst() {
    return true;
}

Type *AST::ASTIntNumber::checker(AST::Context &ctx) {
    return ctx.getTypeByTypeName("int");
}

std::set<std::string> AST::ASTIntNumber::getVarNames() {
    return {};
}

bool AST::ASTFloatNumber::isConst() {
    return true;
}

Type *AST::ASTFloatNumber::checker(AST::Context &ctx) {
    return ctx.getTypeByTypeName("float");
}

std::set<std::string> AST::ASTFloatNumber::getVarNames() {
    return {};
}

bool AST::ASTBoolNumber::isConst() {
    return true;
}


Type *AST::ASTBoolNumber::checker(AST::Context &ctx) {
    return ctx.getTypeByTypeName("bool");
}

std::set<std::string> AST::ASTBoolNumber::getVarNames() {
    return {};
}

bool AST::ASTStruct::isConst() {
    return true;
}

Type *AST::ASTStruct::checker(AST::Context &ctx) {

    auto struct_type = dynamic_cast<StructType *>(type->checker(ctx));

    for (auto &i: values) {
        auto field_type = struct_type->getField(i.first);
        if (!field_type)
            throw std::invalid_argument("ERROR. Field with such name does not exists in structure.");
        if (field_type->canConvertToThisType(i.second->checker(ctx)))
            throw std::invalid_argument("ERROR. Filed and assigned type mismatch.");
    }

    return struct_type;
}

std::set<std::string> AST::ASTStruct::getVarNames() {
    std::set<std::string> res;
    for (auto &i: values)
        res.merge(i.second->getVarNames());
    return res;
}

bool AST::ASTVar::hasAddress() {
    return true;
}

bool AST::ASTVar::isConst() {
    return is_const;
}

Type *AST::ASTVar::checker(AST::Context &ctx) {
    ctx.checkIfNameExist(name);

    for (auto &i: ctx.name_space)
        if (i.name == name)
            is_const = i.is_const;


    return ctx.getTypeByVarName(name);
}

std::set<std::string> AST::ASTVar::getVarNames() {
    return {name};
}

void AST::dispatchedDecl::declare(Context &ctx) {
    //TODO
}

std::vector<AST::dispatchedDecl> AST::ASTTypeDeclaration::globalPreInit() {
    return {dispatchedDecl(name[0], nullptr, type.get(), type->getDependencies(), true)};
}

Type *AST::ASTTypeDeclaration::checker(Context &ctx) {
//    if (ctx.level != 0) {
//        ctx.checkIfTypeExist(name[0]);
//        ctx.addType(name[0], type->checker(ctx)->makeUnique());
//    }
//    return nullptr;
//    TODO
}

std::vector<AST::dispatchedDecl> AST::ASTVarDeclaration::globalPreInit() {
    std::vector<AST::dispatchedDecl> res;
    // TODO if value.size() = 1 and it is func -- parse
    if (name.size() != value.size() && value.size() != 0)
        throw std::invalid_argument("ERROR. Assignment number mismatch.");

    for (int i = 0; i < name.size(); ++i)
        if (type == nullptr)
            res.emplace_back(name[i], value[i].get(), nullptr, value[i]->getVarNames());
        else
            res.emplace_back(name[i], nullptr, type.get(),
                             value.size() == 0 ? std::set<std::string>() : value[i]->getVarNames());

    return res;
}

Type *AST::ASTVarDeclaration::checker(Context &ctx) {

    return nullptr;
}

std::vector<AST::dispatchedDecl> AST::ASTConstDeclaration::globalPreInit() {
    std::vector<AST::dispatchedDecl> res;
    // TODO if value.size() = 1 and it is func -- parse
    if (name.size() != value.size())
        throw std::invalid_argument("ERROR. Assignment number mismatch.");

    for (int i = 0; i < name.size(); ++i)
        if (type == nullptr)
            res.emplace_back(name[i], value[i].get(), nullptr, value[i]->getVarNames(), false, true);
        else
            res.emplace_back(name[i], nullptr, type.get(), value[i]->getVarNames(), false, true);

    return res;
}

Type *AST::ASTConstDeclaration::checker(Context &ctx) {
    for (int i = 0; i < name.size(); ++i) {
        ctx.checkIfNameExist(name[i]);
        if (!value[i]->isConst())
            throw std::invalid_argument("ERROR. Not constable expression is tried to passed to const var.");

    }
    return nullptr;
}

Type *AST::ASTBlock::checker(Context &ctx) {
    ctx.goDeeper();
    for (auto &i: statements)
        i->checker(ctx);
    ctx.goUp();
    return nullptr;
}

Type *AST::ASTBreak::checker(Context &ctx) {
    if (ctx.in_switch || ctx.in_loop)
        return nullptr;
    throw std::invalid_argument("ERROR. Called Break statement not in switch case or not in loop");
}

Type *AST::ASTContinue::checker(Context &ctx) {
    if (ctx.in_loop)
        return nullptr;
    throw std::invalid_argument("ERROR. Called Continue statement not in loop");
}

Type *AST::ASTReturn::checker(Context &ctx) {
    if (ctx.return_type.size() != return_value.size())
        throw std::invalid_argument("ERROR. Mismatch number of returned values.");

    for (int i = 0; i < return_value.size(); ++i)
        if (!return_value[i]->checker(ctx)->canConvertToThisType(ctx.return_type[i]))
            throw std::invalid_argument("ERROR. Not matching return value");

    return nullptr;
}

Type *AST::ASTSwitch::checker(Context &ctx) {
    auto type_of_stat = expr->checker(ctx);
    for (auto &i: cases) {
        ctx.goDeeper(false, true);
        if (!type_of_stat->canConvertToThisType(i.first->checker(ctx)))
            throw std::invalid_argument("ERROR. Not comparable types between case and expression.");
        i.second->checker(ctx);
        ctx.goUp();
    }

    return nullptr;
}

Type *AST::ASTIf::checker(Context &ctx) {
    auto type_of_clause = if_clause->checker(ctx);
    if (!ctx.getTypeByTypeName("bool")->canConvertToThisType(type_of_clause))
        throw std::invalid_argument("ERROR. If clause cannot be converted to bool");

    if_clause->checker(ctx);
    if (else_clause)
        else_clause->checker(ctx);

    return nullptr;
}

Type *AST::ASTFor::checker(Context &ctx) {
    ctx.goDeeper(true);
    for (auto &i: init_clause)
        i->checker(ctx);

    auto type_of_clause = if_clause->checker(ctx);
    if (!ctx.getTypeByTypeName("bool")->canConvertToThisType(type_of_clause))
        throw std::invalid_argument("ERROR. If clause cannot be converted to bool");

    for (auto &i: iterate_clause)
        i->checker(ctx);

    body->checker(ctx);

    ctx.goUp();
    return nullptr;
}


Type *AST::ASTAssign::checker(Context &ctx) {

    for (int i = 0; i < variable.size(); ++i) {
        auto var_type = variable[i]->checker(ctx);
        if (!variable[i]->hasAddress())
            throw std::invalid_argument("ERROR. Var is not assignable type.");
        auto val_type = value[i]->checker(ctx);
        if (!var_type || !val_type || !var_type->canConvertToThisType(val_type))
            throw std::invalid_argument("ERROR. Var and assigned value not the same types");
    }

    return nullptr;
}

Type *AST::Function::checker(Context &ctx) {
    ctx.goDeeper();

    if (type_of_method) {
        ctx.addIntoNameSpace(inner_name, type_of_method->checker(ctx));
    }

    for (auto &i: params)
        for (auto &j: i.first)
            ctx.addIntoNameSpace(j, i.second->checker(ctx));


    for (auto &i: return_type)
        ctx.return_type.emplace_back(i->checker(ctx));

    body->checker(ctx);

    ctx.goUp();

    return nullptr;
}

void AST::Function::globalPreInit(Context &ctx) {
    auto new_function_type = std::make_unique<FunctionType>();

    // todo with return
    if (return_type.size() == 1)
        new_function_type->setReturn(return_type[0]->checker(ctx));


    for (auto &i: params)
        for (auto &j: i.first)
            new_function_type->addParam(i.second->checker(ctx));

    //if it is method
    if (type_of_method) {
        NamedStructure *structType;
        if (dynamic_cast<NamedStructure *>(type_of_method->checker(ctx)))
            structType = dynamic_cast<NamedStructure *>(type_of_method->checker(ctx));

        if (dynamic_cast<PointerType *>(type_of_method->checker(ctx)) &&
            dynamic_cast<NamedStructure *>(dynamic_cast<PointerType *>(type_of_method->checker(ctx))->getBase()))
            structType = dynamic_cast<NamedStructure *>(dynamic_cast<PointerType *>(type_of_method->checker(
                    ctx))->getBase());

        if (!structType)
            throw std::invalid_argument("ERROR. Cannot create method for such type.");

        structType->addNewField(name, ctx.addType("", std::move(new_function_type)));
        return;
    }

    // if it is function
    ctx.addIntoNameSpace(name, ctx.addType("", std::move(new_function_type)));
}


Type *AST::Program::checker(Context &ctx) {
    // type declarations
    {
        std::vector<dispatchedDecl> declarations;
        for (auto &i: typeDeclarations)
            declarations.push_back(i->globalPreInit()[0]);

        auto q = topSort(declarations);

        while (!q.empty()) {
            q.front().declare(ctx);
            q.pop();
        }
        // TODO post declarations
    }

    //
    for (auto &i: functions)
        i->globalPreInit(ctx);

    // var and const declarations
    {
        std::vector<dispatchedDecl> declarations;
        for (auto &i: varDeclarations)
            for (auto &j: i->globalPreInit())
                declarations.push_back(j);

        auto q = topSort(declarations);

        while (!q.empty()) {
            q.front().declare(ctx);
            q.pop();
        }
    }


    ctx.GlobalInit = false;


    for (auto &i: functions)
        i->checker(ctx);

    return nullptr;
}

std::queue<AST::dispatchedDecl> AST::Program::topSort(std::vector<dispatchedDecl> g) {
    std::queue<dispatchedDecl> res;
    while (!g.empty()) {
        auto pr_size = g.size();

        for (auto i = 0; i < g.size(); ++i)
            if (g[i].depends.empty()) {
                res.push(g[i]);
                for (auto &j: g)
                    j.depends.erase(g[i].name);
                g.erase(g.begin() + i, g.begin() + i + 1);
                break;
            }

        if (g.size() == pr_size)
            throw std::invalid_argument("ERROR. Loop declaration.");
    }


    return res;
}