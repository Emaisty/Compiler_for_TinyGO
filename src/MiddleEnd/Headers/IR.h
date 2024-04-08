#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "types.h"


namespace IR {

    class Value {
    public:
        Value() = default;

        virtual ~Value() = default;

        void addUse(Value *);

        virtual void print(std::ostream &, PrintHelper &) = 0;

    private:
        std::vector<Value *> uses;
    };


    struct Context {
        bool Global = true;

        Value *cont_label;

        Value *break_label;

        void addFunction();

        void addVariable();

    private:



    };

    class Const : public Value {
    public:
    private:
    };

    class IntConst : public Const {
    public:
    private:
    };

    class DoubleConst : public Const {
    public:
    private:
    };

    class Instruction : public Value {
    public:
    private:
    };


    class IRArithOp : public Instruction {
    public:
        void addChildren(std::unique_ptr<IR::IRLine> &&, std::unique_ptr<IR::IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

        enum Operator {
            OR, AND, BINOR, BINAND, PLUS, MINUS, MUL, DIV, MOD, EQ, NE, GT, GE, LT, LE
        };

        void setType(Operator);

    private:

        Operator op;

        Value *left, *right;
    };


    class IRLabel : public Instruction {
    public:
        void print(std::ostream &, PrintHelper &) override;

    private:
    };

    class IRLoad : public Instruction {
    public:
        void addLink(IRLine *);

        void print(std::ostream &, PrintHelper &) override;

    private:
        IRLine *from_whom_load;
    };

    class IRStore : public Instruction {
    public:
        void addValue(std::unique_ptr<IRLine> &&);

        void setLink(IRLine *);

        void print(std::ostream &, PrintHelper &) override;

    private:
        std::unique_ptr<IRLine> value;

        IRLine *var_to_store;
    };

    class IRAlloca : public Instruction {
    public:
        void addType(Type *);

        void print(std::ostream &, PrintHelper &) override;

    private:
        Type *type;
    };

    class IRGlobal : public Instruction {
    public:
        void addValue(std::unique_ptr<IRLine> &&);

        void addType(Type *);

        void print(std::ostream &, PrintHelper &) override;

    private:
        Type *type;
        std::unique_ptr<IRLine> value;
    };

    class IRBlock : public Instruction {
    public:
        void addLine(std::unique_ptr<IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

    private:
        std::vector<std::unique_ptr<IRLine>> block;
    };

    class IRCMP : public Instruction {
    public:
        void print(std::ostream &, PrintHelper &) override;

    private:

    };

    class IRBranch : public Instruction {
    public:
        void addCond(std::unique_ptr<IRLine> &&);

        void addBrTaken(IRLine *);

        void addBrNTaken(IRLine *);

        void print(std::ostream &, PrintHelper &) override;

    private:
        IRLine *result;
        IRLine *brT, *brNT;
    };

    class IRRet : public Instruction {
    public:
        void addRetVal(std::unique_ptr<IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

    private:
        std::unique_ptr<IRLine> res;
    };

    class IRCall : public Instruction {
    public:
        void addRetVal(std::unique_ptr<IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

    private:
        IRLine *function;


    };

    class IRCast : public Instruction {
    public:
        void addExpr(IRLine *);

        void addTypeTo(Type *);

        void print(std::ostream &, PrintHelper &) override;

    private:
        IRLine *expr;
        Type *to;
    };

    class IRFuncArg : public Value {
    public:

    private:
        Type *type;

    };

    class IRFunc : public Value {
    public:
        void addReturnType(Type *);

        void addTypeOfArg(Type *, std::string);

        void addBody(std::unique_ptr<IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

    private:

        Type *return_type;

        std::vector<std::unique_ptr<IRFuncArg>> arguments;

        std::unique_ptr<IRLine> body;
    };

    class IRProgram : public Value {
    public:
        void addLine(std::unique_ptr<IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

    private:
        std::vector<std::unique_ptr<IRGlobal>> globalDecl;

        std::vector<std::unique_ptr<IRFunc>> functions;
    };

}

#endif //COMPILER_IR_H
