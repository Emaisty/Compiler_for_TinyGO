#include "IR.h"

void IR::Value::addUse(Value *use) {
    uses.emplace_back(use);
}

void IR::Context::addFunction(std::string, IRFunc *) {

}

IR::IRFunc *IR::Context::getFunction(std::string) {

}

void IR::Context::addVariable(std::string, Value *) {

}

IR::Value *IR::Context::getVariable(std::string) {

}

void IR::Context::setBuilder(std::vector<std::unique_ptr<Value>> *) {

}

void IR::IntConst::print(std::ostream &oss) {
    oss << "   " << "%" << inner_number << " = create int constant " << value << std::endl;
}

void IR::DoubleConst::print(std::ostream &oss) {
    oss << "   " << "%" << inner_number << " = create double constant " << value << std::endl;
}

void IR::IRArithOp::addChildren(Value *n_left, Value *n_right) {
    left = n_left;
    right = n_right;
}

void IR::IRArithOp::setType(Operator new_operator) {
    op = new_operator;
}

void IR::IRArithOp::print(std::ostream &) {

}

void IR::IRLabel::print(std::ostream &oss) {
    oss << "label %" << inner_number << ":" << std::endl;
}

void IR::IRLoad::addLoadFrom(Value *new_link) {
    where = new_link;
}

void IR::IRLoad::print(std::ostream &oss) {
    oss << "   " << "%" << inner_number << " = load from:%" << where->inner_number;
}

void IR::IRStore::addStoreWhere(Value *new_link) {
    where = new_link;
}

void IR::IRStore::addStoreWhat(Value *new_link) {
    what = new_link;
}

void IR::IRStore::print(std::ostream &oss) {
    oss << "   " << "store - what: %" << what->inner_number << " ; where: %" << where->inner_number << std::endl;
}

void IR::IRAlloca::addType(Type *new_type) {
    type = new_type;
}

void IR::IRAlloca::print(std::ostream &oss) {
    // TODO basic value
    oss << "   " << "%" << inner_number << " = alloca '" << type->toString() << "'; value is " << std::endl;
}

void IR::IRGlobal::addValue(Value *n_value) {
    value = n_value;
}

void IR::IRGlobal::addType(Type *new_type) {
    type = new_type;
}

void IR::IRGlobal::print(std::ostream &oss) {
    // TODO basic value
    oss << "%" << inner_number << " = global '" << type->toString() << "' ; value is ";
}

void IR::IRBranch::addCond(Value *new_val) {
    result = new_val;
}

void IR::IRBranch::addBrTaken(Value *taken_label) {
    brT = taken_label;
}

void IR::IRBranch::addBrNTaken(Value *Ntaken_label) {
    brNT = Ntaken_label;
}

void IR::IRBranch::print(std::ostream &oss) {
    if (result) {
        oss << "   " << "cond jmp Cond: %" << result->inner_number << "; If true: %" << brT->inner_number
            << "; false: %" << brNT->inner_number << std::endl;
        return;
    }
    oss << "   " << "jmp %" << brT->inner_number << std::endl;
}

void IR::IRRet::addRetVal(Value *new_val) {
    res = new_val;
}

void IR::IRRet::print(std::ostream &oss) {
    oss << "   " << "ret %" << res->inner_number << std::endl;
}

void IR::IRCall::addLinkToFunc(Value *new_func) {
    function = new_func;
}

void IR::IRCall::addArg(Value *new_arg) {
    arguments.emplace_back(new_arg);
}

void IR::IRCall::print(std::ostream &oss) {
    auto f_name = dynamic_cast<IR::IRFunc *>(function);
    oss << "   " << "call %" << f_name->getName() << " with arguments : (";
    for (auto i = 0; i < arguments.size(); ++i) {
        oss << "%" << arguments[i]->inner_number;

        if (i != arguments.size() - 1)
            oss << ", ";
    }
    oss << ")" << std::endl;
}

void IR::IRCast::addExpr(Value *new_expr) {
    expr = new_expr;
}

void IR::IRCast::addTypeTo(Type *new_type) {
    to = new_type;
}

void IR::IRCast::print(std::ostream &oss) {
    oss << "   " << "%" << inner_number << " = cast what: %" << expr->inner_number << "; to: '" << to->toString() << "'"
        << std::endl;
}

void IR::IRFuncArg::addType(Type *new_type) {
    type = new_type;
}

void IR::IRFuncArg::print(std::ostream &oss) {
    oss << "'" << type->toString() << "' %" << inner_number;
}

void IR::IRFunc::addReturnType(Type *new_type) {
    return_type = new_type;
}

void IR::IRFunc::addArg(std::unique_ptr<IRFuncArg> &&new_arg) {
    arguments.emplace_back(std::move(new_arg));
}

void IR::IRFunc::addInstToBody(std::unique_ptr<Value> &&new_instruction) {
    body.emplace_back(std::move(new_instruction));
}

void IR::IRFunc::setName(std::string new_name) {
    name = new_name;
}

std::string IR::IRFunc::getName() {
    return name;
}

void IR::IRFunc::print(std::ostream &oss) {
    oss << "function %" << name << "; arguments: (";
    for (auto i = 0; i < arguments.size(); ++i) {
        arguments[i]->print(oss);
        if (i != arguments.size() - 1)
            oss << ", ";
    }
    oss << "); return: '" << return_type->toString() << "' {" << std::endl;
    for (auto &i: body)
        i->print(oss);
    oss << "}" << std::endl << std::endl << std::endl;

}

void IR::IRProgram::addGlobDecl(std::unique_ptr<Value> &&new_decl) {
    globalDecl.emplace_back(std::move(new_decl));
}

void IR::IRProgram::addFunc(std::unique_ptr<Value> &&new_function) {
    functions.emplace_back(std::move(new_function));
}

void IR::IRProgram::print(std::ostream &oss) {
    for (auto &i: globalDecl)
        i->print(oss);

    for (auto &i: functions)
        i->print(oss);
}