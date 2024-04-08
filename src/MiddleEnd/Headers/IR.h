#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "types.h"


namespace IR {

    class PrintHelper {

    };

    class IRLine {
    public:
        IRLine() = default;

        virtual ~IRLine() = default;

        void addUse(IRLine *);

        virtual void print(std::ostream &, PrintHelper &) = 0;

    private:
        std::vector<IRLine *> uses;
    };


    struct Context {
        bool Global = true;

        IRLine *cont_label;

        IRLine *break_label;

        void setNewVar(std::string, IRLine *);

        IRLine *getVar(std::string);

    private:

        std::map<std::string, IRLine *> variables;

    };

    class IRArithOp : public IRLine {
    public:
        void addChildren(std::unique_ptr<IR::IRLine> &&, std::unique_ptr<IR::IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

        enum Operator {
            OR, AND, BINOR, BINAND, PLUS, MINUS, MUL, DIV, MOD, EQ, NE, GT, GE, LT, LE
        };

        void setType(Operator);

    private:

        Operator op;

        std::unique_ptr<IRLine> left, right;
    };

    class IRIntValue : public IRLine {
    public:
        IRIntValue(long long = 0);

        void print(std::ostream &, PrintHelper &) override;

    private:
        long long value;
    };

    class IRDoubleValue : public IRLine {
    public:
        IRDoubleValue(double = 0);

        void print(std::ostream &, PrintHelper &) override;

    private:
        double value;
    };

    class IRNullValue : public IRLine {
    public:
        void print(std::ostream &, PrintHelper &) override;

    private:
    };

    class IRLabel : public IRLine {
    public:
        void print(std::ostream &, PrintHelper &) override;

    private:
        std::string name;
    };

    class IRLoad : public IRLine {
    public:
        void addLink(IRLine *);

        void print(std::ostream &, PrintHelper &) override;

    private:
        IRLine *from_whom_load;
    };

    class IRStore : public IRLine {
    public:
        void addValue(std::unique_ptr<IRLine> &&);

        void setLink(IRLine *);

        void print(std::ostream &, PrintHelper &) override;

    private:
        std::unique_ptr<IRLine> value;

        IRLine *var_to_store;
    };

    class IRAlloca : public IRLine {
    public:
        void addType(Type *);

        void print(std::ostream &, PrintHelper &) override;

    private:
        Type *type;
    };

    class IRGlobal : public IRLine {
    public:
        void addValue(std::unique_ptr<IRLine> &&);

        void addType(Type *);

        void print(std::ostream &, PrintHelper &) override;

    private:
        Type *type;
        std::unique_ptr<IRLine> value;
    };

    class IRBlock : public IRLine {
    public:
        void addLine(std::unique_ptr<IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

    private:
        std::vector<std::unique_ptr<IRLine>> block;
    };

    class IRCMP : public IRLine {
    public:
        void print(std::ostream &, PrintHelper &) override;

    private:

    };

    class IRBranch : public IRLine {
    public:
        void addCond(std::unique_ptr<IRLine> &&);

        void addBrTaken(IRLine *);

        void addBrNTaken(IRLine *);

        void print(std::ostream &, PrintHelper &) override;

    private:
        std::unique_ptr<IRLine> result;
        IRLine *brT, *brNT;
    };

    class IRRet : public IRLine {
    public:
        void addRetVal(std::unique_ptr<IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

    private:
        std::unique_ptr<IRLine> res;
    };

    class IRCall : public IRLine {
    public:
        void addRetVal(std::unique_ptr<IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

    private:
        std::unique_ptr<IRLine>;
    };

    class IRFunc : public IRLine {
    public:
        void addReturnType(Type *);

        void addTypeOfArg(Type *, std::string);

        void addBody(std::unique_ptr<IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

    private:

        Type *return_type;

        std::vector<std::pair<Type *, std::string>> type_of_args;
        std::unique_ptr<IRLine> body;
    };

    class IRProgram : public IRLine {
    public:
        void addLine(std::unique_ptr<IRLine> &&);

        void print(std::ostream &, PrintHelper &) override;

    private:
        std::vector<std::unique_ptr<IRLine>> IRLines;
    };

}

#endif //COMPILER_IR_H
