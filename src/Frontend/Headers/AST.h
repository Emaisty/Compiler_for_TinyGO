#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <memory>

#include "IR.h"


namespace AST {

    class ASTType {
    public:

        ASTType() = default;

        virtual ~ASTType() = default;

        virtual std::unique_ptr<ASTType> clone() const = 0;

        virtual bool operator==(const std::unique_ptr<ASTType> &&) const = 0;

        ASTType &set_addressability(bool flag);

        ASTType &set_constability(bool flag);

        bool addressable();

        bool constable();

        virtual std::shared_ptr<IR::IRType> generateIR() = 0;

    protected:
        bool is_variable = false;

        bool is_const = true;
    };


    struct ItemInSpaceName {

        bool is_func;
        std::string name;

        int level;

        ASTType *type;

        std::vector<std::unique_ptr<ASTType>> params;

        ItemInSpaceName(std::string new_name, std::unique_ptr<ASTType> &new_type, int new_level,
                        bool func)
                : name(new_name),
                  type(new_type.get()),
                  level(new_level),
                  is_func(func) {}

        void addParam(std::unique_ptr<ASTType> &new_type) {
            params.emplace_back(new_type.get());
        }
    };

    struct Context {

        std::vector<ItemInSpaceName> name_space;

        bool in_loop = false;
        int level = 0;
        std::unique_ptr<ASTType> return_type = nullptr;

        std::map<std::string, std::set<std::string>> uses_of_other_var;

        void goDeeper();

        void goUp();

        void checkIfNameInSpace(std::string name);

    private:

        std::stack<bool> pr_loop_status;
    };

    class ASTTypeNull : public ASTType {
    public:
        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

        bool operator==(const std::unique_ptr<ASTType> &&type) const override;

        std::shared_ptr<IR::IRType> generateIR() override;
    };

    class ASTTypeInt : public ASTType {
    public:

        ASTTypeInt(int new_bits = 32);

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

        bool operator==(const std::unique_ptr<ASTType> &&type) const override;

        std::shared_ptr<IR::IRType> generateIR() override;

    private:
        int bits;
    };

    class ASTTypeFloat : public ASTType {
    public:

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

        bool operator==(const std::unique_ptr<ASTType> &&type) const override;

        std::shared_ptr<IR::IRType> generateIR() override;

    private:
    };

    class ASTTypeBool : public ASTType {
    public:

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

        bool operator==(const std::unique_ptr<ASTType> &&type) const override;

        std::shared_ptr<IR::IRType> generateIR() override;

    private:
    };

    class ASTTypePointer : public ASTType {
    public:

        ASTTypePointer() = default;

        ASTTypePointer(std::unique_ptr<ASTType> &&new_type);

        ASTTypePointer(const ASTTypePointer &old_pointer);

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

        bool operator==(const std::unique_ptr<ASTType> &&type) const override;

        ASTType *getValue();

        std::shared_ptr<IR::IRType> generateIR() override;

    private:
        std::unique_ptr<ASTType> type;
    };

    class ASTTypeStruct : public ASTType {
    public:

        ASTTypeStruct() = default;

        ASTTypeStruct(const ASTTypeStruct &old_struct);

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

        void addField(std::string name, std::unique_ptr<ASTType> &&type);

        bool operator==(const std::unique_ptr<ASTType> &&type) const override;

        ASTType *findField(std::string name);

        std::shared_ptr<IR::IRType> generateIR() override;

    private:
        std::vector<std::pair<std::string, std::unique_ptr<ASTType >>> fileds;
    };

    class ASTTypeNamed : public ASTType {

        ASTTypeNamed() = default;

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

        bool operator==(const std::unique_ptr<ASTType> &&type) const override;

        std::shared_ptr<IR::IRType> generateIR() override;

    private:
        std::string name;
    };

    class ASTTypeFunction : public ASTType {

    private:
    };

    class Statement {
    public:

        Statement() = default;

        virtual ~Statement() = default;

        virtual void checker(Context &) = 0;

        void addLineNumber(int);

    private:
        int line_number;
    };

    class ASTExpression : public Statement {
    public:

        virtual std::unique_ptr<ASTType> getType(Context &) const = 0;

    private:
    };

    class ASTNullExpr : public ASTExpression {
    public:

        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;
    };

    class ASTBinaryOperator : public ASTExpression {
    public:

        enum Operator {
            OR, AND, BINOR, BINAND, PLUS, MINUS, MUL, DIV, MOD, EQ, NE, GT, GE, LT, LE
        };

        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;

        ASTBinaryOperator(std::unique_ptr<ASTExpression> &&, std::unique_ptr<ASTExpression> &&,
                          Operator new_op = PLUS);

    private:
        std::unique_ptr<ASTExpression> left, right;
        Operator op;
    };

    class ASTUnaryOperator : public ASTExpression {
    public:
        enum Operator {
            NOT, PLUS, MINUS, PREINC, PREDEC, POSTINC, POSTDEC
        };


        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;

        ASTUnaryOperator(std::unique_ptr<ASTExpression> &&, Operator new_op = PLUS);

    private:
        Operator op;
        std::unique_ptr<ASTExpression> value;
    };

    class ASTFunctionCall : public ASTExpression {
    public:

        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;

        ASTFunctionCall(std::unique_ptr<ASTExpression> &&, std::vector<std::unique_ptr<ASTExpression>> &);

    private:
        std::unique_ptr<ASTExpression> name;
        std::vector<std::unique_ptr<ASTExpression>> arg;
    };

    class ASTMemberAccess : public ASTExpression {
    public:

        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;

        ASTMemberAccess(std::unique_ptr<ASTExpression> &&, std::string &);

    private:
        std::unique_ptr<ASTExpression> name;

        std::string member;
    };

    class ASTGetPointer : public ASTExpression {
    public:

        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;

        ASTGetPointer(std::unique_ptr<ASTExpression> &&);

    private:
        std::unique_ptr<ASTExpression> var;
    };

    class ASTGetValue : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;

        ASTGetValue(std::unique_ptr<ASTExpression> &&);

    private:
        std::unique_ptr<ASTExpression> var;
    };


    class ASTIntNumber : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;

        ASTIntNumber(const int new_value = 0);

    private:
        long long int value = 0;
    };

    class ASTFloatNumber : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;

        ASTFloatNumber(double new_value = 0);

    private:
        double value = 0;
    };

    class ASTBoolNumber : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;

        ASTBoolNumber(const bool new_value = false);

    private:
        bool value = false;
    };

    class ASTStruct : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;

        ASTStruct(std::unique_ptr<ASTTypeStruct> &&,
                  std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> &);

    private:
        std::unique_ptr<ASTType> type;
        std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> values;
    };

    class ASTVar : public ASTExpression {
    public:

        [[nodiscard]] std::unique_ptr<ASTType> getType(Context &) const override;

        void checker(Context &) override;

        ASTVar(const std::string new_name);

        std::string getName();

    private:
        std::string name;
    };


    class ASTDeclaration : public Statement {
    public:

        ASTDeclaration(std::vector<std::string> &&new_name, std::vector<std::unique_ptr<ASTExpression>> &&,
                       std::unique_ptr<ASTType> &&);

        ASTDeclaration(std::vector<std::string> &&new_name, std::vector<std::unique_ptr<ASTExpression>> &&);

        ASTDeclaration(std::vector<std::string> &&new_name, std::unique_ptr<ASTType> &&);

        virtual void checkerGlobal(Context &) = 0;

        virtual std::shared_ptr<IR::IRGlobalDecl> generateIRGlobal() const = 0;


    protected:
        std::vector<std::string> name;
        std::vector<std::unique_ptr<ASTExpression>> value;
        std::unique_ptr<ASTType> type;


    };


    class ASTTypeDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        void checkerGlobal(Context &) override;

        void checker(Context &) override;

        std::shared_ptr<IR::IRGlobalDecl> generateIRGlobal() const override;

    private:
    };

    class ASTVarDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        void checkerGlobal(Context &) override;

        void checker(Context &) override;

        std::shared_ptr<IR::IRGlobalDecl> generateIRGlobal() const override;

    private:


    };

    class ASTConstDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        void checkerGlobal(Context &) override;

        void checker(Context &) override;

        std::shared_ptr<IR::IRGlobalDecl> generateIRGlobal() const override;

    private:
    };

    class ASTBlock : public Statement {
    public:

        ASTBlock() = default;

        void addStatement(std::unique_ptr<AST::Statement> &&);

        void checker(Context &) override;

    private:
        std::vector<std::unique_ptr<AST::Statement>> statements;
    };

    class ASTBreak : public Statement {
    public:

        ASTBreak() = default;

        void checker(Context &) override;

    private:

    };

    class ASTContinue : public Statement {
    public:

        ASTContinue() = default;

        void checker(Context &) override;

    private:

    };

    class ASTReturn : public Statement {
    public:

        ASTReturn() = default;

        void addReturnValue(std::unique_ptr<ASTExpression> &&);

        void checker(Context &) override;

    private:
        std::vector<std::unique_ptr<AST::ASTExpression>> return_value;
    };

    class ASTSwitch : public Statement {
    public:

        ASTSwitch() = default;

        void addExpr(std::unique_ptr<ASTExpression> &&);

        void addCase(std::unique_ptr<ASTExpression> &&, std::unique_ptr<AST::ASTBlock> &&);

        void checker(Context &) override;

    private:
        std::unique_ptr<ASTExpression> expr;

        std::vector<std::pair<std::unique_ptr<AST::ASTExpression>, std::unique_ptr<AST::Statement>>> cases;
    };


    class ASTIf : public Statement {
    public:

        ASTIf() = default;

        void addExpr(std::unique_ptr<AST::ASTExpression> &&);

        void addIfClause(std::unique_ptr<AST::ASTBlock> &&);

        void addElseClause(std::unique_ptr<AST::ASTBlock> &&);

        void checker(Context &) override;

    private:
        std::unique_ptr<AST::ASTExpression> expr;

        std::unique_ptr<AST::Statement> if_clause, else_clause;
    };

    class ASTFor : public Statement {
    public:

        ASTFor() = default;

        void addInitClause(std::vector<std::unique_ptr<AST::Statement>> &);

        void addIterClause(std::vector<std::unique_ptr<AST::Statement>> &);

        void addCondClause(std::unique_ptr<AST::ASTExpression> &&);

        void addBody(std::unique_ptr<AST::ASTBlock> &&);

        void checker(Context &) override;

    private:
        std::vector<std::unique_ptr<AST::Statement>> init_clause, iterate_clause;

        std::unique_ptr<AST::ASTExpression> if_clause;

        std::unique_ptr<AST::Statement> body;
    };

    class ASTAssign : public Statement {
    public:
        enum Type {
            ASSIGN, PLUSASSIGN, MINUSASSIGN, MULTASSIGN, DIVASSIGN, MODASSIGN
        };

        ASTAssign() = default;

        ASTAssign(std::unique_ptr<AST::ASTExpression> &&, std::unique_ptr<AST::ASTExpression> &&,
                  Type new_type);

        void checker(Context &) override;

    private:

        std::unique_ptr<AST::ASTExpression> variable, value;

        Type type;

    };


    class Function {
    public:


        void setName(std::string new_name);

        void setParams(std::vector<std::pair<std::string, std::unique_ptr<AST::ASTType>>> new_params);

        void addParam(std::string new_name, std::unique_ptr<AST::ASTType> &&);

        void addReturn(std::unique_ptr<AST::ASTType> &&);

        void setBody(std::unique_ptr<AST::ASTBlock> &&);

        void checkerBody(Context &);

        void checkerName(Context &);

        std::shared_ptr<IR::IRFunc> generateFunc();

        void addLineNumber(int num);

    private:
        std::string name;

        std::vector<std::pair<std::string, std::unique_ptr<AST::ASTType>>> params;

        std::unique_ptr<AST::ASTType> return_type = std::make_unique<AST::ASTTypeNull>();

        std::unique_ptr<AST::Statement> body;

        int line_number;
    };


    class Program {
    public:

        Program() = default;

        ~Program() = default;

        void setName(std::string new_name);

        void addDecl(std::unique_ptr<ASTDeclaration> &&);

        void addFunction(std::unique_ptr<Function> &&);

        void checker();

        std::unique_ptr<IR::IRProgram> generateIR();

    private:
        std::string name;
        std::vector<std::unique_ptr<ASTDeclaration>> declarations;
        std::vector<std::unique_ptr<Function>> functions;
    };

}

#endif //COMPILER_AST_H
