#ifndef COMPILER_T86INST_H
#define COMPILER_T86INST_H

#include <variant>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <iostream>

//#include "magic_enum.hpp"

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

        inline static const std::map<Opcode, std::string> opcode_to_str = {
                {MOV,     "MOV"},
                {LEA,     "LEA"},
                {ADD,     "ADD"},
                {SUB,     "SUB"},
                {INC,     "INC"},
                {DEC,     "DEC"},
                {NEG,     "NEG"},
                {MUL,     "MUL"},
                {DIV,     "DIV"},
                {IMUL,    "IMUL"},
                {IDIV,    "IDIV"},
                {FADD,    "FADD"},
                {FSUB,    "FSUB"},
                {FMUL,    "FMUL"},
                {FDIV,    "FDIV"},
                {AND,     "AND"},
                {OR,      "OR"},
                {XOR,     "XOR"},
                {LSH,     "LSH"},
                {RSH,     "RSH"},
                {CMP,     "CMP"},
                {FCMP,    "FCMP"},
                {JMP,     "JMP"},
                {LOOP,    "LOOP"},
                {JZ,      "JZ"},
                {JNZ,     "JNZ"},
                {JE,      "JE"},
                {JNE,     "JNE"},
                {JG,      "JG"},
                {JGE,     "JGE"},
                {JL,      "JL"},
                {JLE,     "JLE"},
                {JA,      "JA"},
                {JAE,     "JAE"},
                {JB,      "JB"},
                {JBE,     "JBE"},
                {JO,      "JO"},
                {JNO,     "JNO"},
                {JS,      "JS"},
                {JNS,     "JNS"},
                {CALL,    "CALL"},
                {RET,     "RET"},
                {PUSH,    "PUSH"},
                {FPUSH,   "FPUSH"},
                {POP,     "POP"},
                {FPOP,    "FPOP"},
                {PUTCHAR, "PUTCHAR"},
                {PUTNUM,  "PUTNUM"},
                {GETCHAR, "GETCHAR"},
                {EXT,     "EXT"},
                {NRW,     "NRW"},
                {HALT,    "HALT"},
                {NOP,     "NOP"}
        };

        Instruction(Opcode, std::unique_ptr<Operand> && = nullptr, std::unique_ptr<Operand> && = nullptr);

        void addOperand(std::unique_ptr<Operand> &&);

        Opcode getOpcode();

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

    class RegAndMemSpace {

    };

    class Context {
    public:
        long long offset_of_function = 0;

        long long allocated_space_for_variables = 0;

        long long allocated_space_for_arguments = 0;

        void startFunction();

        long long getCurrentPlaceOnStack(long long);

        void addOperand(long long, std::unique_ptr<Operand> &&);

        Operand *getOperand(long long);

        void addInstruction(Instruction &&);

        void print(std::ostream &);

        std::size_t getNumberOfInstructions();

        void addFunctionCall(std::string, IntImmediate *);

        void addFunctionPlace(std::string);

        void addJumpToLabel(long long, IntImmediate *);

        void addLabelPlace(long long);

        void finishCallsAndJmps();

        std::unique_ptr<Operand> getRegister(long long);
        
    private:
        RegAndMemSpace reg_allocator;

        T86Program program;

        std::map<long long, std::unique_ptr<Operand>> instructionToOperand;

        std::map<std::string, std::vector<IntImmediate *>> notFinishedCalls;

        std::set<std::pair<std::string, size_t>> placeForCall;

        std::map<long long, std::vector<IntImmediate *>> notFinishedJumps;

        std::set<std::pair<long long, size_t>> placeForJumps;

        long long current_place_on_stack = 0;

    };

}

#endif //COMPILER_T86INST_H
