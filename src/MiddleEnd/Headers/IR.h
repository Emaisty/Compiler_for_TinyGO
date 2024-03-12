#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <iostream>
#include <vector>
#include <string>
#include "IRTypes.h"


namespace IR {

    class IRLine {
    public:
    private:
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

    class IRLabel : public IRLine {
    public:
    private:

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
        IRType *type;
    };

    class IRGlobal : public IRLine {
    public:
    private:

        IRType *type;
        IRLine *value;
    };

    class IRFunc : public IRLine {
    public:

    private:

    };

    class IRProgram {
    public:

        void addLine(std::unique_ptr<IRLine> &&);

    private:
        std::vector<std::unique_ptr<IRLine>> IRLines;
    };

}

#endif //COMPILER_IR_H
