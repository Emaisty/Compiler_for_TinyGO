#include "T86Inst.h"


T86::Instruction::Instruction(Opcode new_op, std::unique_ptr<Operand> &&new_l,
                              std::unique_ptr<Operand> &&new_r) {
    op = new_op;
    if (new_l)
        first = std::move(new_l);
    if (new_r)
        second = std::move(new_r);
}

void T86::Instruction::addOperand(std::unique_ptr<Operand> &&new_operand) {
    if (!first) {
        first = std::move(new_operand);
        return;
    }

    second = std::move(new_operand);
}

T86::Instruction::Opcode T86::Instruction::getOpcode() {
    return op;
}

void T86::Instruction::print(std::ostream &oss) {
    oss << magic_enum::enum_name(op) << ' ';
    if (first)
        oss << first->toString();
    if (second)
        oss << ',' << second->toString();
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

void T86::Context::addFunctionCall(std::string name, IntImmediate *place_to_call) {
    notFinishedCalls[name].emplace_back(place_to_call);
}

void T86::Context::addFunctionPlace(std::string name) {
    placeForCall.emplace(name, program.getNumberOfInstructions());
}

void T86::Context::addJumpToLabel(long long label_number, IntImmediate *place_to_jump) {
    notFinishedJumps[label_number].emplace_back(place_to_jump);
}

void T86::Context::addLabelPlace(long long label_number) {
    placeForJumps.emplace(label_number, program.getNumberOfInstructions());
}

void T86::Context::finishCallsAndJmps() {
    for (auto &[i, j]: placeForCall)
        for (auto &place: notFinishedCalls[i])
            place->addValue(j);

    for (auto &[i, j]: placeForJumps)
        for (auto &place: notFinishedJumps[i])
            place->addValue(j);

}