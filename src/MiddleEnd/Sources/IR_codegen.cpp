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

void IR::StructConst::generateT86(T86::Context &) {

}

std::unique_ptr<T86::Operand> IR::StructConst::getOperand(T86::Context &) {

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
                                        std::make_unique<T86::Memory>(where->getOperand(ctx))));
}

std::unique_ptr<T86::Operand> IR::IRLoad::getOperand(T86::Context &ctx) {
    return std::make_unique<T86::Register>(inner_number - ctx.offset_of_function);
}

void IR::IRStore::generateT86(T86::Context &ctx) {
    // TODO something more smarter
    // T86 does not support MOV [], []

    if (dynamic_cast<T86::Memory*>(what->getOperand(ctx).get())) {
        ctx.addInstruction(T86::Instruction(T86::Instruction::MOV, std::make_unique<T86::Register>(
                what->inner_number - ctx.offset_of_function), what->getOperand(ctx)));

        ctx.addInstruction(T86::Instruction(T86::Instruction::MOV, std::make_unique<T86::Memory>(where->getOperand(ctx)),
                std::make_unique<T86::Register>(what->inner_number - ctx.offset_of_function)));

        return;
    }
    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV, std::make_unique<T86::Memory>(where->getOperand(ctx)),
            what->getOperand(ctx)));
}

void IR::IRAlloca::generateT86(T86::Context &ctx) {

    place_on_stack = -ctx.getCurrentPlaceOnStack(type->size());
}

std::unique_ptr<T86::Operand> IR::IRAlloca::getOperand(T86::Context &ctx) {
    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,std::make_unique<T86::Register>(inner_number - ctx.offset_of_function), std::make_unique<T86::Register>(T86::Register::BP)));
    ctx.addInstruction(T86::Instruction(T86::Instruction::ADD,std::make_unique<T86::Register>(inner_number - ctx.offset_of_function), std::make_unique<T86::IntImmediate>(place_on_stack)));
    return std::make_unique<T86::Register>(inner_number - ctx.offset_of_function);

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
    // if it returns something -- return
    if (res)
        ctx.addInstruction(T86::Instruction(T86::Instruction::MOV, std::make_unique<T86::Memory>(
                                                    std::make_unique<T86::Register>(T86::Register::BP, 2 + ctx.allocated_space_for_arguments)),
                                            res->getOperand(ctx)));

    // remove from stack all allocated variables
    ctx.addInstruction(T86::Instruction(T86::Instruction::ADD, std::make_unique<T86::Register>(T86::Register::SP),
                                        std::make_unique<T86::IntImmediate>(ctx.allocated_space_for_variables)));


    ctx.addInstruction(T86::Instruction(T86::Instruction::POP, std::make_unique<T86::Register>(T86::Register::BP)));
    ctx.addInstruction(T86::Instruction(T86::Instruction::RET));
}

void IR::IRCall::generateT86(T86::Context &ctx) {
    // reserv space to return value(1) if return value exists
    ctx.addInstruction(T86::Instruction(T86::Instruction::SUB, std::make_unique<T86::Register>(T86::Register::SP),
                                        std::make_unique<T86::IntImmediate>(1)));

    // push arguments to the stack
    for (long long i = arguments.size() - 1; i >= 0; --i)
        ctx.addInstruction(T86::Instruction(T86::Instruction::PUSH, arguments[i]->getOperand(ctx)));

    auto place_to_jump_func = std::make_unique<T86::IntImmediate>();
    ctx.addFunctionCall(name_of_function, place_to_jump_func.get());

    // jump
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

void IR::IRMembCall::generateT86(T86::Context &ctx) {
    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
            where->getOperand(ctx)));

    // calc, place of the member
    long long off = 0;
    auto fields = typeOfWhere->getFields();
    for (auto i = 0; i < what; ++i)
        off += fields[i].second->size();

    ctx.addInstruction(T86::Instruction(T86::Instruction::ADD,std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
            std::make_unique<T86::IntImmediate>(-what)));
}

std::unique_ptr<T86::Operand> IR::IRMembCall::getOperand(T86::Context &ctx) {
    return std::make_unique<T86::Register>(inner_number - ctx.offset_of_function);
}

void IR::IRElemCall::generateT86(T86::Context &ctx) {
    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
                                        where->getOperand(ctx)));
    long long off = typeOfElem->size() * what;
    ctx.addInstruction(T86::Instruction(T86::Instruction::ADD,std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
            std::make_unique<T86::IntImmediate>(-what)));
}

std::unique_ptr<T86::Operand> IR::IRElemCall::getOperand(T86::Context &ctx) {
    return std::make_unique<T86::Register>(inner_number - ctx.offset_of_function);
}

void IR::IRCast::generateT86(T86::Context &ctx) {
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

void IR::IRMemCopy::generateT86(T86::Context &ctx) {
    for (auto i = 0; i < size; ++i) {
        ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,
                                            std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
                                                    from->getOperand(ctx)));

        ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,
                                            std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),
                                            std::make_unique<T86::Memory>(std::make_unique<T86::Register>(inner_number - ctx.offset_of_function,-i))));

        ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,
                                            std::make_unique<T86::Register>(0),
                                            to->getOperand(ctx)));
        ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,std::make_unique<T86::Memory>(std::make_unique<T86::Register>(0,-i)),std::make_unique<T86::Register>(inner_number - ctx.offset_of_function)));
    }
}

void IR::IRScan::generateT86(T86::Context &ctx) {
    ctx.addInstruction(T86::Instruction(T86::Instruction::GETCHAR,
                                        std::make_unique<T86::Register>(inner_number - ctx.offset_of_function)));
    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,
                                        std::make_unique<T86::Memory>(link->getOperand(ctx)),
                                        std::make_unique<T86::Register>(inner_number - ctx.offset_of_function)));
}

void IR::IRPrint::generateT86(T86::Context &ctx) {
    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV,std::make_unique<T86::Register>(inner_number - ctx.offset_of_function),value->getOperand(ctx)));
    ctx.addInstruction(T86::Instruction(T86::Instruction::PUTNUM,std::make_unique<T86::Register>(inner_number - ctx.offset_of_function)));
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

    ctx.startFunction();

    for(auto &i : arguments)
        ctx.allocated_space_for_arguments += i->size();

    for (auto &i : allocas)
        ctx.allocated_space_for_variables += dynamic_cast<IRAlloca*>(i.get())->getType()->size();


    // push basic pointer of pr function
    ctx.addInstruction(T86::Instruction(T86::Instruction::PUSH, std::make_unique<T86::Register>(T86::Register::BP)));
    ctx.addInstruction(T86::Instruction(T86::Instruction::MOV, std::make_unique<T86::Register>(T86::Register::BP),
                                        std::make_unique<T86::Register>(T86::Register::SP)));


    // allocate place for variables
    if (!allocas.empty())
        ctx.addInstruction(T86::Instruction(T86::Instruction::SUB, std::make_unique<T86::Register>(T86::Register::SP),
                                        std::make_unique<T86::IntImmediate>(ctx.allocated_space_for_variables)));


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