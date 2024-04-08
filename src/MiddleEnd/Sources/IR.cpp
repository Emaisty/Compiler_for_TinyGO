#include "IR.h"

void IR::IRLine::addUse(IRLine *line) {
    uses.push_back(line);
}


void IR::Context::setNewVar(std::string new_name, IRLine *link) {
    variables[new_name] = link;
}

IR::IRLine *IR::Context::getVar(std::string name) {
    return variables[name];
}

void IR::IRArithOp::addChildren(std::unique_ptr<IR::IRLine> &&l, std::unique_ptr<IR::IRLine> &&r) {
    left = std::move(l);
    right = std::move(r);
}

void IR::IRArithOp::setType(Operator nop) {
    op = nop;
}

void IR::IRArithOp::print(std::ostream &, PrintHelper &) {

}

IR::IRIntValue::IRIntValue(long long val) {
    value = val;
}

void IR::IRIntValue::print(std::ostream &, PrintHelper &) {

}

IR::IRDoubleValue::IRDoubleValue(double val) {
    value = val;
}

void IR::IRDoubleValue::print(std::ostream &, PrintHelper &) {

}

void IR::IRNullValue::print(std::ostream &, PrintHelper &) {

}

void IR::IRLabel::print(std::ostream &, PrintHelper &) {

}

void IR::IRLoad::addLink(IRLine *link) {
    from_whom_load = link;
}

void IR::IRLoad::print(std::ostream &, PrintHelper &) {

}

void IR::IRStore::addValue(std::unique_ptr<IRLine> &&new_value) {
    new_value->addUse(this);
    value = std::move(new_value);
}

void IR::IRStore::setLink(IRLine *link) {
    var_to_store = link;
}

void IR::IRStore::print(std::ostream &, PrintHelper &) {

}

void IR::IRAlloca::addType(Type *new_type) {
    type = new_type;
}

void IR::IRAlloca::print(std::ostream &, PrintHelper &) {

}

void IR::IRGlobal::addValue(std::unique_ptr<IRLine> &&new_value) {
    new_value->addUse(this);
    value = std::move(new_value);
}

void IR::IRGlobal::addType(Type *new_type) {
    type = new_type;
}

void IR::IRGlobal::print(std::ostream &, PrintHelper &) {

}

void IR::IRBlock::addLine(std::unique_ptr<IRLine> &&new_command) {
    block.emplace_back(std::move(new_command));
}

void IR::IRBlock::print(std::ostream &, PrintHelper &) {

}

void IR::IRCMP::print(std::ostream &, PrintHelper &) {

}

void IR::IRBranch::addCond(std::unique_ptr<IRLine> &&new_cond) {
    result = std::move(new_cond);
}

void IR::IRBranch::addBrTaken(IRLine *link) {
    brT = link;
}

void IR::IRBranch::addBrNTaken(IRLine *link) {
    brNT = link;
}

void IR::IRBranch::print(std::ostream &, PrintHelper &) {

}

void IR::IRRet::addRetVal(std::unique_ptr<IRLine> &&new_ret) {
    res = std::move(new_ret);
}

void IR::IRRet::print(std::ostream &, PrintHelper &) {

}

void IR::IRCall::print(std::ostream &, PrintHelper &) {

}

void IR::IRCast::addExpr(std::unique_ptr<IRLine> &&new_expr) {
    expr = std::move(new_expr);
}

void IR::IRCast::addTypeTo(Type *new_type) {
    to = new_type;
}

void IR::IRCast::print(std::ostream &, PrintHelper &) {

}

void IR::IRFunc::addReturnType(Type *new_type) {
    return_type = new_type;
}

void IR::IRFunc::addTypeOfArg(Type *new_type, std::string name) {
    type_of_args.emplace_back(new_type, name);
}

void IR::IRFunc::addBody(std::unique_ptr<IRLine> &&new_body) {
    body = std::move(new_body);
}

void IR::IRFunc::print(std::ostream &oss, PrintHelper &helper) {

}

void IR::IRProgram::addLine(std::unique_ptr<IRLine> &&new_command) {
    IRLines.emplace_back(std::move(new_command));
}

void IR::IRProgram::print(std::ostream &oss, PrintHelper &helper) {
    for (auto &i: IRLines)
        i->print(oss, helper);
}