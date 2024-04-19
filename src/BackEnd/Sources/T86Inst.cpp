#include "T86Inst.h"


T86::Instruction::Instruction(Opcode new_op, std::unique_ptr<Operand> &&new_l,
                              std::unique_ptr<Operand> &&new_r) {
    op = new_op;
    if (new_l)
        first = std::move(new_l);
    if (new_r)
        second = std::move(new_r);
}

void T86::Instruction::print(std::ostream &oss) {
    oss << magic_enum::enum_name(op) << ' ';
    if (first)
        oss << first->toString() << ' ';
    if (second)
        oss << ", " << second->toString() << ' ';
}

T86::Instruction *T86::T86Program::emplaceInstruction(Instruction &&inst) {
    program.push_back(std::move(inst));
    return &program.back();
}

void T86::T86Program::print(std::ostream &oss) {
    for (size_t i = 0; i < program.size(); ++i) {
        oss << std::to_string(i) << ' ';
        program[i].print(oss);
        oss << std::endl;
    }
}

std::size_t T86::T86Program::getNumberOfInstructions() {
    return program.size();
}

void T86::Context::AllocPlaceOnStack() {
    placeOnStack = 1;
}

long long T86::Context::getCurrentPlaceOnStack() {
    return placeOnStack++;
}

void T86::Context::addInstruction(T86::Instruction &&new_instruction) {
    program.emplaceInstruction(std::move(new_instruction));
}

void T86::Context::printProgramConsole() {
    program.print(std::cout);
}

std::size_t T86::Context::getNumberOfInstructions() {
    return program.getNumberOfInstructions();
}

void T86::Context::addWhereFunctionBegin(std::string name) {
    placeForCall[name] = program.getNumberOfInstructions();
}