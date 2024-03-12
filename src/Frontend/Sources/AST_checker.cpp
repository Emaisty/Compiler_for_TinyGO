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
    if (name_space.find(name) != name_space.end())
        return;

    if (!prCtx)
        throw std::invalid_argument("ERROR. This name already exists");

    prCtx->checkIfTypeExist(name);
}

void AST::Context::checkIfTypeExist(std::string name) {
    if (type_space.find(name) != type_space.end())
        return;

    if (!prCtx)
        throw std::invalid_argument("ERROR. This type already exists");

    prCtx->checkIfTypeExist(name);
}

Type *AST::Context::getTypeByTypeName(std::string name) {
    if (name == "int")
        name = "int32";
    if (type_space.find(name) != type_space.end())
        return type_space[name].get();

    if (prCtx)
        return prCtx->getTypeByTypeName(name);
    return nullptr;
}

AST::ItemInNameSpace *AST::Context::getTypeByVarName(std::string name) {
    if (name_space.find(name) != name_space.end())
        return &name_space.at(name);

    if (prCtx)
        return prCtx->getTypeByVarName(name);

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

void AST::Context::addIntoNameSpace(std::string new_name, Type *new_type, bool is_const) {
    checkIfNameExist(new_name);
    name_space.emplace_back(new_name, new_type, level, is_const);
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
    if (getTypeByTypeName("int8")->type.get() == other || getTypeByTypeName("int32")->type.get() == other ||
        getTypeByTypeName("int64")->type.get() == other)
        return true;
    return false;
}

Type *AST::Context::greaterInt(const Type *TypeA, const Type *TypeB) {
    if (TypeA == getTypeByTypeName("int64")->type.get() || TypeB == getTypeByTypeName("int64")->type.get())
        return getTypeByTypeName("int64")->type.get();

    if (TypeA == getTypeByTypeName("int32")->type.get() || TypeB == getTypeByTypeName("int32")->type.get())
        return getTypeByTypeName("int32")->type.get();

    return getTypeByTypeName("int8")->type.get();
}

bool AST::Context::isFloat(const Type *other) {
    if (getTypeByTypeName("float")->type.get() == other)
        return true;
    return false;
}

bool AST::Context::isBool(const Type *other) {
    if (getTypeByTypeName("bool")->type.get() == other)
        return true;
    return false;
}

void AST::Context::addForLater(std::string new_name, ASTType *new_type, UnNamedStruct *unNamedStruct) {
    unfinishedDecl.emplace_back(new_name, new_type, unNamedStruct);
}

void AST::Context::fillLaterStack() {
    for (auto &i: unfinishedDecl)

}

void AST::Context::setPrCtx(Context &pr_ctx, bool new_in_loop, bool new_in_switch) {
    prCtx = &pr_ctx;
    in_loop = new_in_loop ? new_in_loop : pr_ctx.in_loop;
    in_switch = new_in_switch ? new_in_switch : pr_ctx.in_switch;
}

bool AST::DeclaredItems::nameBeenDeclared(std::string name) {
    if (declaredNames.find(name) != declaredNames.end())
        return true;

    if (prScope)
        return prScope->nameBeenDeclared(name);

    return false;
}

bool AST::ASTNode::hasAddress() {
    return false;
}

bool AST::ASTNode::isConst() {
    return false;
}

Type *AST::ASTTypePointer::checker(Context &ctx) {
    type_of_node = ctx.getPointer(type->checker(ctx));
    return type_of_node;
}

std::set<std::string> AST::ASTTypePointer::getDependencies(DeclaredItems &items) {
    return {};
}

Type *AST::ASTTypeStruct::checker(Context &ctx) {
    auto res = std::unique_ptr<UnNamedStruct>();

    for (auto &i: fileds) {
        try {
            res->addNewField(i.first, i.second->checker(ctx));
        } catch (std::invalid_argument &e) {
            // TODO
            if (ctx.GlobalInit) {
                res->addNewField(i.first, nullptr);
                auto fillLater = res->getField(i.first);
                ctx.addForLater(&fillLater, i.second.get());
            } else
                throw e;
        }
    }

    type_of_node = ctx.addType("", std::move(res));
    return type_of_node;
}

std::set<std::string> AST::ASTTypeStruct::getDependencies(DeclaredItems &items) {
    std::set<std::string> res;
    for (auto &i: fileds)
        res.merge(i.second->getDependencies());
    return res;
}

Type *AST::ASTTypeNamed::checker(Context &ctx) {
    auto res = ctx.getTypeByTypeName(name);
    if (res)
        return res->type.get();
    throw std::invalid_argument("ERROR. Unknown type.");
}

std::set<std::string> AST::ASTTypeNamed::getDependencies(DeclaredItems &items) {
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
        type_of_node = ctx.greaterInt(LType, RType);

    if ((op == LT || op == LE || op == GT || op == GE) &&
        (ctx.isInt(LType) || ctx.isFloat(LType)) && (ctx.isInt(RType) || ctx.isFloat(RType)))
        type_of_node = ctx.getTypeByTypeName("bool")->type.get();

    if ((op == EQ || op == NE) && LType->canConvertToThisType(RType))
        type_of_node = ctx.getTypeByTypeName("bool")->type.get();

    if ((op == PLUS || op == MINUS || op == MUL || op == DIV) &&
        (ctx.isInt(LType) || ctx.isFloat(LType)) && (ctx.isInt(RType) || ctx.isFloat(RType))) {

        if (ctx.isInt(LType) && ctx.isInt(RType))
            type_of_node = ctx.greaterInt(LType, RType);
        type_of_node = ctx.getTypeByTypeName("float")->type.get();

    }

    if ((op == AND || op == OR) && ctx.isBool(LType) && ctx.isBool(RType))
        type_of_node = ctx.getTypeByTypeName("bool")->type.get();

    if (type_of_node)
        return type_of_node;

    throw std::invalid_argument("ERROR. Not allowed operation above types.");
}

std::set<std::string> AST::ASTBinaryOperator::getDependencies(DeclaredItems &items) {
    auto res = left->getDependencies(items);
    res.merge(right->getDependencies(items));
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
        type_of_node = Type;

    if ((op == POSTDEC || op == POSTINC || op == PREDEC || op == PREINC) && (ctx.isInt(Type) || ctx.isFloat(Type)) &&
        value->hasAddress())
        type_of_node = Type;

    if (op == DEREFER) {
        auto pointType = dynamic_cast<PointerType *>(Type);
        if (!pointType)
            throw std::invalid_argument("ERROR. Attempt to deref not pointer type.");

        type_of_node = pointType->getBase();
    }

    if (op == REFER) {
        if (!value->hasAddress())
            throw std::invalid_argument("ERROR. Attempt to take pointer from non-addressable var.");
        type_of_node = ctx.getPointer(Type);
    }

    if (op == NOT && ctx.isBool(Type))
        type_of_node = Type;

    if (type_of_node)
        return type_of_node;

    throw std::invalid_argument("ERROR. Not allowed operation above type.");
}

std::set<std::string> AST::ASTUnaryOperator::getDependencies(DeclaredItems &items) {
    return value->getDependencies(items);
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

    type_of_node = func_type->getReturn();
    return type_of_node;
}

std::set<std::string> AST::ASTFunctionCall::getDependencies(DeclaredItems &items) {
    std::set<std::string> res;
    for (auto &i: arg)
        res.merge(i->getDependencies(items));
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

    type_of_node = field;

    return type_of_node;
}

std::set<std::string> AST::ASTMemberAccess::getDependencies(DeclaredItems &items) {
    return name->getDependencies(items);
}

bool AST::ASTIntNumber::isConst() {
    return true;
}

Type *AST::ASTIntNumber::checker(AST::Context &ctx) {
    type_of_node = ctx.getTypeByTypeName("int")->type.get();
    return type_of_node;
}

std::set<std::string> AST::ASTIntNumber::getVarNames() {
    return {};
}

bool AST::ASTFloatNumber::isConst() {
    return true;
}

Type *AST::ASTFloatNumber::checker(AST::Context &ctx) {
    type_of_node = ctx.getTypeByTypeName("float")->type.get();
    return type_of_node;
}

std::set<std::string> AST::ASTFloatNumber::getDependencies(DeclaredItems &items) {
    return {};
}

bool AST::ASTBoolNumber::isConst() {
    return true;
}


Type *AST::ASTBoolNumber::checker(AST::Context &ctx) {
    type_of_node = ctx.getTypeByTypeName("bool")->type.get();
    return type_of_node;
}

std::set<std::string> AST::ASTBoolNumber::getDependencies(DeclaredItems &items) {
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
    type_of_node = struct_type;
    return type_of_node;
}

std::set<std::string> AST::ASTStruct::getDependencies(DeclaredItems &items) {
    std::set<std::string> res;
    for (auto &i: values)
        res.merge(i.second->getDependencies(items));
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

    is_const = ctx.getTypeByVarName(name)->is_const;

    type_of_node = ctx.getTypeByVarName(name)->type;
    return type_of_node;
}

std::set<std::string> AST::ASTVar::getDependencies(DeclaredItems &items) {
    return {name};
}

void AST::dispatchedDecl::declare(Context &ctx) {
    if (is_type_decl) {
//        ctx.addType(name,);
        return;
    }
    ctx.addIntoNameSpace(name, type == nullptr ? expression->checker(ctx) : type->checker(ctx), is_const);

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
    if (name.size() != value.size() && !value.empty())
        throw std::invalid_argument("ERROR. Assignment number mismatch.");

    for (int i = 0; i < name.size(); ++i)
        if (type == nullptr)
            res.emplace_back(name[i], value[i].get(), nullptr, value[i]->getDependencies());
        else
            res.emplace_back(name[i], nullptr, type.get(),
                             value.size() == 0 ? std::set<std::string>() : value[i]->getDependencies());

    return res;
}

Type *AST::ASTVarDeclaration::checker(Context &ctx) {

    return type_of_node;
}

std::vector<AST::dispatchedDecl> AST::ASTConstDeclaration::globalPreInit() {
    std::vector<AST::dispatchedDecl> res;
    // TODO if value.size() = 1 and it is func -- parse
    if (name.size() != value.size())
        throw std::invalid_argument("ERROR. Assignment number mismatch.");

    for (int i = 0; i < name.size(); ++i)
        if (type == nullptr)
            res.emplace_back(name[i], value[i].get(), nullptr, value[i]->getDependencies(), false, true);
        else
            res.emplace_back(name[i], nullptr, type.get(), value[i]->getDependencies(), false, true);

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
    Context new_ctx;
    new_ctx.setPrCtx(ctx);
    for (auto &i: statements)
        i->checker(new_ctx);
    return type_of_node;
}

Type *AST::ASTBreak::checker(Context &ctx) {
    if (ctx.in_switch || ctx.in_loop)
        return type_of_node;
    throw std::invalid_argument("ERROR. Called Break statement not in switch case or not in loop");
}

Type *AST::ASTContinue::checker(Context &ctx) {
    if (ctx.in_loop)
        return type_of_node;
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
        Context new_ctx;
        new_ctx.setPrCtx(ctx, false, true);
        if (!type_of_stat->canConvertToThisType(i.first->checker(new_ctx)))
            throw std::invalid_argument("ERROR. Not comparable types between case and expression.");
        i.second->checker(new_ctx);
    }

    return type_of_node;
}

Type *AST::ASTIf::checker(Context &ctx) {
    auto type_of_clause = if_clause->checker(ctx);
    if (!ctx.getTypeByTypeName("bool")->type->canConvertToThisType(type_of_clause))
        throw std::invalid_argument("ERROR. If clause cannot be converted to bool");

    if_clause->checker(ctx);
    if (else_clause)
        else_clause->checker(ctx);

    return type_of_node;
}

Type *AST::ASTFor::checker(Context &ctx) {

    Context new_ctx;
    new_ctx.setPrCtx(ctx, true, false);

    for (auto &i: init_clause)
        i->checker(new_ctx);

    auto type_of_clause = if_clause->checker(new_ctx);
    if (!ctx.getTypeByTypeName("bool")->type->canConvertToThisType(type_of_clause))
        throw std::invalid_argument("ERROR. If clause cannot be converted to bool");

    for (auto &i: iterate_clause)
        i->checker(new_ctx);

    body->checker(new_ctx);

    return type_of_node;
}


Type *AST::ASTAssign::checker(Context &ctx) {

    for (int i = 0; i < variable.size(); ++i) {
        auto var_type = variable[i]->checker(ctx);
        if (variable[i]->isConst())
            throw std::invalid_argument("ERROR. Cannot change type of const val.");
        if (!variable[i]->hasAddress())
            throw std::invalid_argument("ERROR. Var is not assignable type.");
        auto val_type = value[i]->checker(ctx);
        if (!var_type || !val_type || !var_type->canConvertToThisType(val_type))
            throw std::invalid_argument("ERROR. Var and assigned value not the same types");
    }

    return type_of_node;
}

Type *AST::Function::checker(Context &ctx) {
    Context new_ctx;
    new_ctx.setPrCtx(ctx);

    if (type_of_method) {
        ctx.addIntoNameSpace(inner_name, type_of_method->checker(new_ctx));
    }

    for (auto &i: params)
        for (auto &j: i.first)
            ctx.addIntoNameSpace(j, i.second->checker(new_ctx));


    if (return_type)
        ctx.return_type.emplace_back(return_type->checker(new_ctx));

    body->checker(new_ctx);

    return type_of_node;
}

void AST::Function::globalPreInit(Context &ctx) {
    auto new_function_type = std::make_unique<FunctionType>();

    // todo with return
    if (return_type)
        new_function_type->setReturn(return_type->checker(ctx));


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


    return type_of_node;
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