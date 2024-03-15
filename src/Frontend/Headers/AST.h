#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <vector>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <string>
#include <memory>
#include <algorithm>

#include "types.h"
#include "IR.h"


namespace AST {

    struct ContextForIR {
        std::map<std::string, IR::IRLine *> namePool;
        IR::IRLine *continueLabel;
        IR::IRLine *breakLabel;
        IR::IRFunc *topFunc;
        IR::IRProgram *program;
    };


    class ASTType;


    class ASTNode {
    public:
        ASTNode() = default;

        virtual ~ASTNode() = default;

        void addLineNumber(int line_num);

        virtual void generateIR(ContextForIR &) = 0;

    protected:
        int line;

    };


    class ASTType : public ASTNode {
    public:
    protected:
    };

    class ASTTypePointer : public ASTType {
    public:

        ASTTypePointer() = default;

        ASTTypePointer(std::unique_ptr<ASTType> &&new_type);

        ASTType *getValue();

        void generateIR(ContextForIR &) override;

    private:
        std::unique_ptr<ASTType> type;
    };

    class ASTTypeStruct : public ASTType {
    public:

        ASTTypeStruct() = default;

        void addField(std::string name, std::unique_ptr<ASTType> &&type);

        ASTType *findField(std::string name) const;

        void generateIR(ContextForIR &) override;


    private:
        std::vector<std::pair<std::string, std::unique_ptr<ASTType >>> fileds;
    };

    class ASTTypeNamed : public ASTType {
    public:
        ASTTypeNamed(std::string new_name) : name(new_name) {};

        void generateIR(ContextForIR &) override;

    private:
        std::string name;
    };

    class Statement : public ASTNode {
    public:

        Statement() = default;

        virtual ~Statement() = default;

    private:

    };

    class ASTExpression : public Statement {
    public:
    private:
    };

//    class ASTNullExpr : public ASTExpression {
//    public:
//
//    private:
//    };

    class ASTBinaryOperator : public ASTExpression {
    public:

        enum Operator {
            OR, AND, BINOR, BINAND, PLUS, MINUS, MUL, DIV, MOD, EQ, NE, GT, GE, LT, LE
        };

        ASTBinaryOperator(std::unique_ptr<ASTExpression> &&, std::unique_ptr<ASTExpression> &&,
                          Operator new_op = PLUS);

        void generateIR(ContextForIR &) override;

    private:
        std::unique_ptr<ASTExpression> left, right;
        Operator op;
    };

    class ASTUnaryOperator : public ASTExpression {
    public:
        enum Operator {
            NOT, PLUS, MINUS, PREINC, PREDEC, POSTINC, POSTDEC, REFER, DEREFER
        };

        ASTUnaryOperator(std::unique_ptr<ASTExpression> &&, Operator new_op = PLUS);

        void generateIR(ContextForIR &) override;

    private:
        Operator op;
        std::unique_ptr<ASTExpression> value;
    };

    class ASTFunctionCall : public ASTExpression {
    public:

        ASTFunctionCall(std::unique_ptr<ASTExpression> &&, std::vector<std::unique_ptr<ASTExpression>> &);

        void generateIR(ContextForIR &) override;

    private:
        std::unique_ptr<ASTExpression> name;
        std::vector<std::unique_ptr<ASTExpression>> arg;
    };

    class ASTMemberAccess : public ASTExpression {
    public:

        ASTMemberAccess(std::unique_ptr<ASTExpression> &&, std::string &);

        void generateIR(ContextForIR &) override;

    private:
        std::unique_ptr<ASTExpression> name;

        std::string member;
    };


    class ASTIntNumber : public ASTExpression {
    public:

        ASTIntNumber(const int new_value = 0);

        void generateIR(ContextForIR &) override;

    private:
        long long int value = 0;
    };

    class ASTFloatNumber : public ASTExpression {
    public:

        ASTFloatNumber(double new_value = 0);

        void generateIR(ContextForIR &) override;

    private:
        double value = 0;
    };

    class ASTBoolNumber : public ASTExpression {
    public:

        ASTBoolNumber(const bool new_value = false);

        void generateIR(ContextForIR &) override;

    private:
        bool value = false;
    };

    class ASTStruct : public ASTExpression {
    public:

        ASTStruct(std::unique_ptr<ASTTypeStruct> &&,
                  std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> &);

        void generateIR(ContextForIR &) override;

    private:
        std::unique_ptr<ASTType> type;
        std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> values;
    };

    class ASTVar : public ASTExpression {
    public:

        ASTVar(const std::string new_name);

        std::string getName();

        void generateIR(ContextForIR &) override;

    private:
        std::string name;

        bool is_const = false;
    };


    class ASTDeclaration : public Statement {
    public:

        ASTDeclaration(std::vector<std::string> &&new_name, std::vector<std::unique_ptr<ASTExpression>> &&,
                       std::unique_ptr<ASTType> &&);

        ASTDeclaration(std::vector<std::string> &&new_name, std::vector<std::unique_ptr<ASTExpression>> &&);

        ASTDeclaration(std::vector<std::string> &&new_name, std::unique_ptr<ASTType> &&);


    protected:
        std::vector<std::string> name;
        std::vector<std::unique_ptr<ASTExpression>> value;
        std::unique_ptr<ASTType> type;


    };


    class ASTTypeDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        void generateIR(ContextForIR &) override;

    private:
    };

    class ASTVarDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        void generateIR(ContextForIR &) override;

    private:


    };

    class ASTConstDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        void generateIR(ContextForIR &) override;

    private:
    };

    class ASTBlock : public Statement {
    public:

        ASTBlock() = default;

        void addStatement(std::unique_ptr<AST::Statement> &&);

        void generateIR(ContextForIR &) override;

    private:
        std::vector<std::unique_ptr<AST::Statement>> statements;
    };

    class ASTBreak : public Statement {
    public:

        ASTBreak() = default;

        void generateIR(ContextForIR &) override;

    private:

    };

    class ASTContinue : public Statement {
    public:

        ASTContinue() = default;

        void generateIR(ContextForIR &) override;

    private:

    };

    class ASTReturn : public Statement {
    public:

        ASTReturn() = default;

        void addReturnValue(std::unique_ptr<ASTExpression> &&);

        void generateIR(ContextForIR &) override;

    private:
        std::vector<std::unique_ptr<AST::ASTExpression>> return_value;
    };

    class ASTSwitch : public Statement {
    public:

        ASTSwitch() = default;

        void addExpr(std::unique_ptr<ASTExpression> &&);

        void addCase(std::unique_ptr<ASTExpression> &&, std::unique_ptr<AST::ASTBlock> &&);

        void generateIR(ContextForIR &) override;

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

        void generateIR(ContextForIR &) override;

    private:
        std::unique_ptr<AST::ASTExpression> expr;

        std::unique_ptr<AST::Statement> if_clause, else_clause;
    };

    class ASTFor : public Statement {
    public:

        ASTFor() = default;

        void addInitClause(std::vector<std::unique_ptr<AST::Statement>> &&);

        void addIterClause(std::vector<std::unique_ptr<AST::Statement>> &&);

        void addCondClause(std::unique_ptr<AST::ASTExpression> &&);

        void addBody(std::unique_ptr<AST::ASTBlock> &&);

        void generateIR(ContextForIR &) override;

    private:
        std::vector<std::unique_ptr<AST::Statement>> init_clause, iterate_clause;

        std::unique_ptr<AST::ASTExpression> if_clause;

        std::unique_ptr<AST::Statement> body;
    };

    class ASTAssign : public Statement {
    public:
        enum TypeOfAssign {
            ASSIGN, PLUSASSIGN, MINUSASSIGN, MULTASSIGN, DIVASSIGN, MODASSIGN
        };

        ASTAssign() = default;

        ASTAssign(std::vector<std::unique_ptr<AST::ASTExpression>> &&,
                  std::vector<std::unique_ptr<AST::ASTExpression>> &&,
                  TypeOfAssign new_type);

        void generateIR(ContextForIR &) override;

    private:

        std::vector<std::unique_ptr<AST::ASTExpression>> variable, value;

        TypeOfAssign type;

    };


    class Function : public ASTNode {
    public:
        void setName(std::string new_name);

        void addParam(std::vector<std::string> &&, std::unique_ptr<ASTType> &&);

        void setMethod(std::string, std::unique_ptr<ASTType> &&);

        void addReturn(std::unique_ptr<AST::ASTType> &&);

        void setBody(std::unique_ptr<AST::ASTBlock> &&);

        void generateIR(ContextForIR &) override;

    private:
        std::string name;

        std::vector<std::pair<std::vector<std::string>, std::unique_ptr<ASTType>>> params;

        std::unique_ptr<AST::ASTType> return_type;

        std::unique_ptr<AST::Statement> body;

        std::unique_ptr<ASTType> type_of_method;

        std::string inner_name;

    };


    class Program : public ASTNode {
    public:

        Program() = default;

        ~Program() = default;

        void setName(std::string new_name);

        void addVarDecl(std::unique_ptr<ASTDeclaration> &&);

        void addTypeDecl(std::unique_ptr<ASTDeclaration> &&);

        void addFunction(std::unique_ptr<Function> &&);

        void generateIR(ContextForIR &) override;

    private:
        std::string name;
        std::vector<std::unique_ptr<ASTDeclaration>> typeDeclarations;
        std::vector<std::unique_ptr<ASTDeclaration>> varDeclarations;
        std::vector<std::unique_ptr<Function>> functions;
    };

}

#endif //COMPILER_AST_H
