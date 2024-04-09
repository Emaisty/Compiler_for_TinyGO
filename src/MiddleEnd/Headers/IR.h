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
        Value() = default;

        virtual ~Value() = default;

        void addUse(Value *);

        virtual void print(std::ostream &) = 0;

        long long inner_number;

    private:
        std::vector<Value *> uses;
    };

    class IRFunc;

    struct Context {
        bool Global = true;

        std::stack<Value *> cont_label;

        std::stack<Value *> break_label;

        void addFunction(std::string, IRFunc *);

        IRFunc *getFunction(std::string);

        void addVariable(std::string, Value *);

        Value *getVariable(std::string);

        void setBuilder(std::vector<std::unique_ptr<Value>> *);

    private:
        std::vector<std::unique_ptr<Value>> *where_put;

    };

    class Const : public Value {
    public:
    private:
    };

    class IntConst : public Const {
    public:
        IntConst(long long new_value = 0) : value(new_value) {}

        void print(std::ostream &) override;

    private:
        long long value = 0;
    };

    class DoubleConst : public Const {
    public:
        DoubleConst(double new_value = 0) : value(new_value) {}

        void print(std::ostream &) override;

    private:
        double value = 0;
    };

    class Instruction : public Value {
    public:
    private:
    };


    class IRArithOp : public Instruction {
    public:
        enum Operator {
            OR, AND, BINOR, BINAND, PLUS, MINUS, MUL, DIV, MOD, EQ, NE, GT, GE, LT, LE
        };

        void addChildren(Value *, Value *);

        void setType(Operator);

        void print(std::ostream &) override;

    private:

        Operator op;

        Value *left, *right;
    };


    class IRLabel : public Instruction {
    public:
        void print(std::ostream &) override;

    private:
    };

    class IRLoad : public Instruction {
    public:
        void addLoadFrom(Value *);

        void print(std::ostream &) override;

    private:
        Value *where;
    };

    class IRStore : public Instruction {
    public:
        void addStoreWhere(Value *);

        void addStoreWhat(Value *);

        void print(std::ostream &) override;

    private:
        Value *where;

        Value *what;
    };

    class IRAlloca : public Instruction {
    public:
        void addType(Type *);

        void print(std::ostream &) override;

    private:
        Type *type;
    };

    class IRGlobal : public Instruction {
    public:
        void addValue(Value *);

        void addType(Type *);

        void print(std::ostream &) override;

    private:
        Type *type;
        Value *value;
    };

    class IRBranch : public Instruction {
    public:
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
        void addRetVal(Value *);

        void print(std::ostream &) override;

    private:
        Value *res;
    };

    class IRCall : public Instruction {
    public:
        void addLinkToFunc(Value *);

        void addArg(Value *);

        void print(std::ostream &) override;

    private:
        Value *function;

        std::vector<Value *> arguments;

    };

    class IRCast : public Instruction {
    public:
        void addExpr(Value *);

        void addTypeTo(Type *);

        void print(std::ostream &) override;

    private:
        Value *expr;
        Type *to;
    };

    class IRFuncArg : public Value {
    public:
        void addType(Type *);

        void print(std::ostream &) override;

    private:
        Type *type;

    };

    class IRFunc : public Value {
    public:
        void addReturnType(Type *);

        void addArg(std::unique_ptr<IRFuncArg> &&);

        void addInstToBody(std::unique_ptr<Value> &&);

        void setName(std::string);

        std::string getName();

        void print(std::ostream &) override;

    private:
        Type *return_type;

        std::string name;

        std::vector<std::unique_ptr<IRFuncArg>> arguments;

        std::vector<std::unique_ptr<Value>> body;

    };

    class IRProgram : public Value {
    public:
        void addGlobDecl(std::unique_ptr<Value> &&);

        void addFunc(std::unique_ptr<Value> &&);

        void print(std::ostream &) override;

    private:
        std::vector<std::unique_ptr<Value>> globalDecl;

        std::vector<std::unique_ptr<Value>> functions;
    };

}

#endif //COMPILER_IR_H
