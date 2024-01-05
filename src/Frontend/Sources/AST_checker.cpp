#include "AST.h"

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

void AST::Context::addType(std::string new_name, std::unique_ptr<Type> &&new_type) {
    checkIfTypeExist(new_name);
    type_space.emplace_back(new_name, std::move(new_type), level);
}

void AST::Context::addIntoNameSpace(std::string new_name, Type *new_type) {
    checkIfNameExist(new_name);
    name_space.emplace_back(new_name, new_type, level);
}

/*


std::unique_ptr<AST::ASTType> AST::ASTNullExpr::getType(const std::vector<ItemInNameSpace> &name_space) const {
    return std::make_unique<AST::ASTTypeNull>();
}

void AST::ASTNullExpr::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    return;
}

std::unique_ptr<AST::ASTType> AST::ASTBinaryOperator::getType(const std::vector<ItemInNameSpace> &name_space) const {
    if (op == AND || op == OR || op == EQ || op == NE || op == GE || op == GT || op == LE || op == LT)
        return std::make_unique<AST::ASTTypeBool>();

    if (AST::ASTTypeInt() == left->getType(name_space) && AST::ASTTypeInt() == right->getType(name_space))
        return std::make_unique<AST::ASTTypeInt>();

    return std::make_unique<AST::ASTTypeFloat>();
}

void AST::ASTBinaryOperator::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    left->checker(name_space, inf);
    right->checker(name_space, inf);
    if (!(AST::ASTTypeInt() == left->getType(name_space) || AST::ASTTypeFloat() == left->getType(name_space) ||
          AST::ASTTypeBool() == left->getType(name_space)))
        throw std::invalid_argument("ERROR. Not supported type for binary operator");
    if (!(AST::ASTTypeInt() == right->getType(name_space) || AST::ASTTypeFloat() == right->getType(name_space) ||
          AST::ASTTypeBool() == right->getType(name_space)))
        throw std::invalid_argument("ERROR. Not supported type for binary operator");
    if (op == MOD &&
        !(AST::ASTTypeInt() == left->getType(name_space) || AST::ASTTypeInt() == right->getType(name_space)))
        throw std::invalid_argument("ERROR. Operator mod does not support anything except INT");

    if ((op == BINAND || op == BINOR) &&
        !(AST::ASTTypeInt() == left->getType(name_space) || AST::ASTTypeInt() == right->getType(name_space)))
        throw std::invalid_argument("ERROR. Bin operator & and | does not support anything except INT");
    if (!(op == AND || op == OR) &&
        (AST::ASTTypeBool() == left->getType(name_space) || AST::ASTTypeBool() == right->getType(name_space)))
        throw std::invalid_argument("ERROR. Unsupported bin operation for bool type");
}

std::unique_ptr<AST::ASTType> AST::ASTUnaryOperator::getType(const std::vector<ItemInNameSpace> &name_space) const {
    if (op == AST::ASTUnaryOperator::NOT)
        return std::make_unique<AST::ASTTypeBool>();
    return value->getType(name_space);
}

void AST::ASTUnaryOperator::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    value->checker(name_space, inf);

    if (!(AST::ASTTypeInt() == value->getType(name_space) || AST::ASTTypeFloat() == value->getType(name_space) ||
          AST::ASTTypeBool() == value->getType(name_space) || AST::ASTTypePointer() == value->getType(name_space)))
        throw std::invalid_argument("ERROR. Not supported type for unary operator");

    if ((op == POSTINC || op == POSTDEC || op == PREINC || op == PREDEC) &&
        !value->getType(name_space)->addressable())
        throw std::invalid_argument("ERROR. Try to change (inc or dec) value without address");

    if (AST::ASTTypePointer() == value->getType(name_space) && op != NOT)
        throw std::invalid_argument("ERROR. Forbidden arithmetic on pointers");


}

std::unique_ptr<AST::ASTType> AST::ASTFunctionCall::getType(const std::vector<ItemInNameSpace> &name_space) const {
//TODO
}

void AST::ASTFunctionCall::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    //TODO
}

std::unique_ptr<AST::ASTType> AST::ASTMemberAccess::getType(const std::vector<ItemInNameSpace> &name_space) const {
    //TODO
}

void AST::ASTMemberAccess::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
//    name->checker(name_space, inf);
//    member->checker(name_space, inf);
//    auto struc = dynamic_cast<ASTTypeStruct *>(name->getType(name_space).get());
//    auto memb = dynamic_cast<ASTVar *>(name->getType(name_space).get());
//    if (!memb)
//        throw std::invalid_argument("ERROR. Wrong type of member call");
//
//    if (struc) {
//        struc->findField(memb->getName());
//        return;
//    }
//
//    auto point_to_stuct = dynamic_cast<ASTTypePointer *>(name->getType(name_space).get());
//    if (!point_to_stuct)
//        throw std::invalid_argument("ERROR. Cannot take member from not struct type");
//
//    auto tmp_struc = dynamic_cast<ASTTypeStruct *>(point_to_stuct->getValue().get());
//    if (!tmp_struc)
//        throw std::invalid_argument("ERROR. Cannot take member from not struct type");
//    tmp_struc->findField(memb->getName());
    //TODO

}

std::unique_ptr<AST::ASTType> AST::ASTGetPointer::getType(const std::vector<ItemInNameSpace> &name_space) const {
    return std::make_unique<ASTTypePointer>(var->getType(name_space));
}

void AST::ASTGetPointer::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    var->checker(name_space, inf);
    if (var->getType(name_space)->constable() || !var->getType(name_space)->addressable())
        throw std::invalid_argument("ERROR. Cannot take address of const value");
}

std::unique_ptr<AST::ASTType> AST::ASTGetValue::getType(const std::vector<ItemInNameSpace> &name_space) const {
    auto val = dynamic_cast<ASTTypePointer *>(var->getType(name_space).get());
    if (!val)
        throw std::invalid_argument("!!!!!!!!!!!!!!!!!!!!! SHOULD NEVER SEE THIS");
    return val->getValue();
}

void AST::ASTGetValue::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    var->checker(name_space, inf);
    if (var->getType(name_space) != AST::ASTTypePointer())
        throw std::invalid_argument("ERROR. Cannot inderect non-pointer type");
}

std::unique_ptr<AST::ASTType> AST::ASTIntNumber::getType(const std::vector<ItemInNameSpace> &name_space) const {
    return std::make_unique<ASTTypeInt>();
}

void AST::ASTIntNumber::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    return;
}

std::unique_ptr<AST::ASTType> AST::ASTFloatNumber::getType(const std::vector<ItemInNameSpace> &name_space) const {
    return std::make_unique<ASTTypeFloat>();
}

void AST::ASTFloatNumber::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    return;
}

std::unique_ptr<AST::ASTType> AST::ASTBoolNumber::getType(const std::vector<ItemInNameSpace> &name_space) const {
    return std::make_unique<ASTTypeBool>();
}

void AST::ASTBoolNumber::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    return;
}

std::unique_ptr<AST::ASTType> AST::ASTStruct::getType(const std::vector<ItemInNameSpace> &name_space) const {
    return type->clone();
}

void AST::ASTStruct::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    if (*type != std::make_unique<ASTTypeStruct>())
        throw std::invalid_argument("!!!!!!!!!!!!!! U SHOULD NEVER SEE THIS");
    for (auto &i: values) {
        auto struc = dynamic_cast<ASTTypeStruct *>(type.get());
        if (struc->findField(i.first) != i.second->getType(name_space))
            throw std::invalid_argument("ERROR. Mismatch of types");
    }
}

std::unique_ptr<AST::ASTType> AST::ASTVar::getType(const std::vector<ItemInNameSpace> &name_space) const {
    for (auto &i: name_space)
        if (i.name == name) {
            if (i.type_of_var == AST::ItemInNameSpace::VAR)
                return i.type->set_addressability(true)->set_constability(false);
            return i.type->set_addressability(true);
        }

}

void AST::ASTVar::checker(Context &inf) {
    for (auto &i: name_space)
        if (i.name == name)
            return;

    throw std::invalid_argument("ERROR. Unknown name for var or function");

}

void AST::ASTTypeDeclaration::checkerGlobal(Context &context) {
    context.checkIfNameInSpace(name);
    context.name_space.emplace_back(name, type, 0, false);
}

void AST::ASTTypeDeclaration::checker(Context &context) {
    if (context.level != 0) {
        context.checkIfNameInSpace(name);
        context.name_space.emplace_back(name, type, context.level, false);
    }
}

void AST::ASTVarDeclaration::checkerGlobal(Context &context) {
    context.checkIfNameInSpace(name);
    if (type)
        context.name_space.emplace_back(name, type, 0, false);
    else {
        auto tmp = value->getType(name_space);
        name_space.emplace_back(name, tmp, 0, AST::ItemInNameSpace::VAR);
    }
}


void AST::ASTVarDeclaration::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    if (inf.level != 0) {
        name_in_space(name, name_space);
        if (type)
            name_space.emplace_back(name, type, inf.level, AST::ItemInNameSpace::VAR);
        else {
            auto tmp = value->getType(name_space);
            name_space.emplace_back(name, tmp, 0, AST::ItemInNameSpace::VAR);
        }
    }

    if (value)
        type = value->getType(name_space);

    if (type && value) {
        value->checker(name_space, inf);
        if (!(*type == value->getType(name_space)))
            throw std::invalid_argument("Error. Diff type of declaration and given expr");
    }
}

void AST::ASTConstDeclaration::checkerGlobal(std::vector<ItemInNameSpace> &name_space) {
    name_in_space(name, name_space);
    if (type)
        name_space.emplace_back(name, type, 0, AST::ItemInNameSpace::CONST);
    else {
        auto tmp = value->getType(name_space);
        name_space.emplace_back(name, tmp, 0, AST::ItemInNameSpace::CONST);
    }
}

void AST::ASTConstDeclaration::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    if (inf.level != 0) {
        name_in_space(name, name_space);
        if (type)
            name_space.emplace_back(name, type, inf.level, AST::ItemInNameSpace::CONST);
        else {
            auto tmp = value->getType(name_space);
            name_space.emplace_back(name, tmp, 0, AST::ItemInNameSpace::CONST);
        }
    }
    value->checker(name_space, inf);
    if (!type)
        type = value->getType(name_space);

    if (!(*type == value->getType(name_space)))
        throw std::invalid_argument("Error. Diff type of declaration and given expr");
    if (!value->getType(name_space)->constable())
        throw std::invalid_argument("Error. Non-const value for const var");
}

void AST::ASTBlock::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    inf.goDeeper();
    for (auto &i: statements)
        i->checker(name_space, inf);

    for (int i = name_space.size() - 1; i >= 0; --i)
        if (name_space[i].level == inf.level)
            name_space.erase(name_space.end() - 1, name_space.end());

    inf.goUp();

}

void AST::ASTBreak::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    if (!inf.in_loop)
        throw std::invalid_argument("ERROR. Try to break not in clause");
}

void AST::ASTContinue::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    if (!inf.in_loop)
        throw std::invalid_argument("ERROR. Try to continue not in clause");
}

void AST::ASTReturn::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
//    if (!(*inf.return_type == return_value->getType(name_space)))
//        throw std::invalid_argument("ERROR. Return type and return expr mismatches");
}

void AST::ASTSwitch::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    expr->checker(name_space, inf);

    inf.goDeeper();
    for (auto &i: cases) {
        i.first->checker(name_space, inf);
        i.second->checker(name_space, inf);
    }

    for (int i = name_space.size() - 1; i >= 0; --i)
        if (name_space[i].level == inf.level)
            name_space.erase(name_space.end() - 1, name_space.end());

    inf.goUp();

}

void AST::ASTIf::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    expr->checker(name_space, inf);

    inf.goDeeper();

    if_clause->checker(name_space, inf);

    if (else_clause)
        else_clause->checker(name_space, inf);

    for (int i = name_space.size() - 1; i >= 0; --i)
        if (name_space[i].level == inf.level)
            name_space.erase(name_space.end() - 1, name_space.end());

    inf.goUp();
}

void AST::ASTFor::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    inf.goDeeper();
    for (auto &i: init_clause)
        i->checker(name_space, inf);
    for (auto &i: iterate_clause)
        i->checker(name_space, inf);

    body->checker(name_space, inf);

    for (int i = name_space.size() - 1; i >= 0; --i)
        if (name_space[i].level == inf.level)
            name_space.erase(name_space.end() - 1, name_space.end());

    inf.goUp();
}

void AST::ASTAssign::checker(std::vector<ItemInNameSpace> &name_space, Context &inf) {
    variable->checker(name_space, inf);
    value->checker(name_space, inf);
    if (!(*variable->getType(name_space) == value->getType(name_space)))
        throw std::invalid_argument("Error. Diff type of var and value");

    if (!variable->getType(name_space)->addressable())
        throw std::invalid_argument("Error. Try to change value without address");

    if (variable->getType(name_space)->constable())
        throw std::invalid_argument("ERROR. Try to change const var");

}


void AST::Function::checkerBody(std::vector<ItemInNameSpace> &name_space) {
    Context inf;
    inf.level = 1;
    inf.return_type = return_type->clone();

    //add params into name space
    for (auto &i: params) {
        name_in_space(i.first, name_space);
        name_space.emplace_back(i.first, i.second, inf.level, AST::ItemInNameSpace::VAR);
    }

    // check body
    body->checker(name_space, inf);

    //delete params from name space
    for (int i = name_space.size() - 1; i >= 0; --i)
        if (name_space[i].level == inf.level)
            name_space.erase(name_space.end() - 1, name_space.end());

}


void AST::Function::checkerName(std::vector<ItemInNameSpace> &name_space) {
    name_in_space(name, name_space);

    ItemInNameSpace var(name, return_type, 0, AST::ItemInNameSpace::FUNC);
    for (auto &i: params)
        var.addParam(i.second);

    name_space.push_back(std::move(var));
}


void AST::Program::checker() {
    Context context;

    for (auto &i: declarations)
        i->checkerGlobal(context);

    for (auto &i: functions)
        i->checkerName(context);

    for (auto &i: declarations)
        i->checker(context);

    for (auto &i: functions)
        i->checkerBody(context);
}*/

void AST::ASTTypeDeclaration::globalPreInit(Context &ctx) {
    ctx.checkIfTypeExist(name[0]);

    typ
}

Type *AST::ASTTypeDeclaration::checker(Context &ctx) {
    return nullptr;
}

void AST::ASTVarDeclaration::globalPreInit(Context &ctx) {

}

Type *AST::ASTVarDeclaration::checker(Context &ctx) {

    return nullptr;
}

void AST::ASTConstDeclaration::globalPreInit(Context &ctx) {

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
        if (!return_value[i]->checker(ctx)->compare(ctx.return_type[i]))
            throw std::invalid_argument("ERROR. Not matching return value");

    return nullptr;
}

Type *AST::ASTSwitch::checker(Context &ctx) {
    auto type_of_stat = expr->checker(ctx);
    for (auto &i: cases) {
        ctx.goDeeper(false, true);
        if (!type_of_stat->compare(i.first->checker(ctx)))
            throw std::invalid_argument("ERROR. Not comparable types between case and expression.");
        i.second->checker(ctx);
        ctx.goUp();
    }

    return nullptr;
}

Type *AST::ASTIf::checker(Context &ctx) {
    auto type_of_clause = if_clause->checker(ctx);
    if (!type_of_clause->compare(ctx.getTypeByTypeName("bool")))
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
    if (!type_of_clause->compare(ctx.getTypeByTypeName("bool")))
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
        if (!var_type || !val_type || !var_type->compare(val_type))
            throw std::invalid_argument("ERROR. Var and assigned value not the same types");
    }

    return nullptr;
}

Type *AST::Function::checker(Context &ctx) {
    ctx.goDeeper();
    for (auto &i: params)
        i->checker(ctx);

    for (auto &i: return_type)
        ctx.return_type.emplace_back(i->checker(ctx));

    body->checker(ctx);

    ctx.goUp();

    return nullptr;
}


Type *AST::Program::checker(Context &ctx) {
    for (auto &i: declarations)
        i->checker(ctx);

    for (auto &i: functions)
        i->checker(ctx);

    return nullptr;
}