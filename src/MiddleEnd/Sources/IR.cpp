#include "IR.h"

void IR::IRLine::addUse(IRLine *line) {
    uses.push_back(line);
}

void IR::IRGlobal::addValue(std::unique_ptr<IRLine> &&new_value) {
    value = std::move(new_value);
}

void IR::IRGlobal::addType(Type *new_type) {
    type = new_type;
}

void IR::IRBlock::addLine(std::unique_ptr<IRLine> &&new_command) {
    block.emplace_back(std::move(new_command));
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







