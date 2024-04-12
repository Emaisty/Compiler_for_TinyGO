#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stack>

#include "types.h"


namespace IR {

    class Value {
    public:
        Value(long long &);

        virtual ~Value() = default;

        void addUse(Value *);

        virtual void print(std::ostream &) = 0;

        long long inner_number;

    private:
        std::vector<Value *> uses;
    };

    class IRFunc;

    class IRProgram;

    struct Context {
        Context();

        bool Global = true;

        std::unique_ptr<IRProgram> program;

        long long counter = 0;

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

    private:
        IR::IRFunc *where_build;

        std::stack<Value *> cont_label;

        std::stack<Value *> break_label;

        std::vector<std::map<std::string, IRFunc *>> functions;

        std::vector<std::map<std::string, Value *>> variables;

    };

    class Const : public Value {
    public:
        using Value::Value;
    private:
    };

    class IntConst : public Const {
    public:
        using Const::Const;

        void addValue(long long);

        void print(std::ostream &) override;

    private:
        long long value = 0;
    };

    class DoubleConst : public Const {
    public:
        using Const::Const;

        void addValue(double);

        void print(std::ostream &) override;

    private:
        double value = 0;
    };

    class Nullptr : public Const {
    public:
    private:
    };

    class StructValue : public Const {
    public:
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

    private:
    };

    class IRLoad : public Instruction {
    public:
        using Instruction::Instruction;

        void addLoadFrom(Value *);

        Value *getPointer();

        void print(std::ostream &) override;

    private:
        Value *where;
    };

    class IRStore : public Instruction {
    public:
        using Instruction::Instruction;

        void addStoreWhere(Value *);

        void addStoreWhat(Value *);

        void print(std::ostream &) override;

    private:
        Value *where;

        Value *what;
    };

    class IRAlloca : public Instruction {
    public:
        using Instruction::Instruction;

        void addType(Type *);

        void print(std::ostream &) override;

    private:
        Type *type;
    };

    class IRGlobal : public Instruction {
    public:
        using Instruction::Instruction;

        void addValue(Value *);

        void addType(Type *);

        void print(std::ostream &) override;

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

    private:
        Value *result;
        Value *brT, *brNT;
    };

    class IRRet : public Instruction {
    public:
        using Instruction::Instruction;

        void addRetVal(Value *);

        void print(std::ostream &) override;

    private:
        Value *res;
    };

    class IRCall : public Instruction {
    public:
        using Instruction::Instruction;

        void addLinkToFunc(Value *);

        void addArg(Value *);

        void print(std::ostream &) override;

    private:
        Value *function;

        std::vector<Value *> arguments;

    };

    class IRCast : public Instruction {
    public:
        using Instruction::Instruction;

        void addExpr(Value *);

        void addTypeTo(Type *);

        void print(std::ostream &) override;

    private:
        Value *expr;
        Type *to;
    };

    class IRFuncArg : public Value {
    public:
        using Value::Value;

        void addType(Type *);

        void print(std::ostream &) override;

    private:
        Type *type;

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

    private:
        std::vector<std::unique_ptr<Value>> globalDecl;

        std::vector<std::unique_ptr<Value>> functions;
    };

    class IRComment : public Instruction {
    public:
        using Instruction::Instruction;

        IRComment *addText(std::string);

        void print(std::ostream &) override;

    private:
        std::string comment;
    };

}

#endif //COMPILER_IR_H
