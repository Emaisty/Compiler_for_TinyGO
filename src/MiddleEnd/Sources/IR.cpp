#include "IR.h"

void IR::IRLine::addUse(IRLine *line) {
    uses.push_back(line);
}

void IR::IRArithOp::addChildren(std::unique_ptr<IR::IRLine> &&l, std::unique_ptr<IR::IRLine> &&r) {
    left = std::move(l);
    right = std::move(r);
}

void IR::IRStore::addValue(std::unique_ptr<IRLine> &&new_value) {
    new_value->addUse(this);
    value = std::move(new_value);
}

void IR::IRStore::setLink(IRLine *link) {
    var_to_store = link;
}

void IR::IRAlloca::addType(Type *new_type) {
    type = new_type;
}

void IR::IRGlobal::addValue(std::unique_ptr<IRLine> &&new_value) {
    new_value->addUse(this);
    value = std::move(new_value);
}

void IR::IRGlobal::addType(Type *new_type) {
    type = new_type;
}

void IR::IRBlock::addLine(std::unique_ptr<IRLine> &&new_command) {
    block.emplace_back(std::move(new_command));
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

void IR::IRRet::addRetVal(std::unique_ptr<IRLine> &&new_ret) {
    res = std::move(new_ret);
}

void IR::IRFunc::addReturnType(Type *new_type) {
    return_type = new_type;
}

void IR::IRFunc::addTypeOfArg(Type *new_type) {
    type_of_args.emplace_back(new_type);
}

void IR::IRFunc::addBody(std::unique_ptr<IRLine> &&new_body) {
    body = std::move(new_body);
}

void IR::IRProgram::addLine(std::unique_ptr<IRLine> &&new_command) {
    IRLines.emplace_back(std::move(new_command));
}







