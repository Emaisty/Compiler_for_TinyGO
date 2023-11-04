#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <string>


namespace IR {

    class IRType {

    };

    class IRTypeInt : public IRType {
    public:
    private:
    };

    class IRValue {

    };

    class IRValueVar : public IRValue {

    };

    class IRValueNumber : public IRValue {

    };

    class IRValueNone : public IRValue {

    };

    class IRLine {

    };


    class IRGlobalDecl {
    public:

    private:
        std::string name;
        std::shared_ptr<IRType> type;
        std::shared_ptr<IRValue> value;

    };

    class IRAlloca : public IRLine {
    public:
    private:
    };

    class IRLabel : public IRLine {
    public:
    private:
        std::string name;
    };

    class IRBranch : public IRLine {
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

    class IRReturn : public IRLine {
    public:
    private:
    };





}

#endif //COMPILER_IR_H
