#include "IR.h"

std::unique_ptr<T86::Operand> IR::Value::getOperand(T86::Context &) {
    throw std::invalid_argument("Never should happened");
}

void IR::IntConst::generateT86(T86::Context &ctx) {

}

std::unique_ptr<T86::Operand> IR::IntConst::getOperand(T86::Context &) {
    return std::make_unique<T86::IntImmediate>(value);
}

void IR::DoubleConst::generateT86(T86::Context &ctx) {

}

std::unique_ptr<T86::Operand> IR::DoubleConst::getOperand(T86::Context &) {
    return std::make_unique<T86::DoubleImmediate>(value);
}

void IR::Nullptr::generateT86(T86::Context &ctx) {

}

std::unique_ptr<T86::Operand> IR::Nullptr::getOperand(T86::Context &) {
    return std::make_unique<T86::IntImmediate>();
}

void IR::IRArithOp::generateT86(T86::Context &ctx) {
    if (op == PLUS || op == MINUS || op == MUL || op == DIV || op == BINAND || op == AND || op == BINOR || op == OR ||
        op == XOR) {
        T86::Instruction::Opcode opcode_for_instruction;
        // float operations
        if (dynamic_cast<FloatType *>(result_type)) {
            if (op == PLUS)
                opcode_for_instruction = T86::Instruction::FADD;
            else if (op == MINUS)
                opcode_for_instruction = T86::Instruction::FSUB;
            else if (op == MUL)
                opcode_for_instruction = T86::Instruction::FMUL;
            else
                opcode_for_instruction = T86::Instruction::FDIV;

        } else {
            // int operation
            if (op == PLUS)
                opcode_for_instruction = T86::Instruction::ADD;
            else if (op == MINUS)
                opcode_for_instruction = T86::Instruction::SUB;
            else if (op == MUL)
                opcode_for_instruction = T86::Instruction::MUL;
            else
                opcode_for_instruction = T86::Instruction::DIV;
        }
        if (op == BINAND || op == AND)
            opcode_for_instruction = T86::Instruction::AND;
        if (op == BINOR || op == OR)
            opcode_for_instruction = T86::Instruction::OR;
        if (op == XOR)
            opcode_for_instruction = T86::Instruction::XOR;

        ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,
                                            std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
                                            left->getOperand(ctx)));
        ctx.addInstruction(T86::Instruction(opcode_for_instruction,
                                            std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
                                            right->getOperand(ctx)));
        return;
    }
    // TODO mod




    //EQ, NE, GT, GE, LT, LE,

    T86::Instruction::Opcode type_of_compare;
    if (dynamic_cast<FloatType *>(result_type))
        type_of_compare = T86::Instruction::FCMP;
    else
        type_of_compare = T86::Instruction::CMP;

    ctx.addInstruction(T86::Instruction(type_of_compare, left->getOperand(ctx), right->getOperand(ctx)));


    T86::Instruction::Opcode opcode_of_compare;
    if (op == EQ)
        opcode_of_compare = T86::Instruction::JE;
    else if (op == NE)
        opcode_of_compare = T86::Instruction::JNE;
    else if (op == GT)
        opcode_of_compare = T86::Instruction::JG;
    else if (op == GE)
        opcode_of_compare = T86::Instruction::JGE;
    else if (op == LT)
        opcode_of_compare = T86::Instruction::JL;
    else
        opcode_of_compare = T86::Instruction::JLE;
    // Jump to true
    ctx.addInstruction(T86::Instruction(opcode_of_compare,
                                        std::make_unique<T86::IntImmediate>(ctx.getNumberOfInstructions() + 3)));

    // if false

    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,
                                        std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
                                        std::make_unique<T86::IntImmediate>(0)));

    ctx.addInstruction(T86::Instruction(T86::Instruction::JMP,
                                        std::make_unique<T86::IntImmediate>(ctx.getNumberOfInstructions() + 2)));

    // if true.

    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,
                                        std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
                                        std::make_unique<T86::IntImmediate>(1)));

}

std::unique_ptr<T86::Operand> IR::IRArithOp::getOperand(T86::Context &ctx) {
    return std::make_unique<T86::Register>(inner_number - ctx.offset_of_function);
}

void IR::IRLabel::generateT86(T86::Context &ctx) {
    ctx.addLabelPlace(inner_number);
}

void IR::IRLoad::generateT86(T86::Context &ctx) {
    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,
                                        std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
                                        where->getOperand(ctx)));
}

std::unique_ptr<T86::Operand> IR::IRLoad::getOperand(T86::Context &ctx) {
    return std::make_unique<T86::Register>(inner_number - ctx.offset_of_function);
}

void IR::IRStore::generateT86(T86::Context &ctx) {
    // todo something more smarter
    if (dynamic_cast<T86::Memory*>(what->getOperand(ctx).get())) {
        ctx.addInstruction(T86::Instruction(T86::Instruction::MOV, std::make_unique<T86::Register>(
                what->inner_number - ctx.offset_of_function), what->getOperand(ctx)));
        ctx.addInstruction(T86::Instruction(T86::Instruction::MOV, where->getOperand(ctx), std::make_unique<T86::Register>(
                what->inner_number - ctx.offset_of_function)));
        return;
    }
    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV, where->getOperand(ctx), what->getOperand(ctx)));
}

void IR::IRAlloca::generateT86(T86::Context &ctx) {

    place_on_stack = -ctx.getCurrentPlaceOnStack();

    if (basicValue)
        ctx.addInstruction(
                T86::Instruction(T86::Instruction::MOV,
                                 std::make_unique<T86::Memory>(
                                         std::make_unique<T86::Register>(T86::Register::BP, place_on_stack)),
                                 basicValue->getOperand(ctx)));

}

std::unique_ptr<T86::Operand> IR::IRAlloca::getOperand(T86::Context &) {
    return std::make_unique<T86::Memory>(std::make_unique<T86::Register>(T86::Register::BP, place_on_stack));
}

void IR::IRGlobal::generateT86(T86::Context &ctx) {

}

std::unique_ptr<T86::Operand> IR::IRGlobal::getOperand(T86::Context &) {

}

void IR::IRBranch::generateT86(T86::Context &ctx) {
    auto place_to_jmp_if_true = std::make_unique<T86::IntImmediate>();
    auto place_to_jmp_if_false = std::make_unique<T86::IntImmediate>();

    ctx.addJumpToLabel(brT->inner_number, place_to_jmp_if_true.get());
    if (!result) {
        ctx.addInstruction(T86::Instruction(T86::Instruction::JMP, std::move(place_to_jmp_if_true)));
        return;
    }

    ctx.addJumpToLabel(brNT->inner_number, place_to_jmp_if_false.get());
    ctx.addInstruction(
            T86::Instruction(T86::Instruction::CMP, result->getOperand(ctx), std::make_unique<T86::IntImmediate>(1)));

    ctx.addInstruction(T86::Instruction(T86::Instruction::JE, std::move(place_to_jmp_if_true)));
    ctx.addInstruction(T86::Instruction(T86::Instruction::JMP, std::move(place_to_jmp_if_false)));

}

void IR::IRRet::generateT86(T86::Context &ctx) {
    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV, std::make_unique<T86::Memory>(
                                                std::make_unique<T86::Register>(T86::Register::BP, 2 + ctx.number_of_arguments_in_function)),
                                        res->getOperand(ctx)));
  //  ctx.addInstruction(T86::Instruction(T86::Instruction(T86::Instruction::PUTNUM,res->getOperand(ctx))));
    ctx.addInstruction(T86::Instruction(T86::Instruction::ADD, std::make_unique<T86::Register>(T86::Register::SP),
                                        std::make_unique<T86::IntImmediate>(ctx.number_of_arguments_in_function)));

    ctx.addInstruction(T86::Instruction(T86::Instruction::POP, std::make_unique<T86::Register>(T86::Register::BP)));
    ctx.addInstruction(T86::Instruction(T86::Instruction::RET));
}

void IR::IRCall::generateT86(T86::Context &ctx) {
    // reserv space to return value(1) if return value exists
    // todo later for structures
    ctx.addInstruction(T86::Instruction(T86::Instruction::SUB, std::make_unique<T86::Register>(T86::Register::SP),
                                        std::make_unique<T86::IntImmediate>(1)));

    // push values to a stack

    for (long long i = arguments.size() - 1; i >= 0; --i)
        ctx.addInstruction(T86::Instruction(T86::Instruction::PUSH, arguments[i]->getOperand(ctx)));

    auto place_to_jump_func = std::make_unique<T86::IntImmediate>();
    ctx.addFunctionCall(name_of_function, place_to_jump_func.get());

    ctx.addInstruction(T86::Instruction(T86::Instruction::CALL, std::move(place_to_jump_func)));


    // delete all arguments from stack
    ctx.addInstruction(T86::Instruction(T86::Instruction::ADD, std::make_unique<T86::Register>(T86::Register::SP),
                                        std::make_unique<T86::IntImmediate>(arguments.size())));

    // pop return value
    ctx.addInstruction(T86::Instruction(T86::Instruction::POP,
                                        std::make_unique<T86::Register>(inner_number - ctx.offset_of_function)));

}

std::unique_ptr<T86::Operand> IR::IRCall::getOperand(T86::Context &ctx) {
    return std::make_unique<T86::Register>(inner_number - ctx.offset_of_function);
}

void IR::IRCast::generateT86(T86::Context &ctx) {
    // TODO anything else???
    T86::Instruction::Opcode opcode_for_instruction;
    if (dynamic_cast<IntType *>(from) && dynamic_cast<FloatType *>(to))
        opcode_for_instruction = T86::Instruction::EXT;
    if (dynamic_cast<FloatType *>(from) && dynamic_cast<IntType *>(to))
        opcode_for_instruction = T86::Instruction::NRW;

    ctx.addInstruction(T86::Instruction(opcode_for_instruction,
                                        std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
                                        expr->getOperand(ctx)));
}

std::unique_ptr<T86::Operand> IR::IRCast::getOperand(T86::Context &ctx) {
    return std::make_unique<T86::Register>(inner_number - ctx.offset_of_function);
}

void IR::IRFuncArg::generateT86(T86::Context &ctx) {
    throw std::invalid_argument("Should not happen???");
}

std::unique_ptr<T86::Operand> IR::IRFuncArg::getOperand(T86::Context &ctx) {
    return std::make_unique<T86::Memory>(std::make_unique<T86::Register>(T86::Register::BP, order_of_arg + 2));
}

void IR::IRFunc::generateT86(T86::Context &ctx) {
    // pre init for any function
    ctx.addFunctionPlace(this->name);
    ctx.offset_of_function = inner_number;
    ctx.number_of_arguments_in_function = arguments.size();

    ctx.addInstruction(T86::Instruction(T86::Instruction::PUSH, std::make_unique<T86::Register>(T86::Register::BP)));
    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV, std::make_unique<T86::Register>(T86::Register::BP),
                                        std::make_unique<T86::Register>(T86::Register::SP)));

    // allocating space on stack for local variables
    // TODO later for structures
    if (!allocas.empty())
        ctx.addInstruction(T86::Instruction(T86::Instruction::SUB, std::make_unique<T86::Register>(T86::Register::SP),
                                        std::make_unique<T86::IntImmediate>(allocas.size())));
    ctx.AllocPlaceOnStack();

    for (auto &i: allocas)
        i->generateT86(ctx);

    // Body. Generate code for inner

    for (auto &i: body)
        i->generateT86(ctx);
}

void IR::IRProgram::generateT86(T86::Context &ctx) {
    // pre init

    // TODO later -- .data segment

    // Pre init of any program;
    // call main and afet ret -- halt
    auto place_of_main = std::make_unique<T86::IntImmediate>();
    ctx.addFunctionCall("main", place_of_main.get());

    ctx.addInstruction(T86::Instruction(T86::Instruction::CALL, std::move(place_of_main)));
    ctx.addInstruction(T86::Instruction(T86::Instruction::HALT));

    for (auto &i: functions)
        i->generateT86(ctx);

    ctx.finishCallsAndJmps();
}

void IR::IRComment::generateT86(T86::Context &ctx) {

}