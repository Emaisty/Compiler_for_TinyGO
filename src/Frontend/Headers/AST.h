#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <vector>
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

        std::unique_ptr<ASTType> set_addressability(bool flag);

        std::unique_ptr<ASTType> set_constability(bool flag);

        bool addressable();

        bool constable();

        virtual std::shared_ptr<IR::IRType> generateIR() = 0;

    protected:
        bool is_variable = false;

        bool is_const = true;
    };


    struct Variable {

        enum TypeOfVariable {
            FUNC, VAR, TYPE, CONST
        };
        std::string name;

        int level;

        std::unique_ptr<ASTType> type;
        // if function -- params
        std::vector<std::unique_ptr<ASTType>> params;

        enum TypeOfVariable type_of_var;

        Variable(std::string new_name, std::unique_ptr<ASTType> &new_type, int new_level, TypeOfVariable new_type_of)
                : name(new_name),
                  type(new_type->clone()),
                  level(new_level),
                  type_of_var(new_type_of) {}

        void addParam(std::unique_ptr<ASTType> &new_type) {
            params.emplace_back(new_type->clone());
        }
    };

    struct Info {
        bool in_loop = false;
        int level = 0;
        std::unique_ptr<ASTType> return_type = nullptr;

        void goDeeper();

        void goUp();

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

        ASTTypePointer(std::unique_ptr<ASTType> new_type);

        ASTTypePointer(const ASTTypePointer &old_pointer);

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

        bool operator==(const std::unique_ptr<ASTType> &&type) const override;

        std::unique_ptr<ASTType> getValue();

        std::shared_ptr<IR::IRType> generateIR() override;

    private:
        std::unique_ptr<ASTType> type;
    };

    class ASTTypeStruct : public ASTType {
    public:

        ASTTypeStruct() = default;

        ASTTypeStruct(const ASTTypeStruct &old_struct);

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

        void addField(std::string name, std::unique_ptr<ASTType> &type);

        bool operator==(const std::unique_ptr<ASTType> &&type) const override;

        std::unique_ptr<ASTType> findField(std::string name);

        std::shared_ptr<IR::IRType> generateIR() override;

    private:
        std::vector<std::pair<std::string, std::unique_ptr<ASTType >>> fileds;
    };

    class Statement {
    public:

        Statement() = default;

        virtual ~Statement() = default;

        virtual std::unique_ptr<Statement> clone() const = 0;

        virtual void checker(std::vector<Variable> &, Info &) = 0;

        void addLineNumber(int);

    private:
        int line_number;
    };

    class ASTExpression : public Statement {
    public:

        [[nodiscard]] std::unique_ptr<Statement> clone() const override;

        virtual std::unique_ptr<ASTExpression> cloneExpr() const = 0;

        virtual std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const = 0;

    private:
    };

    class ASTNullExpr : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;
    };

    class ASTBinaryOperator : public ASTExpression {
    public:

        enum Operator {
            OR, AND, BINOR, BINAND, PLUS, MINUS, MUL, DIV, MOD, EQ, NE, GT, GE, LT, LE
        };

        ASTBinaryOperator(const ASTBinaryOperator &old_expr);

        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;

        ASTBinaryOperator(std::unique_ptr<ASTExpression> &new_left, std::unique_ptr<ASTExpression> &new_right,
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

        ASTUnaryOperator(const ASTUnaryOperator &old_expr);

        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;

        ASTUnaryOperator(std::unique_ptr<ASTExpression> &new_value, Operator new_op = PLUS);

    private:
        Operator op;
        std::unique_ptr<ASTExpression> value;
    };

    class ASTFunctionCall : public ASTExpression {
    public:

        ASTFunctionCall(const ASTFunctionCall &old_func);

        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;

        ASTFunctionCall(const std::unique_ptr<ASTExpression> &new_name,
                        const std::vector<std::unique_ptr<ASTExpression>> &new_args);

    private:
        std::unique_ptr<ASTExpression> name;
        std::vector<std::unique_ptr<ASTExpression>> arg;
    };

    class ASTMemberAccess : public ASTExpression {
    public:
        ASTMemberAccess(const ASTMemberAccess &old_access);

        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;

        ASTMemberAccess(const std::unique_ptr<ASTExpression> &new_name,
                        const std::unique_ptr<ASTExpression> &new_member);

    private:
        std::unique_ptr<ASTExpression> name, member;
    };

    class ASTGetPointer : public ASTExpression {
    public:
        ASTGetPointer(const ASTGetPointer &old_address);

        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;

        ASTGetPointer(const std::unique_ptr<ASTExpression> &new_var);

    private:
        std::unique_ptr<ASTExpression> var;
    };

    class ASTGetValue : public ASTExpression {
    public:
        ASTGetValue(const ASTGetValue &old_pointer);

        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;

        ASTGetValue(const std::unique_ptr<ASTExpression> &new_var);

    private:
        std::unique_ptr<ASTExpression> var;
    };


    class ASTIntNumber : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;

        ASTIntNumber(const int new_value = 0);

    private:
        long long int value = 0;
    };

    class ASTFloatNumber : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;

        ASTFloatNumber(double new_value = 0);

    private:
        double value = 0;
    };

    class ASTBoolNumber : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;

        ASTBoolNumber(const bool new_value = false);

    private:
        bool value = false;
    };

    class ASTStruct : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;

        ASTStruct(std::unique_ptr<ASTTypeStruct> &new_type,
                  std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> &);

        ASTStruct(const ASTStruct &old_struct);

    private:
        std::unique_ptr<ASTType> type;
        std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> values;
    };

    class ASTVar : public ASTExpression {
    public:

        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType(const std::vector<Variable> &) const override;

        void checker(std::vector<Variable> &, Info &) override;

        ASTVar(const std::string new_name);

        std::string getName();

    private:
        std::string name;
    };


    class ASTDeclaration : public Statement {
    public:

        ASTDeclaration(const ASTDeclaration &pr_decl);

        [[nodiscard]] std::unique_ptr<Statement> clone() const override;

        ASTDeclaration(const std::string new_name, const std::unique_ptr<ASTExpression> &new_value,
                       const std::unique_ptr<ASTType> &new_type);

        ASTDeclaration(const std::string new_name, const std::unique_ptr<ASTExpression> &new_value);

        ASTDeclaration(const std::string new_name, const std::unique_ptr<ASTType> &new_type);

        virtual std::unique_ptr<ASTDeclaration> cloneDecl() const = 0;

        virtual void checkerGlobal(std::vector<Variable> &) = 0;

        virtual std::shared_ptr<IR::IRGlobalDecl> generateIRGlobal() const = 0;


    protected:
        std::string name;
        std::unique_ptr<ASTExpression> value;
        std::unique_ptr<ASTType> type;


    };


    class ASTTypeDeclaration : public ASTDeclaration {
    public:
        [[nodiscard]] std::unique_ptr<ASTDeclaration> cloneDecl() const override;

        using ASTDeclaration::ASTDeclaration;

        void checkerGlobal(std::vector<Variable> &) override;

        void checker(std::vector<Variable> &, Info &) override;

        std::shared_ptr<IR::IRGlobalDecl> generateIRGlobal() const override;

    private:
    };

    class ASTVarDeclaration : public ASTDeclaration {
    public:
        [[nodiscard]] std::unique_ptr<ASTDeclaration> cloneDecl() const override;

        using ASTDeclaration::ASTDeclaration;

        void checkerGlobal(std::vector<Variable> &) override;

        void checker(std::vector<Variable> &, Info &) override;

        std::shared_ptr<IR::IRGlobalDecl> generateIRGlobal() const override;

    private:


    };

    class ASTConstDeclaration : public ASTDeclaration {
    public:
        [[nodiscard]] std::unique_ptr<ASTDeclaration> cloneDecl() const override;

        using ASTDeclaration::ASTDeclaration;

        void checkerGlobal(std::vector<Variable> &) override;

        void checker(std::vector<Variable> &, Info &) override;

        std::shared_ptr<IR::IRGlobalDecl> generateIRGlobal() const override;

    private:
    };

    class ASTBlock : public Statement {
    public:

        ASTBlock() = default;

        std::unique_ptr<Statement> clone() const override;

        ASTBlock(const ASTBlock &old_block);

        void addStatement(std::unique_ptr<AST::Statement> &stat);

        void checker(std::vector<Variable> &, Info &) override;

    private:
        std::vector<std::unique_ptr<AST::Statement>> statements;
    };

    class ASTBreak : public Statement {
    public:

        ASTBreak() = default;

        std::unique_ptr<Statement> clone() const override;

        void checker(std::vector<Variable> &, Info &) override;

    private:

    };

    class ASTContinue : public Statement {
    public:

        ASTContinue() = default;

        std::unique_ptr<Statement> clone() const override;

        void checker(std::vector<Variable> &, Info &) override;

    private:

    };

    class ASTReturn : public Statement {
    public:

        ASTReturn() = default;

        std::unique_ptr<Statement> clone() const override;

        ASTReturn(const ASTReturn &old_return);

        void addReturnValue(std::unique_ptr<ASTExpression> &new_value);

        void checker(std::vector<Variable> &, Info &) override;

    private:
        std::unique_ptr<AST::ASTExpression> return_value = std::make_unique<AST::ASTNullExpr>();
    };

    class ASTSwitch : public Statement {
    public:

        ASTSwitch() = default;

        std::unique_ptr<Statement> clone() const override;

        ASTSwitch(const ASTSwitch &old_switch);

        void addExpr(std::unique_ptr<ASTExpression> &new_expr);

        void addCase(std::unique_ptr<ASTExpression> &new_expr, std::unique_ptr<AST::Statement> &block);

        void checker(std::vector<Variable> &, Info &) override;

    private:
        std::unique_ptr<ASTExpression> expr;

        std::vector<std::pair<std::unique_ptr<AST::ASTExpression>, std::unique_ptr<AST::Statement>>> cases;
    };


    class ASTIf : public Statement {
    public:

        ASTIf() = default;

        std::unique_ptr<Statement> clone() const override;

        ASTIf(const ASTIf &old_if);

        void addExpr(std::unique_ptr<AST::ASTExpression> &new_expr);

        void addIfClause(std::unique_ptr<AST::ASTBlock> &new_if_clause);

        void addElseClause(std::unique_ptr<AST::ASTBlock> &new_else_clause);

        void checker(std::vector<Variable> &, Info &) override;

    private:
        std::unique_ptr<AST::ASTExpression> expr;

        std::unique_ptr<AST::Statement> if_clause, else_clause;
    };

    class ASTFor : public Statement {
    public:

        ASTFor() = default;

        std::unique_ptr<Statement> clone() const override;

        ASTFor(const ASTFor &old_for);

        void addInitClause(std::vector<std::unique_ptr<AST::Statement>> &new_init_clause);

        void addIterClause(std::vector<std::unique_ptr<AST::Statement>> &new_iter_clause);

        void addCondClause(std::unique_ptr<AST::ASTExpression> &new_condition);

        void addBody(std::unique_ptr<AST::Statement> &new_body);

        void checker(std::vector<Variable> &, Info &) override;

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

        ASTAssign(std::unique_ptr<AST::ASTExpression> &new_variable, std::unique_ptr<AST::ASTExpression> &new_value,
                  Type new_type);

        std::unique_ptr<Statement> clone() const override;

        ASTAssign(const ASTAssign &old_assign);

        void checker(std::vector<Variable> &, Info &) override;

    private:

        std::unique_ptr<AST::ASTExpression> variable, value;

        Type type;

    };


    class Function {
    public:


        void setName(std::string new_name);

        void setParams(std::vector<std::pair<std::string, std::unique_ptr<AST::ASTType>>> new_params);

        void addParam(std::string new_name, std::unique_ptr<AST::ASTType> &type);

        void addReturn(std::unique_ptr<AST::ASTType> &new_return);

        void setBody(std::unique_ptr<AST::Statement> &new_body);

        void checkerBody(std::vector<Variable> &);

        void checkerName(std::vector<Variable> &);

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

        void addDecl(std::unique_ptr<ASTDeclaration> &new_decl);

        void addFunction(std::unique_ptr<Function> &new_func);

        void checker();

        std::unique_ptr<IR::IRProgram> generateIR();

    private:
        std::string name;
        std::vector<std::unique_ptr<ASTDeclaration>> declarations;
        std::vector<std::unique_ptr<Function>> functions;
    };

}

#endif //COMPILER_AST_H
