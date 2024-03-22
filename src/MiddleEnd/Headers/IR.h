#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <iostream>
#include <vector>
#include <string>

#include "types.h"


namespace IR {

    class IRLine {
    public:

        void addUse(IRLine *);

    private:
        std::vector<IRLine *> uses;
    };


    struct Context {
        bool Global = true;

        IRLine *cont_label;

        IRLine *break_label;



    };

    class IRArithOp : public IRLine {
    public:
        void addChildren(std::unique_ptr<IR::IRLine> &&, std::unique_ptr<IR::IRLine> &&);

    private:
        std::unique_ptr<IRLine> left, right;
    };

    class IRIntValue : public IRLine {
    public:
    private:
        long long value;
    };

    class IRDoubleValue : public IRLine {
    public:
    private:
        double value;
    };

    class IRNullValue : public IRLine {
    public:
    private:
    };

    class IRLabel : public IRLine {
    public:
    private:
        std::string name;
    };

    class IRLoad : public IRLine {
    public:
    private:
    };

    class IRStore : public IRLine {
    public:

        void addValue(std::unique_ptr<IRLine> &&);

        void setLink(IRLine *);

    private:

        std::unique_ptr<IRLine> value;

        IRLine *var_to_store;

    };

    class IRAlloca : public IRLine {
    public:

        void addType(Type *);

    private:
        Type *type;
    };

    class IRGlobal : public IRLine {
    public:
        void addValue(std::unique_ptr<IRLine> &&);

        void addType(Type *);

    private:
        Type *type;
        std::unique_ptr<IRLine> value;
    };

    class IRBlock : public IRLine {
    public:
        void addLine(std::unique_ptr<IRLine> &&);

    private:
        std::vector<std::unique_ptr<IRLine>> block;
    };

    class IRCMP : public IRLine {
    public:
    private:

    };

    class IRBranch : public IRLine {
    public:

        void addCond(std::unique_ptr<IRLine> &&);

        void addBrTaken(IRLine *);

        void addBrNTaken(IRLine *);

    private:
        std::unique_ptr<IRLine> result;
        IRLine *brT, *brNT;
    };

    class IRRet : public IRLine {
    public:
        void addRetVal(std::unique_ptr<IRLine> &&);

    private:
        std::unique_ptr<IRLine> res;
    };

    class IRFunc : public IRLine {
    public:

        void addReturnType(Type *);

        void addTypeOfArg(Type *);

        void addBody(std::unique_ptr<IRLine> &&);

    private:

        Type *return_type;

        std::vector<Type *> type_of_args;
        std::unique_ptr<IRLine> body;
    };

    class IRProgram : public IRLine {
    public:

        void addLine(std::unique_ptr<IRLine> &&);

    private:
        std::vector<std::unique_ptr<IRLine>> IRLines;
    };

}

#endif //COMPILER_IR_H
