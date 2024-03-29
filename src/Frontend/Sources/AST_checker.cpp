#include "AST.h"

AST::Context::Context() {
    typeSpace.emplace_back();
    nameSpace.emplace_back();
    for (auto &i: base_types) {
        if (i == "int8")
            addAliasType(i, addType(std::make_unique<IntType>(8)));
        if (i == "int32")
            addAliasType(i, addType(std::make_unique<IntType>(32)));
        if (i == "int64")
            addAliasType(i, addType(std::make_unique<IntType>(64)));
        if (i == "bool")
            addAliasType(i, addType(std::make_unique<BoolType>()));
        if (i == "float")
            addAliasType(i, addType(std::make_unique<FloatType>()));

    }
}

bool AST::Context::checkIfNameExist(std::string name) {
    if (nameSpace.back().find(name) != nameSpace.back().end())
        return true;
    return false;
}

bool AST::Context::checkIfTypeExist(std::string name) {
    for (long long i = typeSpace.size() - 1; i >= 0; i--)
        if (typeSpace[i].find(name) != typeSpace[i].end())
            return true;
    return false;
}

void AST::Context::goDeeper(bool go_in_loop = false, bool go_in_switch = false) {
    typeSpace.emplace_back();
    nameSpace.emplace_back();

    pr_loop_status.push(in_loop);
    pr_switch_status.push(in_switch);
    in_loop = go_in_loop || in_loop;
    in_switch = go_in_switch || in_switch;
}

void AST::Context::goUp() {
    typeSpace.erase(typeSpace.end() - 1, typeSpace.end());
    nameSpace.erase(nameSpace.end() - 1, nameSpace.end());

    in_loop = pr_loop_status.top();
    in_switch = pr_switch_status.top();
}

Type *AST::Context::getTypeByTypeName(std::string name) {
    if (name == "int")
        name = "int32";
    for (long long i = typeSpace.size() - 1; i >= 0; i--)
        if (typeSpace[i].find(name) != typeSpace[i].end())
            return typeSpace[i][name];

    return nullptr;
}

AST::ItemInNameSpace *AST::Context::getInfByVarName(std::string name) {
    for (long long i = nameSpace.size() - 1; i >= 0; i--)
        if (nameSpace[i].find(name) != nameSpace[i].end())
            return &nameSpace[i][name];

    return nullptr;
}

Type *AST::Context::addType(std::unique_ptr<Type> &&new_type) {
    for (auto &i: existItems)
        if (i->compareSignatures(new_type.get()))
            return i.get();

    existItems.emplace_back(std::move(new_type));
    return existItems[existItems.size() - 1].get();
}

Type *AST::Context::addAliasType(std::string new_name, Type *new_type) {
    typeSpace[typeSpace.size() - 1][new_name] = new_type;
    return new_type;
}

void AST::Context::addIntoNameSpace(std::string new_name, Type *new_type, bool is_const = false) {
    if (checkIfNameExist(new_name))
        throw std::invalid_argument("ERROR. Var with such name (" + new_name + ") already exists.");
    nameSpace[nameSpace.size() - 1][new_name] = ItemInNameSpace(new_type, is_const);
}

Type *AST::Context::getPointer(Type *base_type) {
    if (pointers.find(base_type) != pointers.end())
        return pointers[base_type];

    auto new_pointer = std::make_unique<PointerType>(base_type);
    pointers[base_type] = new_pointer.get();

    addType(std::move(new_pointer));
    return pointers[base_type];
}

Type *AST::Context::addForLater(std::unique_ptr<Type> &&new_tmp_type) {
    linksToType[new_tmp_type.get()] = std::vector<Type **>();
    this->notYetFinishedTypes.emplace_back(std::move(new_tmp_type));

    return notYetFinishedTypes[notYetFinishedTypes.size() - 1].get();
}

void AST::Context::addFieldFillInLater(ASTType *type_node, Type **type) {
    notYetFinishedFields.emplace_back(type_node, type);
}

void AST::Context::fillUpEmptyField() {
    for (auto &i: notYetFinishedFields) {
        *i.second = i.first->checker(*this);
    }

}

void AST::Context::transFromTmpTypes() {
    for (auto &i: notYetFinishedTypes)
        addType(std::move(i));
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

bool AST::ASTNode::hasAddress() {
    return false;
}

bool AST::ASTNode::isConst() {
    return false;
}

Type *AST::ASTTypePointer::checker(Context &ctx) {
    typeOfNode = ctx.getPointer(type->checker(ctx));
    return typeOfNode;
}

std::set<std::string> AST::ASTTypePointer::getDependencies() {
    return {};
}

Type *AST::ASTTypeStruct::checker(Context &ctx) {
    auto res = std::make_unique<StructType>();
    for (auto &i: fileds) {
        try {
            res->addNewField(i.first, i.second->checker(ctx));
        } catch (std::invalid_argument e) {
            if (ctx.GlobalInit) {
                res->addNewField(i.first, nullptr);
                ctx.addFieldFillInLater(i.second.get(), res->getDoubleLinkToField(i.first));
            } else
                throw e;
        }
    }
    typeOfNode = ctx.addType(std::move(res));
    return typeOfNode;
}

std::set<std::string> AST::ASTTypeStruct::getDependencies() {
    std::set<std::string> res;
    for (auto &i: fileds)
        res.merge(i.second->getDependencies());
    return res;
}

Type *AST::ASTTypeNamed::checker(Context &ctx) {
    typeOfNode = ctx.getTypeByTypeName(name);
    if (typeOfNode)
        return typeOfNode;
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
        typeOfNode = ctx.greaterInt(LType, RType);

    if ((op == LT || op == LE || op == GT || op == GE) &&
        (ctx.isInt(LType) || ctx.isFloat(LType)) && (ctx.isInt(RType) || ctx.isFloat(RType)))
        typeOfNode = ctx.getTypeByTypeName("bool");

    if ((op == EQ || op == NE) && LType->canConvertToThisType(RType))
        typeOfNode = ctx.getTypeByTypeName("bool");

    if ((op == PLUS || op == MINUS || op == MUL || op == DIV) &&
        (ctx.isInt(LType) || ctx.isFloat(LType)) && (ctx.isInt(RType) || ctx.isFloat(RType))) {

        if (ctx.isInt(LType) && ctx.isInt(RType))
            typeOfNode = ctx.greaterInt(LType, RType);
        typeOfNode = ctx.getTypeByTypeName("float");

    }

    if ((op == AND || op == OR) && ctx.isBool(LType) && ctx.isBool(RType))
        typeOfNode = ctx.getTypeByTypeName("bool");

    if (!typeOfNode)
        throw std::invalid_argument("ERROR. Not allowed operation above types.");

    return typeOfNode;
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
        typeOfNode = Type;

    if ((op == POSTDEC || op == POSTINC || op == PREDEC || op == PREINC) && (ctx.isInt(Type) || ctx.isFloat(Type)) &&
        value->hasAddress())
        typeOfNode = Type;

    if (op == DEREFER) {
        auto pointType = dynamic_cast<PointerType *>(Type);
        if (!pointType)
            throw std::invalid_argument("ERROR. Attempt to deref not pointer type.");

        typeOfNode = pointType->getBase();
    }

    if (op == REFER) {
        if (!value->hasAddress())
            throw std::invalid_argument("ERROR. Attempt to take pointer from non-addressable var.");
        typeOfNode = ctx.getPointer(Type);
    }

    if (op == NOT && ctx.isBool(Type))
        typeOfNode = Type;

    if (!typeOfNode)
        throw std::invalid_argument("ERROR. Not allowed operation above type.");
    return typeOfNode;
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


    typeOfNode = func_type->getReturn();
    return typeOfNode;
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
    typeOfNode = field;
    return typeOfNode;
}

std::set<std::string> AST::ASTMemberAccess::getVarNames() {
    return name->getVarNames();
}

bool AST::ASTIntNumber::isConst() {
    return true;
}

Type *AST::ASTIntNumber::checker(AST::Context &ctx) {
    typeOfNode = ctx.getTypeByTypeName("int");
    return typeOfNode;
}

std::set<std::string> AST::ASTIntNumber::getVarNames() {
    return {};
}

bool AST::ASTFloatNumber::isConst() {
    return true;
}

Type *AST::ASTFloatNumber::checker(AST::Context &ctx) {
    typeOfNode = ctx.getTypeByTypeName("float");
    return typeOfNode;
}

std::set<std::string> AST::ASTFloatNumber::getVarNames() {
    return {};
}

bool AST::ASTBoolNumber::isConst() {
    return true;
}


Type *AST::ASTBoolNumber::checker(AST::Context &ctx) {
    typeOfNode = ctx.getTypeByTypeName("bool");
    return typeOfNode;
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

    typeOfNode = struct_type;
    return typeOfNode;
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

    if (!ctx.checkIfNameExist(name))
        throw std::invalid_argument("ERROR. Unknown name (" + name + ") for a variable");
    auto var = ctx.getInfByVarName(name);
    is_const = var->is_const;

    typeOfNode = var->type;
    return typeOfNode;
}

std::set<std::string> AST::ASTVar::getVarNames() {
    return {name};
}

void AST::dispatchedDecl::declareVars(Context &ctx) {

    if (ctx.checkIfNameExist(name))
        throw std::invalid_argument("ERROR. Such name (" + name + ") for var already exists.");

    if (type && !type->typeOfNode)
        type->checker(ctx);

    if (!type) {
        ctx.addIntoNameSpace(name, expr->checker(ctx), const_var);
        return;
    }

    if (expr) {
        auto type_of_expr = expr->checker(ctx);
        if (!type->typeOfNode->canConvertToThisType(type_of_expr))
            throw std::invalid_argument("ERROR. Type of var and type of expression different.");
    }

    ctx.addIntoNameSpace(name, type->typeOfNode, const_var);


}

std::vector<AST::dispatchedDecl> AST::ASTTypeDeclaration::globalPreInit() {
    return {dispatchedDecl(name[0], this, type->getDependencies())};
}

Type *AST::ASTTypeDeclaration::checker(Context &ctx) {
    if (!ctx.GlobalInit && ctx.checkIfTypeExist(name[0]))
        throw std::invalid_argument("ERROR. Name " + name[0] + " used twice during declaration.");

    ctx.addAliasType(name[0], type->checker(ctx));
    return nullptr;
}

std::vector<AST::dispatchedDecl> AST::ASTVarDeclaration::globalPreInit() {
    std::vector<AST::dispatchedDecl> res;
    // TODO if value.size() = 1 and it is func -- parse
    if (name.size() != value.size() && value.size() != 0)
        throw std::invalid_argument("ERROR. Assignment number mismatch.");

    for (int i = 0; i < name.size(); ++i)
        res.emplace_back(name[i], this, value.size() > 0 ? value[i]->getVarNames() : std::set<std::string>(),
                         value.size() > 0 ? value[i].get() : nullptr, type.get());


    return res;
}

Type *AST::ASTVarDeclaration::checker(Context &ctx) {
    if (type && !type->typeOfNode)
        type->checker(ctx);

    if (name.size() != value.size() && value.size() != 0)
        throw std::invalid_argument("ERROR. Assignment number mismatch.");

    for (auto i = 0; i < name.size(); ++i) {

        if (ctx.checkIfNameExist(name[i]))
            throw std::invalid_argument("ERROR. Such name (" + name[i] + ") for var already exists.");

        if (!type) {
            ctx.addIntoNameSpace(name[i], value[i]->checker(ctx));
            continue;
        }

        if (!value.empty()) {
            auto type_of_expr = value[i]->checker(ctx);
            if (!type->typeOfNode->canConvertToThisType(type_of_expr))
                throw std::invalid_argument("ERROR. Type of var and type of expression different.");
        }

        ctx.addIntoNameSpace(name[i], type->typeOfNode);

    }


    return nullptr;
}

std::vector<AST::dispatchedDecl> AST::ASTConstDeclaration::globalPreInit() {
    std::vector<AST::dispatchedDecl> res;
    // TODO if value.size() = 1 and it is func -- parse
    if (name.size() != value.size())
        throw std::invalid_argument("ERROR. Assignment number mismatch.");

    for (int i = 0; i < name.size(); ++i)
        res.emplace_back(name[i], this, value[i]->getVarNames(), value[i].get(), type.get(), true);

    return res;
}

Type *AST::ASTConstDeclaration::checker(Context &ctx) {
    if (type && !type->typeOfNode)
        type->checker(ctx);

    if (name.size() != value.size())
        throw std::invalid_argument("ERROR. Assignment number mismatch.");

    for (auto i = 0; i < name.size(); ++i) {

        if (ctx.checkIfNameExist(name[i]))
            throw std::invalid_argument("ERROR. Such name (" + name[i] + ") for var already exists.");

        if (!type) {
            ctx.addIntoNameSpace(name[i], value[i]->checker(ctx), true);
            continue;
        }

        auto type_of_expr = value[i]->checker(ctx);
        if (!type->typeOfNode->canConvertToThisType(type_of_expr))
            throw std::invalid_argument("ERROR. Type of var and type of expression different.");


        ctx.addIntoNameSpace(name[i], type->typeOfNode, true);

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
    auto type_of_clause = expr->checker(ctx);
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
        StructType *structType;
        if (dynamic_cast<StructType *>(type_of_method->checker(ctx)))
            structType = dynamic_cast<StructType *>(type_of_method->checker(ctx));

        if (dynamic_cast<PointerType *>(type_of_method->checker(ctx)) &&
            dynamic_cast<StructType *>(dynamic_cast<PointerType *>(type_of_method->checker(ctx))->getBase()))
            structType = dynamic_cast<StructType *>(dynamic_cast<PointerType *>(type_of_method->checker(
                    ctx))->getBase());

        if (!structType)
            throw std::invalid_argument("ERROR. Cannot create method for such type.");

        structType->addNewField(name, ctx.addType(std::move(new_function_type)));
        return;
    }

    // if it is function
    ctx.addIntoNameSpace(name, ctx.addType(std::move(new_function_type)));
}


Type *AST::Program::checker(Context &ctx) {
    // type declarations
    {
        std::vector<dispatchedDecl> declarations;
        for (auto &i: typeDeclarations)
            declarations.push_back(i->globalPreInit()[0]);

        // check if there is the same names
        for (long long i = 0; i < declarations.size(); ++i)
            for (long long j = i + 1; j < declarations.size(); ++j)
                if (i != j && declarations[i].name == declarations[j].name)
                    throw std::invalid_argument(
                            "ERROR. Name " + declarations[i].name + " used twice during declaration of the types.");

        // sort types in an order
        auto q = topSort(declarations);

        // first round of declarations.
        while (!q.empty()) {
            q.front().decl->checker(ctx);

            q.pop();
        }

        // Fulfill empty fields, which occur in case of a forward declaration.
        ctx.fillUpEmptyField();

        // run second round of declarations.
        for (auto &i: typeDeclarations)
            i->checker(ctx);

    }

    // declare
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
            q.front().declareVars(ctx);
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