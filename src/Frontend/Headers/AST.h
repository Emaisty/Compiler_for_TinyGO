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
    struct ItemInNameSpace {

        ItemInNameSpace(std::string new_name, Type *new_type, int new_level)
                : name(new_name),
                  type(new_type),
                  level(new_level) {}

        std::string name;
        Type *type;
        int level;

    };

    struct ItemInTypeSpace {

        ItemInTypeSpace(std::string new_name, std::unique_ptr<Type> &&new_type, int new_level)
                : name(new_name),
                  type(std::move(new_type)),
                  level(new_level) {}

        std::string name;
        std::unique_ptr<Type> type;
        int level;

    };

    struct Context {

        std::vector<ItemInNameSpace> name_space;

        std::vector<ItemInTypeSpace> type_space;

        std::map<Type *, PointerType *> pointers;

        bool in_loop = false;
        bool in_switch = false;
        int level = 0;
        std::vector<Type *> return_type;

        void checkIfNameExist(std::string);

        void checkIfTypeExist(std::string);

        //Go into loop/switch
        void goDeeper(bool, bool);

        void goUp();

        Type *getTypeByTypeName(std::string);

        Type *getTypeByVarName(std::string);

        Type* addType(std::string, std::unique_ptr<Type> &&);

        void addIntoNameSpace(std::string, Type *);

        Type *getPointer(Type *);

        inline static std::vector<std::string> base_types = {""};

    private:

        std::stack<bool> pr_loop_status;
        std::stack<bool> pr_switch_status;

        int count_for_tmp_types = 0;


    };


    class ASTNode {
    public:
        ASTNode() = default;

        virtual ~ASTNode() = default;

        void addLineNumber(int line_num);

        virtual bool hasAddress();

        virtual bool isConst();

        virtual Type *checker(Context &) = 0;

    private:
        int line;
    };

    class ASTType : public ASTNode {
    public:

        virtual std::set<std::string> getDependencies() = 0;

    protected:
    };

    class ASTTypePointer : public ASTType {
    public:

        ASTTypePointer() = default;

        ASTTypePointer(std::unique_ptr<ASTType> &&new_type);

        ASTType *getValue();

        Type *checker(Context &) override;

        std::set<std::string> getDependencies() override;

    private:
        std::unique_ptr<ASTType> type;
    };

    class ASTTypeStruct : public ASTType {
    public:

        ASTTypeStruct() = default;

        void addField(std::string name, std::unique_ptr<ASTType> &&type);

        ASTType *findField(std::string name);

        Type *checker(Context &) override;

        std::set<std::string> getDependencies() override;

    private:
        std::set<std::pair<std::string, std::unique_ptr<ASTType >>> fileds;
    };

    class ASTTypeNamed : public ASTType {
    public:
        ASTTypeNamed(std::string new_name) : name(new_name) {};

        Type *checker(Context &) override;

        std::set<std::string> getDependencies() override;

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

        virtual std::set<std::string> getVarNames() = 0;

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

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

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

        bool hasAddress() override;

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

    private:
        Operator op;
        std::unique_ptr<ASTExpression> value;
    };

    class ASTFunctionCall : public ASTExpression {
    public:

        ASTFunctionCall(std::unique_ptr<ASTExpression> &&, std::vector<std::unique_ptr<ASTExpression>> &);

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

    private:
        std::unique_ptr<ASTExpression> name;
        std::vector<std::unique_ptr<ASTExpression>> arg;
    };

    class ASTMemberAccess : public ASTExpression {
    public:

        ASTMemberAccess(std::unique_ptr<ASTExpression> &&, std::string &);

        bool hasAddress() override;

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

    private:
        std::unique_ptr<ASTExpression> name;

        std::string member;
    };


    class ASTIntNumber : public ASTExpression {
    public:

        ASTIntNumber(const int new_value = 0);

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

    private:
        long long int value = 0;
    };

    class ASTFloatNumber : public ASTExpression {
    public:

        ASTFloatNumber(double new_value = 0);

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

    private:
        double value = 0;
    };

    class ASTBoolNumber : public ASTExpression {
    public:

        ASTBoolNumber(const bool new_value = false);

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

    private:
        bool value = false;
    };

    class ASTStruct : public ASTExpression {
    public:

        ASTStruct(std::unique_ptr<ASTTypeStruct> &&,
                  std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> &);

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

    private:
        std::unique_ptr<ASTType> type;
        std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> values;
    };

    class ASTVar : public ASTExpression {
    public:

        ASTVar(const std::string new_name);

        bool hasAddress() override;

        bool isConst() override;

        std::string getName();

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

    private:
        std::string name;
    };


    class ASTDeclaration : public Statement {
    public:

        ASTDeclaration(std::vector<std::string> &&new_name, std::vector<std::unique_ptr<ASTExpression>> &&,
                       std::unique_ptr<ASTType> &&);

        ASTDeclaration(std::vector<std::string> &&new_name, std::vector<std::unique_ptr<ASTExpression>> &&);

        ASTDeclaration(std::vector<std::string> &&new_name, std::unique_ptr<ASTType> &&);


        virtual std::vector<std::pair<std::string, std::set<std::string>>> globalPreInit() = 0;

    protected:
        std::vector<std::string> name;
        std::vector<std::unique_ptr<ASTExpression>> value;
        std::unique_ptr<ASTType> type;


    };


    class ASTTypeDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        std::vector<std::pair<std::string, std::set<std::string>>> globalPreInit() override;

        void globChecker(Context &);

        Type *checker(Context &) override;

    private:
    };

    class ASTVarDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        std::vector<std::pair<std::string, std::set<std::string>>> globalPreInit() override;

        Type *checker(Context &) override;

    private:


    };

    class ASTConstDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        std::vector<std::pair<std::string, std::set<std::string>>> globalPreInit() override;

        Type *checker(Context &) override;

    private:
    };

    class ASTBlock : public Statement {
    public:

        ASTBlock() = default;

        void addStatement(std::unique_ptr<AST::Statement> &&);

        Type *checker(Context &) override;

    private:
        std::vector<std::unique_ptr<AST::Statement>> statements;
    };

    class ASTBreak : public Statement {
    public:

        ASTBreak() = default;

        Type *checker(Context &) override;

    private:

    };

    class ASTContinue : public Statement {
    public:

        ASTContinue() = default;

        Type *checker(Context &) override;

    private:

    };

    class ASTReturn : public Statement {
    public:

        ASTReturn() = default;

        void addReturnValue(std::unique_ptr<ASTExpression> &&);

        Type *checker(Context &) override;

    private:
        std::vector<std::unique_ptr<AST::ASTExpression>> return_value;
    };

    class ASTSwitch : public Statement {
    public:

        ASTSwitch() = default;

        void addExpr(std::unique_ptr<ASTExpression> &&);

        void addCase(std::unique_ptr<ASTExpression> &&, std::unique_ptr<AST::ASTBlock> &&);

        Type *checker(Context &) override;

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

        Type *checker(Context &) override;

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

        Type *checker(Context &) override;

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

        Type *checker(Context &) override;

    private:

        std::vector<std::unique_ptr<AST::ASTExpression>> variable, value;

        TypeOfAssign type;

    };


    class Function : public ASTNode {
    public:


        void setName(std::string new_name);

        void setParams(std::vector<std::unique_ptr<ASTDeclaration>> &&);

        void addParam(std::unique_ptr<ASTDeclaration> &&);

        void addReturn(std::unique_ptr<AST::ASTType> &&);

        void setBody(std::unique_ptr<AST::ASTBlock> &&);

        Type *checker(Context &) override;

    private:
        std::string name;

        std::vector<std::unique_ptr<ASTDeclaration>> params;

        std::vector<std::unique_ptr<AST::ASTType>> return_type;

        std::unique_ptr<AST::Statement> body;

    };


    class Program : public ASTNode {
    public:

        Program() = default;

        ~Program() = default;

        void setName(std::string new_name);

        void addVarDecl(std::unique_ptr<ASTDeclaration> &&);

        void addTypeDecl(std::unique_ptr<ASTDeclaration> &&);

        void addFunction(std::unique_ptr<Function> &&);

        Type *checker(Context &) override;

        std::queue<std::string> topSort(std::vector<std::pair<std::string, std::set<std::string>>> &);

    private:
        std::string name;
        std::vector<std::unique_ptr<ASTDeclaration>> typeDeclarations;
        std::vector<std::unique_ptr<ASTDeclaration>> varDeclarations;
        std::vector<std::unique_ptr<Function>> functions;
    };

}

#endif //COMPILER_AST_H
