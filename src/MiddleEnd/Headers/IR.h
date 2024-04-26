#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stack>

#include "types.h"
#include "T86Inst.h"

namespace IR {

    class Value {
    public:
        Value(long long &);

        virtual ~Value() = default;

        void addUse(Value *);

        virtual void print(std::ostream &) = 0;

        virtual void generateT86(T86::Context &) = 0;

        virtual std::unique_ptr<T86::Operand> getOperand(T86::Context &);

        long long inner_number;

    private:
        std::vector<Value *> uses;
    };

    class IRFunc;

    class IRProgram;

    class Const : public Value {
    public:
        using Value::Value;

        virtual std::string toString() = 0;

    private:
    };

    struct Context {
        Context();

        bool Global = true;

        std::unique_ptr<IRProgram> program;

        long long counter = 0;

        T86::Context createT86Context();

        void goDeeper();

        void goUp();

        void addContinueLabel(Value *);

        Value *getContinueLabel();

        void addBreakLabel(Value *);

        Value *getBreakLabel();

        void addFunction(std::string, IRFunc *);

        IRFunc *getFunction(std::string);

        void addVariable(std::string, Value *);

        Value *getVariable(std::string);

        void setFunction(IRFunc *);

        Value *buildInstruction(std::unique_ptr<Value> &&);

        void deleteLastRow();

        std::unique_ptr<Const> getBasicValue(Type *);

    private:
        IR::IRFunc *where_build;

        std::stack<Value *> cont_label;

        std::stack<Value *> break_label;

        std::vector<std::map<std::string, IRFunc *>> functions;

        std::vector<std::map<std::string, Value *>> variables;

    };


    class IntConst : public Const {
    public:
        using Const::Const;

        void addValue(long long);

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

        void print(std::ostream &) override;

        std::string toString() override;

    private:
        long long value = 0;
    };

    class DoubleConst : public Const {
    public:
        using Const::Const;

        void addValue(double);

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

        void print(std::ostream &) override;

        std::string toString() override;

    private:
        double value = 0;
    };

    class Nullptr : public Const {
    public:
        using Const::Const;

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

        void print(std::ostream &) override;

        std::string toString() override;

    private:
    };

    class StructValue : public Const {
    public:
        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

        void print(std::ostream &) override;

    private:
    };

    class Instruction : public Value {
    public:
        using Value::Value;
    private:
    };


    class IRArithOp : public Instruction {
    public:
        using Instruction::Instruction;

        enum Operator {
            OR, AND, BINOR, BINAND, PLUS, MINUS, MUL, DIV, MOD, EQ, NE, GT, GE, LT, LE, XOR
        };

        inline static const std::map<Operator, std::string> operator_to_str = {
                {Operator::OR,     "OR"},
                {Operator::AND,    "AND"},
                {Operator::BINOR,  "BIN OR"},
                {Operator::BINAND, "BIN AND"},
                {Operator::PLUS,   "ADDITION"},
                {Operator::MINUS,  "SUBTRACT."},
                {Operator::MUL,    "MULTIPLICATION"},
                {Operator::DIV,    "DIVISION"},
                {Operator::MOD,    "MODULUS"},
                {Operator::EQ,     "CMPR for equality"},
                {Operator::NE,     "CMPR for not equality"},
                {Operator::GT,     "CMPR for greater than"},
                {Operator::GE,     "CMPR for greater or equal"},
                {Operator::LT,     "CMPR for lower than"},
                {Operator::LE,     "CMPR for lower or equal"},
                {Operator::XOR,    "XOR"},
        };

        void addChildren(Value *, Value *);

        void setTypeOfOperation(Operator);

        void setTypeOfResult(Type *);

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

        void print(std::ostream &) override;

    private:

        Operator op;

        Value *left, *right;

        Type *result_type;
    };


    class IRLabel : public Instruction {
    public:
        using Instruction::Instruction;

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

    private:
    };

    class IRLoad : public Instruction {
    public:
        using Instruction::Instruction;

        void addLoadFrom(Value *);

        Value *getPointer();

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

    private:
        Value *where;
    };

    class IRStore : public Instruction {
    public:
        using Instruction::Instruction;

        void addStoreWhere(Value *);

        void addStoreWhat(Value *);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

    private:
        Value *where;

        Value *what;
    };

    class IRAlloca : public Instruction {
    public:
        using Instruction::Instruction;

        void addType(Type *);

        void addBasicValue(std::unique_ptr<Const> &&);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

    private:
        Type *type;

        std::unique_ptr<Const> basicValue;

        long long place_on_stack = -1;

    };

    class IRGlobal : public Instruction {
    public:
        using Instruction::Instruction;

        void addValue(Value *);

        void addType(Type *);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

    private:
        Type *type;
        Value *value;
    };

    class IRBranch : public Instruction {
    public:
        using Instruction::Instruction;

        void addCond(Value *);

        void addBrTaken(Value *);

        void addBrNTaken(Value *);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

//        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

    private:
        Value *result;
        Value *brT, *brNT;
    };

    class IRRet : public Instruction {
    public:
        using Instruction::Instruction;

        void addRetVal(Value *);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

//        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

    private:
        Value *res;
    };

    class IRCall : public Instruction {
    public:
        using Instruction::Instruction;

        void addLinkToFunc(IRFunc *);

        void addFunctionName(std::string);

        void addArg(Value *);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

    private:
        IRFunc *function;

        std::string name_of_function;

        std::vector<Value *> arguments;

    };

    class IRCast : public Instruction {
    public:
        using Instruction::Instruction;

        void addExpr(Value *);

        void addTypeToFrom(Type *, Type *);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

    private:
        Value *expr;
        Type *to, *from;
    };

    class IRFuncArg : public Value {
    public:
        using Value::Value;

        void addType(Type *);

        void addOrder(int = 0);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

    private:
        Type *type;

        int order_of_arg = 0;

    };

    class IRFunc : public Value {
    public:
        using Value::Value;

        void addReturnType(Type *);

        void addArg(std::unique_ptr<IRFuncArg> &&);

        void addAlloca(std::unique_ptr<Value> &&);

        void addInstToBody(std::unique_ptr<Value> &&);

        void setName(std::string);

        std::string getName();

        std::vector<std::unique_ptr<Value>> *getLinkToBody();

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

    private:
        Type *return_type;

        std::string name;

        std::vector<std::unique_ptr<IRFuncArg>> arguments;

        std::vector<std::unique_ptr<Value>> allocas;

        std::vector<std::unique_ptr<Value>> body;

    };

    class IRProgram : public Value {
    public:
        using Value::Value;

        void addGlobDecl(std::unique_ptr<Value> &&);

        void addFunc(std::unique_ptr<Value> &&);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

    private:
        std::vector<std::unique_ptr<Value>> globalDecl;

        std::vector<std::unique_ptr<Value>> functions;
    };

    class IRComment : public Instruction {
    public:
        using Instruction::Instruction;

        IRComment *addText(std::string);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

    private:
        std::string comment;
    };

}

#endif //COMPILER_IR_H
