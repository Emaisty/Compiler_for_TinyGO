#ifndef COMPILER_T86INST_H
#define COMPILER_T86INST_H

#include <variant>
#include <memory>
#include <vector>
#include <map>
#include <iostream>

#include "magic_enum.hpp"

#include "Operands.h"

namespace T86 {

    class Instruction {
    public:
        enum Opcode {
            MOV,
            LEA,
            ADD,
            SUB,
            INC,
            DEC,
            NEG,
            MUL,
            DIV,
            IMUL,
            IDIV,
            FADD,
            FSUB,
            FMUL,
            FDIV,
            AND,
            OR,
            XOR,
            LSH,
            RSH,
            CMP,
            FCMP,
            JMP,
            LOOP,
            JZ,
            JNZ,
            JE,
            JNE,
            JG,
            JGE,
            JL,
            JLE,
            JA,
            JAE,
            JB,
            JBE,
            JO,
            JNO,
            JS,
            JNS,
            CALL,
            RET,
            PUSH,
            FPUSH,
            POP,
            FPOP,
            PUTCHAR,
            PUTNUM,
            GETCHAR,
            EXT,
            NRW,
            HALT,
            NOP
        };

        Instruction(Opcode, std::unique_ptr<Operand> && = nullptr, std::unique_ptr<Operand> && = nullptr);

        void print(std::ostream &);

    private:
        Opcode op;

        // sometimes 0 (HELT, RET), sometimes 1 (CALL, PUSH)
        std::unique_ptr<Operand> first, second;

    };


    class T86Program {
    public:
        Instruction *emplaceInstruction(Instruction &&);

        void print(std::ostream &);

        std::size_t getNumberOfInstructions();

    private:
        // .text segment
        std::vector<Instruction> program;

        // also need to add .data segment
    };

    class Context {
    public:
        long long offset_of_function = 0;

        void AllocPlaceOnStack();

        long long getCurrentPlaceOnStack();

        void addOperand(long long, std::unique_ptr<Operand> &&);

        Operand *getOperand(long long);

        void addInstruction(Instruction &&);

        void printProgramConsole();

        std::size_t getNumberOfInstructions();

        void addWhereFunctionBegin(std::string);

    private:
        T86Program program;

        std::map<long long, std::unique_ptr<Operand>> instructionToOperand;

        long long placeOnStack = 1;

        std::map<std::string, std::vector<Instruction *>> notFinishedCalls;

        std::map<std::string, size_t> placeForCall;

    };

}

#endif //COMPILER_T86INST_H
