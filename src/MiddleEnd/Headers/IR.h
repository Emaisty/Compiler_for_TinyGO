#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <iostream>
#include <vector>
#include <string>
#include "IRTypes.h"


namespace IR {

    class IRLine {
    public:

        void addUse(IRLine *);

    private:
        std::vector<IRLine *> uses;
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
    private:

    };

    class IRAlloca : public IRLine {
    public:
    private:
        std::unique_ptr<IRType> type;
    };

    class IRGlobal : public IRLine {
    public:
        void addValue(std::unique_ptr<IRLine>)

    private:
        std::unique_ptr<IRType> type;
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
    private:
        std::unique_ptr<IRLine> result;
        IRLine *brT, brNT;
    };

    class IRFunc : public IRLine {
    public:

        void addDecl(std::unique_ptr<IRLine> &&);

        void addBody(std::unique_ptr<IRLine> &&);

    private:
        std::unique_ptr<>;

        std::vector<std::unique_ptr<IRLine>> decls;
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
