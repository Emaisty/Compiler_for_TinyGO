#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
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

        unsigned long long inner_number;

    private:
        std::vector<Value *> uses;
    };

    class IRFunc;

    class IRProgram;

    class Const : public Value {
    public:
        using Value::Value;

        virtual std::string toString() = 0;

        virtual void fillWithValue(unsigned long long) = 0;

    private:
    };

    struct Context {
        Context();

        bool Global = true;

        bool l_value = false;

        bool inside_dispatch = false;

        std::unique_ptr<IRProgram> program;

        long long counter = 0;

        std::string name_if_return_become_arg;

        std::string name_of_dispatched_struct;

        StructType* type_of_return_arg;

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

        void addModifiedVar(std::string);

        bool wasVarModified(std::string);

        void clearModifiedVars();

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

        std::set<std::string> modifiedVars;

    };


    class IntConst : public Const {
    public:
        using Const::Const;

        void addValue(long long);

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

        void print(std::ostream &) override;

        std::string toString() override;

        void fillWithValue(unsigned long long) override{};

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

        void fillWithValue(unsigned long long) override{};

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

        void fillWithValue(unsigned long long) override{};

    private:
    };

    class StructConst : public Const {
    public:
        using Const::Const;

        void addValue(Value *);

        void addConst(std::unique_ptr<Const> &&);

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

        void print(std::ostream &) override;

        std::string toString() override;

        void fillWithValue(unsigned long long) override{};

    private:
        //it may be const, or might be a value
        std::vector<std::pair<std::unique_ptr<Const>, Value *>> basic_values;

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

        Type *getType();

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

    class IRMembCall : public Instruction {
    public:
        using Instruction::Instruction;

        void addCallWhere(Value *);

        void addTypeWhere(StructType*);

        void addCallWhat(int);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

    private:
        Value *where;

        int what;

        StructType* typeOfWhere;

    };

    class IRElemCall : public Instruction {
    public:
        using Instruction::Instruction;

        void addCallWhere(Value *);

        void addTypeWhere(Type*);

        void addCallWhat(int);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

    private:
        Value *where;

        int what;

        Type* typeOfElem;

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

    class IRMemCopy : public Instruction {
    public:
        using Instruction::Instruction;

        void addCopyFrom(Value*);

        void addCopyTo(Value*);

        void addSize(long long);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

    private:
        Value *from, *to;

        long long size;

    };

    class IRScan : public Instruction {
    public:
        using Instruction::Instruction;

        void addLink(Value*);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

    private:
        Value* link;

    };

    class IRPrint : public Instruction {
    public:
        using Instruction::Instruction;

        void addValue(Value*);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

    private:
        Value* value;

    };

    class IRFuncArg : public Value {
    public:
        using Value::Value;

        void addType(Type *);

        void addOrder(long long = 0);

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

        std::unique_ptr<T86::Operand> getOperand(T86::Context &) override;

        long long size();

    private:
        Type *type;

        long long order_of_arg = 0;

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

        void setSpaceForAlloca(long long);

        std::vector<std::unique_ptr<Value>> *getLinkToBody();

        void print(std::ostream &) override;

        void generateT86(T86::Context &) override;

    private:
        Type *return_type = nullptr;

        std::string name;

        std::vector<std::unique_ptr<IRFuncArg>> arguments;

        std::vector<std::unique_ptr<Value>> allocas;

        std::vector<std::unique_ptr<Value>> body;

        long long space_for_alloca = 0;

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
