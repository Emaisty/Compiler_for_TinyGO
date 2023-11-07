#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <iostream>
#include <vector>
#include <string>


namespace IR {


    class IRValue {
    public:

        friend std::ostream &operator<<(std::ostream &os, const IRValue &);

    private:

        virtual void print(std::ostream &) const = 0;
    };

    class IRValueVar : public IRValue {
    public:
    private:

        void print(std::ostream &) const override;

        std::string name;
    };

    class IRValueIntNumber : public IRValue {
    public:
        IRValueIntNumber(int num = 0) : number(num) {}

    private:

        void print(std::ostream &) const override;

        int number;

    };

    class IRValueFloatNumber : public IRValue {
    public:
        IRValueFloatNumber(double num = 0) : number(num) {}

    private:

        void print(std::ostream &) const override;

        double number;
    };

    class IRValueNullPrt : public IRValue {
    public:
    private:

        void print(std::ostream &) const override;

    };

    class IRValueNone : public IRValue {
    public:
    private:

        void print(std::ostream &) const override;

    };

    class IRType {
    public:

        virtual std::shared_ptr<IRValue> getDefaultValue() = 0;

        friend std::ostream &operator<<(std::ostream &os, const IRType &);

    protected:

        virtual void print(std::ostream &) const = 0;
    };

    class IRTypeInt : public IRType {
    public:

        IRTypeInt(int new_bits = 32) : bits(new_bits) {}

        std::shared_ptr<IRValue> getDefaultValue() override;

    private:

        void print(std::ostream &) const override;

        int bits;
    };

    class IRTypeFloat : public IRType {
    public:

        std::shared_ptr<IRValue> getDefaultValue() override;

    private:

        void print(std::ostream &) const override;

    };

    class IRTypeStruct : public IRType {
    public:

        void addFiled(std::shared_ptr<IRType> &&);

        std::shared_ptr<IRValue> getDefaultValue() override;

    private:

        void print(std::ostream &) const override;

        std::vector<std::shared_ptr<IRType>> fields;
    };

    class IRTypeNull : public IRType {
    public:

        std::shared_ptr<IRValue> getDefaultValue() override;

    private:

        void print(std::ostream &) const override;

    };

    class IRTypePointer : public IRType {
    public:

        IRTypePointer(std::shared_ptr<IRType> &&);

        std::shared_ptr<IRValue> getDefaultValue() override;

    private:

        void print(std::ostream &) const override;

        std::shared_ptr<IRType> type;
    };


    class IRLine {
    public:
    protected:
    };


    class IRGlobalDecl {
    public:

        void addName(std::string);

        void addType(std::shared_ptr<IR::IRType> &&);

        friend std::ostream &operator<<(std::ostream &os, const IRGlobalDecl &);

        virtual void print(std::ostream &) const = 0;

    protected:
        std::string name;
        std::shared_ptr<IRType> type;
    };

    class IRGlobalVarDecl : public IRGlobalDecl {
    public:
        void print(std::ostream &) const override;

    private:
    };

    class IRGlobalTypeDecl : public IRGlobalDecl {
    public:
        void print(std::ostream &) const override;

    private:

    };

    class IRNewType {
    public:
    private:
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

    class IRFunc {
    public:
    private:
    };


    class IRProgram {
    public:

        void addDecl(std::shared_ptr<IRGlobalDecl> &&);

        void addFunc(std::shared_ptr<IRFunc> &&);

        friend std::ostream &operator<<(std::ostream &os, const IRProgram &);

    private:
        std::vector<std::shared_ptr<IRGlobalDecl>> declarations;
        std::vector<std::shared_ptr<IRFunc>> functions;
    };


}

#endif //COMPILER_IR_H
