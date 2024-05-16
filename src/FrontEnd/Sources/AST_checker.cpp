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
    // compare with already existed. If exists with the exact same signature
    // delete new_type and return pointer to already exists item
    for (auto &i: existItems)
        if (i->compareSignatures(new_type.get()))
            return i.get();

    // if there was no type with the same signature -- add to the space and return pointer to it
    existItems.emplace_back(std::move(new_type));
    return existItems.back().get();
}

Type *AST::Context::addAliasType(std::string new_name, Type *new_type) {
    typeSpace.back()[new_name] = new_type;
    return new_type;
}

void AST::Context::addIntoNameSpace(std::string new_name, Type *new_type, bool is_const = false) {
    if (checkIfNameExist(new_name))
        throw std::invalid_argument("ERROR. Var with such name (" + new_name + ") already exists.");
    nameSpace.back()[new_name] = ItemInNameSpace(new_type, is_const);
}

Type *AST::Context::getPointer(Type *base_type) {
    // if pointer to such type already exists -- return it
    if (pointers.find(base_type) != pointers.end())
        return pointers[base_type];

    // if not -- create and return it
    auto new_pointer = std::make_unique<PointerType>(base_type);
    pointers[base_type] = new_pointer.get();

    addType(std::move(new_pointer));
    return pointers[base_type];
}

Type *AST::Context::addForLater(std::unique_ptr<Type> &&new_tmp_type) {
    linksToType[new_tmp_type.get()] = std::vector<Type **>();
    this->notYetFinishedTypes.emplace_back(std::move(new_tmp_type));

    return notYetFinishedTypes.back().get();
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

std::unique_ptr<AST::ASTExpression>
AST::Context::convertTypeTo(std::unique_ptr<ASTExpression> &&from, Type *to) {
    if (from->typeOfNode == to)
        return from;
    auto cast = std::make_unique<AST::ASTCast>();
    cast->setChild(std::move(from));
    cast->setTypeCastTo(to);
    return cast;
}

std::pair<std::unique_ptr<AST::ASTExpression>, std::unique_ptr<AST::ASTExpression>>
AST::Context::convertTypesEq(std::unique_ptr<ASTExpression> &&left, std::unique_ptr<ASTExpression> &&right) {
    // if exact -- change nothing and return
    if (left->typeOfNode == right->typeOfNode)
        return std::make_pair(std::move(left), std::move(right));

    // if left less - change the left and return new one
    if (typeGreater(left->typeOfNode, right->typeOfNode)) {
        auto new_left = std::make_unique<AST::ASTCast>();
        new_left->setChild(std::move(left));
        new_left->setTypeCastTo(right->typeOfNode);
        return std::make_pair(std::move(new_left), std::move(right));
    }

    auto new_right = std::make_unique<AST::ASTCast>();
    new_right->setChild(std::move(right));
    new_right->setTypeCastTo(left->typeOfNode);
    return std::make_pair(std::move(left), std::move(new_right));
}

bool AST::Context::typeGreater(Type *l_type, Type *r_type) {
    int l_level = -1, r_level = -1;
    static const std::vector<std::string> types_name = {"float", "int64", "int32", "int8"};
    for (auto i = 4; i >= 1; --i)
        if (getTypeByTypeName(types_name[4 - i]) == l_type)
            l_level = i;
    return l_level > r_level;
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
    for (auto &[names, type]: fileds) {
        for (auto &name: names)
            try {
                res->addNewField(name, type->checker(ctx));
            } catch (std::invalid_argument e) {
                if (ctx.GlobalInit) {
                    // if type does not exist yet -- do not throw error and leave it to fulfill it later
                    res->addNewField(name, nullptr);
                    ctx.addFieldFillInLater(type.get(), res->getDoubleLinkToField(name));
                } else
                    // if decl is not in the top-level -- raise the error
                    throw e;
            }
    }
    typeOfNode = ctx.addType(std::move(res));
    return typeOfNode;
}

std::set<std::string> AST::ASTTypeStruct::getDependencies() {
    std::set < std::string > res;
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
    if ((op == IR::IRArithOp::Operator::BINAND || op == IR::IRArithOp::Operator::BINOR ||
         op == IR::IRArithOp::Operator::MOD) && ctx.isInt(LType) && ctx.isInt(RType))
        typeOfNode = ctx.greaterInt(LType, RType);

    if ((op == IR::IRArithOp::Operator::LT || op == IR::IRArithOp::Operator::LE || op == IR::IRArithOp::Operator::GT ||
         op == IR::IRArithOp::Operator::GE) &&
        (ctx.isInt(LType) || ctx.isFloat(LType)) && (ctx.isInt(RType) || ctx.isFloat(RType)))
        typeOfNode = ctx.getTypeByTypeName("bool");

    if ((op == IR::IRArithOp::Operator::EQ || op == IR::IRArithOp::Operator::NE) && LType->canConvertToThisType(RType))
        typeOfNode = ctx.getTypeByTypeName("bool");

    if ((op == IR::IRArithOp::Operator::PLUS || op == IR::IRArithOp::Operator::MINUS ||
         op == IR::IRArithOp::Operator::MUL || op == IR::IRArithOp::Operator::DIV) &&
        (ctx.isInt(LType) || ctx.isFloat(LType)) && (ctx.isInt(RType) || ctx.isFloat(RType))) {

        if (ctx.isInt(LType) && ctx.isInt(RType))
            typeOfNode = ctx.greaterInt(LType, RType);
        else
            typeOfNode = ctx.getTypeByTypeName("float");

    }

    if ((op == IR::IRArithOp::Operator::AND || op == IR::IRArithOp::Operator::OR) && ctx.isBool(LType) &&
        ctx.isBool(RType))
        typeOfNode = ctx.getTypeByTypeName("bool");

    if (!typeOfNode)
        throw std::invalid_argument("ERROR. Not allowed operation above types.");

    auto new_exprs = ctx.convertTypesEq(std::move(left), std::move(right));
    left = std::move(new_exprs.first);
    right = std::move(new_exprs.second);

    return typeOfNode;
}

std::set<std::string> AST::ASTBinaryOperator::getVarNames() {
    auto res = left->getVarNames();
    res.merge(right->getVarNames());
    return res;
}

bool AST::ASTUnaryOperator::hasAddress() {
    if (op == DEREFER)
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

    if (!func_type->getReturnArg().empty()) {
        name_for_return_arg = func_type->getReturnArg();
        type_for_return_arg = func_type->getReturnArgType();
    }

    std::vector<Type *> args;
    for (auto &i: arg)
        args.emplace_back(i->checker(ctx));


    if (!func_type->compareArgs(args))
        throw std::invalid_argument("ERROR. Mismatch in types between passed argument type and expected type.");

    auto list_of_func_args = func_type->getArgs();

    for (auto i = 0; i < list_of_func_args.size(); ++i)
        arg[i] = ctx.convertTypeTo(std::move(arg[i]), list_of_func_args[i]);

    typeOfNode = func_type->getReturn();
    return typeOfNode;
}

std::set<std::string> AST::ASTFunctionCall::getVarNames() {
    std::set < std::string > res;
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
        if (!field_type->canConvertToThisType(i.second->checker(ctx)))
            throw std::invalid_argument("ERROR. Field and assigned type mismatch.");
    }

    typeOfNode = struct_type;
    return typeOfNode;
}

std::set<std::string> AST::ASTStruct::getVarNames() {
    std::set < std::string > res;
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
    auto var = ctx.getInfByVarName(name);
    if (!var)
        throw std::invalid_argument("ERROR. Unknown name (" + name + ") for a variable");
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
    if (name.size() != value.size() && value.size() != 0)
        throw std::invalid_argument("ERROR. Assignment number mismatch.");

    for (int i = 0; i < name.size(); ++i)
        res.emplace_back(name[i], this, value.size() > 0 ? value[i]->getVarNames() : std::set < std::string > (),
                         value.size() > 0 ? value[i].get() : nullptr, type.get());


    return res;
}

Type *AST::ASTVarDeclaration::checker(Context &ctx) {
    if (type && !type->typeOfNode)
        type->checker(ctx);

    // if func with many return values
    // create a tmp variables-structure, which will saves the result
    // and take the result for the variables as the assigment of the members
    if (value.size() == 1 && dynamic_cast<SeqType *>(value[0]->checker(ctx)) &&
        dynamic_cast<ASTFunctionCall *>(value[0].get())) {
        auto seq = dynamic_cast<SeqType *>(value[0]->typeOfNode);

        if (name.size() != seq->getTypes().size())
            throw std::invalid_argument(
                    "ERROR. Number of variables and number of values returned by functions is not the same.");

        if (type)
            for (auto &i: seq->getTypes())
                if (!type->typeOfNode->canConvertToThisType(i))
                    throw std::invalid_argument("ERROR. Type of var and type of expression different.");

        auto name_for_struct = "tmp" + std::to_string(ctx.tmp_count++);
        auto name_list = {name_for_struct};
        std::vector<std::unique_ptr<ASTExpression>> tmp;
        tmp.emplace_back(std::move(value[0]));


        function_dispatch = std::make_unique<ASTVarDeclaration>(name_list, std::move(tmp));
        function_dispatch->dispatcher = true;

        ctx.addIntoNameSpace(name_for_struct, seq->corespStruct);
        value.clear();

        for (unsigned long long i = 0; i < name.size(); ++i)
            value.emplace_back(std::make_unique<AST::ASTMemberAccess>(std::make_unique<ASTVar>(name_for_struct),
                                                                      std::to_string(i)));

    }

    if (name.size() != value.size() && value.size() != 0)
        throw std::invalid_argument("ERROR. Assignment number mismatch.");

    for (auto i = 0; i < name.size(); ++i) {

        if (ctx.checkIfNameExist(name[i]))
            throw std::invalid_argument("ERROR. Such name (" + name[i] + ") for var already exists.");

        if (!type) {
            if (!value[i]->checker(ctx))
                throw std::invalid_argument("ERROR. Variable cannot be null type");
            ctx.addIntoNameSpace(name[i], value[i]->typeOfNode);
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
            if (!value[i]->checker(ctx))
                throw std::invalid_argument("ERROR. Variable cannot be null type");
            ctx.addIntoNameSpace(name[i], value[i]->typeOfNode, true);
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
    if (ctx.body_of_function) {
        ctx.body_of_function = false;
        for (auto &i: statements)
            i->checker(ctx);
        return nullptr;
    }
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

    for (auto i = 0; i < return_value.size(); ++i) {
        if (return_value[i]->typeOfNode != ctx.return_type[i]) {
            auto cast = std::make_unique<AST::ASTCast>();

            cast->setTypeCastTo(ctx.return_type[i]);
            cast->setChild(std::move(return_value[i]));

            return_value[i] = std::move(cast);
        }
    }

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
    // check basics. Variables have addresses, and not constants
    for (auto &i: variable) {
        i->checker(ctx);
        if (!i->hasAddress())
            throw std::invalid_argument("ERROR. Var is not assignable type.");
        if (i->isConst())
            throw std::invalid_argument("ERROR. Cannot assign new value to a const.");
    }


    // if func with many return values
    // create a new variable structure in which will store the result
    // and assigned the members of it to the variables
    if (value.size() == 1 && dynamic_cast<SeqType *>(value[0]->checker(ctx)) &&
        dynamic_cast<ASTFunctionCall *>(value[0].get())) {
        auto seq = dynamic_cast<SeqType *>(value[0]->typeOfNode);

        if (variable.size() != seq->getTypes().size())
            throw std::invalid_argument(
                    "ERROR. Number of variables and number of values returned by functions is not the same.");

        for (unsigned long long i = 0; i < variable.size(); ++i)
            if (!variable[i]->checker(ctx)->canConvertToThisType(seq->getTypes()[i]))
                throw std::invalid_argument("ERROR. Var and assigned value not the same types");

        auto name = "tmp" + std::to_string(ctx.tmp_count++);
        auto name_list = {name};
        std::vector<std::unique_ptr<ASTExpression>> tmp;
        tmp.emplace_back(std::move(value[0]));


        function_dispatch = std::make_unique<ASTVarDeclaration>(name_list, std::move(tmp));
        function_dispatch->dispatcher = true;

        ctx.addIntoNameSpace(name, seq->corespStruct);
        value.clear();

        for (unsigned long long i = 0; i < variable.size(); ++i)
            value.emplace_back(std::make_unique<AST::ASTMemberAccess>(std::make_unique<ASTVar>(name),
                                                                      std::to_string(i)));

    }

    if (variable.size() != value.size())
        throw std::invalid_argument("ERROR. Number of variables and number of values are mismatched.");

    for (int i = 0; i < variable.size(); ++i) {
        auto var_type = variable[i]->typeOfNode;

        auto val_type = value[i]->checker(ctx);
        if (!var_type || !val_type || !var_type->canConvertToThisType(val_type))
            throw std::invalid_argument("ERROR. Var and assigned value not the same types");

        if (!(type == ASSIGN || dynamic_cast<IntType *>(val_type) || dynamic_cast<FloatType *>(val_type)))
            throw std::invalid_argument(
                    "ERROR. Cannot perform math operations on non integer and non float values and variables");


        value[i] = ctx.convertTypeTo(std::move(value[i]), variable[i]->typeOfNode);
    }

    return nullptr;
}

Type *AST::ASTScan::checker(Context &ctx) {
    expression->checker(ctx);
    PointerType *pointer = dynamic_cast<PointerType *>(expression->typeOfNode);
    if (!pointer)
        throw std::invalid_argument("ERROR. Cannot scan into not pointer type.");

    if (!dynamic_cast<IntType *>(pointer->getBase()) && !dynamic_cast<FloatType *>(pointer->getBase()))
        throw std::invalid_argument("ERROR. Cannot scan non integer or non float type.");

    return nullptr;
}

Type *AST::ASTPrint::checker(Context &ctx) {
    expression->checker(ctx);

    if (!dynamic_cast<PointerType *>(expression->typeOfNode) && !dynamic_cast<IntType *>(expression->typeOfNode) &&
        !dynamic_cast<FloatType *>(expression->typeOfNode))
        throw std::invalid_argument("ERROR. Cannot print non pointer, float or integer type");
    return nullptr;
}

Type *AST::Function::checker(Context &ctx) {
    // go inside
    ctx.goDeeper();

    // if type is method -- add it as the argument
    if (type_of_method) {
        ctx.addIntoNameSpace(inner_name, type_of_method->checker(ctx));
        if (dynamic_cast<StructType *>(type_of_method->typeOfNode)) {
            type_of_method = std::make_unique<AST::ASTTypePointer>(std::move(type_of_method));
            type_of_method->checker(ctx);
            was_arg_modified.emplace_back(true);
        } else
            was_arg_modified.emplace_back(false);
    }

    // add all the rest arguments into space
    for (auto &i: params)
        for (auto &j: i.first) {
            ctx.addIntoNameSpace(j, i.second->checker(ctx));
            if (dynamic_cast<StructType *>(i.second->typeOfNode)) {
                i.second = std::make_unique<AST::ASTTypePointer>(std::move(i.second));
                i.second->checker(ctx);
                was_arg_modified.emplace_back(true);
            } else
                was_arg_modified.emplace_back(false);

        }

    ctx.return_type.clear();


    for (auto &i: return_type)
        ctx.return_type.emplace_back(i->checker(ctx));


    ctx.body_of_function = true;
    body->checker(ctx);

    ctx.goUp();

    return nullptr;
}

void AST::Function::globalPreInit(Context &ctx) {
    // create a function type
    auto new_function_type = std::make_unique<FunctionType>();

    if (return_type.size() == 1) {
        // just one type to return
        new_function_type->setReturn(return_type[0]->checker(ctx));
        typeOfNode = return_type[0]->typeOfNode;
    } else if (return_type.size() > 1) {
        // more than one to return
        // in that case return is going to be accepted as the argument and values it going to be written in it
        auto new_seq = std::make_unique<SeqType>();
        auto same_struct = std::make_unique<StructType>();
        for (unsigned long long i = 0; i < return_type.size(); ++i) {
            new_seq->addType(return_type[i]->checker(ctx));
            same_struct->addNewField(std::to_string(i), return_type[i]->typeOfNode);
        }

        new_seq->corespStruct = dynamic_cast<StructType *>(ctx.addType(std::move(same_struct)));
        typeOfNode = new_seq->corespStruct;
        new_function_type->setReturn(ctx.addType(std::move(new_seq)));
    }

    // if the return type is a structure -- remake it for the argument
    if (auto st = dynamic_cast<StructType *>(typeOfNode)) {
        type_for_return_arg = ctx.getPointer(typeOfNode);
        name_for_return = "_tmp_nameForReturn";
        new_function_type->setReturnArg(name_for_return);
        new_function_type->setReturnArgType(st);
    }



    // check arguments and it to the function type
    for (auto &i: params)
        for (auto &j: i.first)
            new_function_type->addParam(i.second->checker(ctx));

    //if it is method
    if (type_of_method) {

        std::string new_name;

        StructType *structType;
        type_of_method->checker(ctx);
        // if method passed by value
        if (dynamic_cast<StructType *>(type_of_method->typeOfNode)) {
            structType = dynamic_cast<StructType *>(type_of_method->typeOfNode);
            new_function_type->setIsPointer(false);
            new_name = *type_of_method->getDependencies().begin();

        }

        // if method passed by reference
        if (dynamic_cast<PointerType *>(type_of_method->typeOfNode) &&
            dynamic_cast<StructType *>(dynamic_cast<PointerType *>(type_of_method->typeOfNode)->getBase())) {
            structType = dynamic_cast<StructType *>(dynamic_cast<PointerType *>(type_of_method->typeOfNode)->getBase());
            new_function_type->setIsPointer(true);
            new_name = *(dynamic_cast<ASTTypePointer *>(type_of_method.get())->getValue())->getDependencies().begin();
        }

        if (!structType)
            throw std::invalid_argument("ERROR. Cannot create method for such type.");

        new_function_type->setInnerName(new_name);
        structType->addNewField(name, ctx.addType(std::move(new_function_type)));
        name = "_" + new_name + "_" + name;
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

    // end of the declaration in global
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

void AST::ASTCast::setChild(std::unique_ptr<AST::ASTExpression> &&new_expr) {
    expr = std::move(new_expr);
}

void AST::ASTCast::setTypeCastTo(Type *new_type) {
    cast_to = new_type;
    typeOfNode = new_type;
}

Type *AST::ASTCast::checker(Context &ctx) {
    throw std::invalid_argument("Never should be it.");
}

std::set<std::string> AST::ASTCast::getVarNames() {

}