#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <vector>
#include <string>
#include <memory>


namespace AST {

    class ASTType {
    public:

        ASTType() = default;

        virtual ~ASTType() = default;

        virtual std::unique_ptr<ASTType> clone() const = 0;

    private:
    };

    class ASTTypeInt : public ASTType {
    public:

        ASTTypeInt(int new_bits = 32);

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

    private:
        int bits;
    };

    class ASTTypeFloat : public ASTType {
    public:

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

    private:
    };

    class ASTTypeBool : public ASTType {
    public:

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

    private:
    };

    class ASTTypePointer : public ASTType {
    public:

        ASTTypePointer(std::unique_ptr<ASTType> new_type);

        ASTTypePointer(const ASTTypePointer &old_pointer);

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

    private:
        std::unique_ptr<ASTType> type;
    };

    class ASTTypeStruct : public ASTType {
    public:

        ASTTypeStruct(const ASTTypeStruct &old_struct);

        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

        void addField(std::string name, std::unique_ptr<ASTType> &type);

    private:
        std::vector<std::pair<std::string, std::unique_ptr<ASTType >>> fileds;
    };

    class Statement {
    public:

        Statement() = default;

        virtual ~Statement() = default;

        virtual std::unique_ptr<Statement> clone() const = 0;

    private:
    };

    class ASTExpression : public Statement {
    public:

        [[nodiscard]] std::unique_ptr<Statement> clone() const override;

        virtual std::unique_ptr<ASTExpression> cloneExpr() const = 0;

        virtual std::unique_ptr<ASTType> getType() const = 0;

    private:
    };

    class ASTBinaryOperator : public ASTExpression {
    public:

        enum Operator {
            OR, AND, BINOR, BINAND, PLUS, MINUS, MUL, DIV, MOD, EQ, NE, GT, GE, LT, LE
        };

        ASTBinaryOperator(const ASTBinaryOperator &old_expr);

        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType() const override;

        ASTBinaryOperator(std::unique_ptr<ASTExpression> &new_left, std::unique_ptr<ASTExpression> &new_right,
                          Operator new_op = PLUS);

    private:
        std::unique_ptr<ASTExpression> left, right;
        Operator op;
    };

    class ASTUnaryOperator : public ASTExpression {
    public:
        enum Operator {
            NOT, PLUS, MINUS
        };

        ASTUnaryOperator(const ASTUnaryOperator &old_expr);

        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType() const override;

        ASTUnaryOperator(std::unique_ptr<ASTExpression> &new_value, Operator new_op = PLUS);

    private:
        Operator op;
        std::unique_ptr<ASTExpression> value;
    };

    class ASTFunctionCall : public ASTExpression {
    public:

        ASTFunctionCall(const ASTFunctionCall &old_func);

        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType() const override;

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

        [[nodiscard]] std::unique_ptr<ASTType> getType() const override;

        ASTMemberAccess(const std::unique_ptr<ASTExpression> &new_name,
                        const std::unique_ptr<ASTExpression> &new_member);

    private:
        std::unique_ptr<ASTExpression> name, member;
    };

    class ASTIntNumber : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType() const override;

        ASTIntNumber(const int new_value = 0);

    private:
        long long int value = 0;
    };

    class ASTFloatNumber : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType() const override;

        ASTFloatNumber(double new_value = 0);

    private:
        double value = 0;
    };

    class ASTBoolNumber : public ASTExpression {
    public:
        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType() const override;

        ASTBoolNumber(const bool new_value = false);

    private:
        bool value = false;
    };

    class ASTVar : public ASTExpression {
    public:

        [[nodiscard]] std::unique_ptr<ASTExpression> cloneExpr() const override;

        [[nodiscard]] std::unique_ptr<ASTType> getType() const override;

        ASTVar(const std::string new_name);

    private:
        std::string name;
    };


    class ASTDeclaration : public Statement {
    public:

        ASTDeclaration(const ASTDeclaration &pr_decl);

        [[nodiscard]] std::unique_ptr<Statement> clone() const override;

        ASTDeclaration(const std::string new_name, const std::unique_ptr<ASTExpression> &new_value,
                       const std::unique_ptr<ASTType> &new_type);

        ASTDeclaration(const std::string new_name, const std::unique_ptr<ASTType> &new_type);

        virtual std::unique_ptr<ASTDeclaration> cloneDecl() const = 0;

    private:
        std::string name;
        std::unique_ptr<ASTExpression> value;
        std::unique_ptr<ASTType> type;


    };


    class ASTTypeDeclaration : public ASTDeclaration {
    public:
        [[nodiscard]] std::unique_ptr<ASTDeclaration> cloneDecl() const override;

        using ASTDeclaration::ASTDeclaration;
    private:
    };

    class ASTVarDeclaration : public ASTDeclaration {
    public:
        [[nodiscard]] std::unique_ptr<ASTDeclaration> cloneDecl() const override;

        using ASTDeclaration::ASTDeclaration;
    private:


    };

    class ASTConstDeclaration : public ASTDeclaration {
    public:
        [[nodiscard]] std::unique_ptr<ASTDeclaration> cloneDecl() const override;

        using ASTDeclaration::ASTDeclaration;
    private:
    };


    class Function {
    public:
    private:
        std::string name;
        std::vector<std::unique_ptr<Statement>> body;
    };


    class Program {
    public:

        Program() = default;

        ~Program() = default;

        void setName(std::string new_name);

        void addDecl(std::unique_ptr<ASTDeclaration> &new_decl);

        void addFunction(std::unique_ptr<Function> &new_func);

    private:
        std::string name;
        std::vector<std::unique_ptr<ASTDeclaration>> declarations;
        std::vector<std::unique_ptr<Function>> functions;
    };

}

#endif //COMPILER_AST_H
