#include "AST.h"


void name_in_space(const std::string &name, const std::vector<AST::Variable> &name_space) {
    for (auto &i: name_space)
        if (i.name == name)
            throw std::invalid_argument("ERROR. This name already exists");
}

void AST::Info::goDeeper() {
    level += 1;
    pr_loop_status.push(in_loop);
    in_loop = true;
}

void AST::Info::goUp() {
    level -= 1;
    in_loop = pr_loop_status.top();
}

std::unique_ptr<AST::ASTType> AST::ASTNullExpr::getType(const std::vector<Variable> &name_space) const {
    return std::make_unique<AST::ASTTypeNull>();
}

void AST::ASTNullExpr::checker(std::vector<Variable> &name_space, Info &inf) {
    return;
}

std::unique_ptr<AST::ASTType> AST::ASTBinaryOperator::getType(const std::vector<Variable> &name_space) const {
    if (op == AND || op == OR || op == EQ || op == NE || op == GE || op == GT || op == LE || op == LT)
        return std::make_unique<AST::ASTTypeBool>();

    if (AST::ASTTypeInt() == left->getType(name_space) && AST::ASTTypeInt() == right->getType(name_space))
        return std::make_unique<AST::ASTTypeInt>();

    return std::make_unique<AST::ASTTypeFloat>();
}

void AST::ASTBinaryOperator::checker(std::vector<Variable> &name_space, Info &inf) {
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

std::unique_ptr<AST::ASTType> AST::ASTUnaryOperator::getType(const std::vector<Variable> &name_space) const {
    if (op == AST::ASTUnaryOperator::NOT)
        return std::make_unique<AST::ASTTypeBool>();
    return value->getType(name_space);
}

void AST::ASTUnaryOperator::checker(std::vector<Variable> &name_space, Info &inf) {
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

std::unique_ptr<AST::ASTType> AST::ASTFunctionCall::getType(const std::vector<Variable> &name_space) const {
//TODO
}

void AST::ASTFunctionCall::checker(std::vector<Variable> &name_space, Info &inf) {
    //TODO
}

std::unique_ptr<AST::ASTType> AST::ASTMemberAccess::getType(const std::vector<Variable> &name_space) const {
    //TODO
}

void AST::ASTMemberAccess::checker(std::vector<Variable> &name_space, Info &inf) {
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

std::unique_ptr<AST::ASTType> AST::ASTGetPointer::getType(const std::vector<Variable> &name_space) const {
    return std::make_unique<ASTTypePointer>(var->getType(name_space));
}

void AST::ASTGetPointer::checker(std::vector<Variable> &name_space, Info &inf) {
    var->checker(name_space, inf);
    if (var->getType(name_space)->constable() || !var->getType(name_space)->addressable())
        throw std::invalid_argument("ERROR. Cannot take address of const value");
}

std::unique_ptr<AST::ASTType> AST::ASTGetValue::getType(const std::vector<Variable> &name_space) const {
    auto val = dynamic_cast<ASTTypePointer *>(var->getType(name_space).get());
    if (!val)
        throw std::invalid_argument("!!!!!!!!!!!!!!!!!!!!! SHOULD NEVER SEE THIS");
    return val->getValue();
}

void AST::ASTGetValue::checker(std::vector<Variable> &name_space, Info &inf) {
    var->checker(name_space, inf);
    if (var->getType(name_space) != AST::ASTTypePointer())
        throw std::invalid_argument("ERROR. Cannot inderect non-pointer type");
}

std::unique_ptr<AST::ASTType> AST::ASTIntNumber::getType(const std::vector<Variable> &name_space) const {
    return std::make_unique<ASTTypeInt>();
}

void AST::ASTIntNumber::checker(std::vector<Variable> &name_space, Info &inf) {
    return;
}

std::unique_ptr<AST::ASTType> AST::ASTFloatNumber::getType(const std::vector<Variable> &name_space) const {
    return std::make_unique<ASTTypeFloat>();
}

void AST::ASTFloatNumber::checker(std::vector<Variable> &name_space, Info &inf) {
    return;
}

std::unique_ptr<AST::ASTType> AST::ASTBoolNumber::getType(const std::vector<Variable> &name_space) const {
    return std::make_unique<ASTTypeBool>();
}

void AST::ASTBoolNumber::checker(std::vector<Variable> &name_space, Info &inf) {
    return;
}

std::unique_ptr<AST::ASTType> AST::ASTStruct::getType(const std::vector<Variable> &name_space) const {
    return type->clone();
}

void AST::ASTStruct::checker(std::vector<Variable> &name_space, Info &inf) {
    if (*type != std::make_unique<ASTTypeStruct>())
        throw std::invalid_argument("!!!!!!!!!!!!!! U SHOULD NEVER SEE THIS");
    for (auto &i: values) {
        auto struc = dynamic_cast<ASTTypeStruct *>(type.get());
        if (struc->findField(i.first) != i.second->getType(name_space))
            throw std::invalid_argument("ERROR. Mismatch of types");
    }
}

std::unique_ptr<AST::ASTType> AST::ASTVar::getType(const std::vector<Variable> &name_space) const {
    for (auto &i: name_space)
        if (i.name == name) {
            if (i.type_of_var == AST::Variable::VAR)
                return i.type->set_addressability(true)->set_constability(false);
            return i.type->set_addressability(true);
        }

}

void AST::ASTVar::checker(std::vector<Variable> &name_space, Info &inf) {
    for (auto &i: name_space)
        if (i.name == name)
            return;

    throw std::invalid_argument("ERROR. Unknown name for var or function");

}

void AST::ASTTypeDeclaration::checkerGlobal(std::vector<Variable> &name_space) {
    name_in_space(name, name_space);
    name_space.emplace_back(name, type, 0, AST::Variable::TYPE);
}

void AST::ASTTypeDeclaration::checker(std::vector<Variable> &name_space, Info &inf) {
    if (inf.level != 0) {
        name_in_space(name, name_space);
        name_space.emplace_back(name, type, inf.level, AST::Variable::TYPE);
    }
}

void AST::ASTVarDeclaration::checkerGlobal(std::vector<Variable> &name_space) {
    name_in_space(name, name_space);
    if (type)
        name_space.emplace_back(name, type, 0, AST::Variable::VAR);
    else {
        auto tmp = value->getType(name_space);
        name_space.emplace_back(name, tmp, 0, AST::Variable::VAR);
    }
}


void AST::ASTVarDeclaration::checker(std::vector<Variable> &name_space, Info &inf) {
    if (inf.level != 0) {
        name_in_space(name, name_space);
        if (type)
            name_space.emplace_back(name, type, inf.level, AST::Variable::VAR);
        else {
            auto tmp = value->getType(name_space);
            name_space.emplace_back(name, tmp, 0, AST::Variable::VAR);
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

void AST::ASTConstDeclaration::checkerGlobal(std::vector<Variable> &name_space) {
    name_in_space(name, name_space);
    if (type)
        name_space.emplace_back(name, type, 0, AST::Variable::CONST);
    else {
        auto tmp = value->getType(name_space);
        name_space.emplace_back(name, tmp, 0, AST::Variable::CONST);
    }
}

void AST::ASTConstDeclaration::checker(std::vector<Variable> &name_space, Info &inf) {
    if (inf.level != 0) {
        name_in_space(name, name_space);
        if (type)
            name_space.emplace_back(name, type, inf.level, AST::Variable::CONST);
        else {
            auto tmp = value->getType(name_space);
            name_space.emplace_back(name, tmp, 0, AST::Variable::CONST);
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

void AST::ASTBlock::checker(std::vector<Variable> &name_space, Info &inf) {
    inf.goDeeper();
    for (auto &i: statements)
        i->checker(name_space, inf);

    for (int i = name_space.size() - 1; i >= 0; --i)
        if (name_space[i].level == inf.level)
            name_space.erase(name_space.end() - 1, name_space.end());

    inf.goUp();

}

void AST::ASTBreak::checker(std::vector<Variable> &name_space, Info &inf) {
    if (!inf.in_loop)
        throw std::invalid_argument("ERROR. Try to break not in clause");
}

void AST::ASTContinue::checker(std::vector<Variable> &name_space, Info &inf) {
    if (!inf.in_loop)
        throw std::invalid_argument("ERROR. Try to continue not in clause");
}

void AST::ASTReturn::checker(std::vector<Variable> &name_space, Info &inf) {
    if (!(*inf.return_type == return_value->getType(name_space)))
        throw std::invalid_argument("ERROR. Return type and return expr mismatches");
}

void AST::ASTSwitch::checker(std::vector<Variable> &name_space, Info &inf) {
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

void AST::ASTIf::checker(std::vector<Variable> &name_space, Info &inf) {
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

void AST::ASTFor::checker(std::vector<Variable> &name_space, Info &inf) {
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

void AST::ASTAssign::checker(std::vector<Variable> &name_space, Info &inf) {
    variable->checker(name_space, inf);
    value->checker(name_space, inf);
    if (!(*variable->getType(name_space) == value->getType(name_space)))
        throw std::invalid_argument("Error. Diff type of var and value");

    if (!variable->getType(name_space)->addressable())
        throw std::invalid_argument("Error. Try to change value without address");

    if (variable->getType(name_space)->constable())
        throw std::invalid_argument("ERROR. Try to change const var");

}


void AST::Function::checkerBody(std::vector<Variable> &name_space) {
    Info inf;
    inf.level = 1;
    inf.return_type = return_type->clone();

    //add params into name space
    for (auto &i: params) {
        name_in_space(i.first, name_space);
        name_space.emplace_back(i.first, i.second, inf.level, AST::Variable::VAR);
    }

    // check body
    body->checker(name_space, inf);

    //delete params from name space
    for (int i = name_space.size() - 1; i >= 0; --i)
        if (name_space[i].level == inf.level)
            name_space.erase(name_space.end() - 1, name_space.end());

}


void AST::Function::checkerName(std::vector<Variable> &name_space) {
    name_in_space(name, name_space);

    Variable var(name, return_type, 0, AST::Variable::FUNC);
    for (auto &i: params)
        var.addParam(i.second);

    name_space.push_back(std::move(var));
}


void AST::Program::checker() {
    std::vector<Variable> name_space;
    Info inf;

    for (auto &i: declarations)
        i->checkerGlobal(name_space);

    for (auto &i: functions)
        i->checkerName(name_space);

    for (auto &i: declarations)
        i->checker(name_space, inf);

    for (auto &i: functions)
        i->checkerBody(name_space);
}