#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <vector>
#include <string>


namespace AST {

    class ASTType {
    public:
    private:
    };

    class ASTTypeInt : public ASTType {
    public:
    private:
        int bits;
    };

    class ASTTypeDouble : public ASTType {
    public:
    private:
    };

    class ASTTypePointer : public ASTType {
    public:
    private:
        std::unique_ptr<ASTType> type;
    };

    class ASTTypeStruct : public ASTType {
    public:
    private:
        std::vector<std::pair<std::string, std::unique_ptr<ASTType >>> filed;
    };

    class ASTTypeNamed : public ASTType {
    public:
    private:
        std::string name;
        std::unique_ptr<ASTType> type;
    };

    class Statement {

    };

    class ASTExpression : public Statement {

    };

    class ASTBinaryOperator : public ASTExpression {

    };

    class ASTDeclaration : public Statement {
    public:

        void setName(std::string new_name);

        void setType(std::unique_ptr<ASTType> new_type);

    private:
        std::string name;
        std::unique_ptr<ASTType> type;


    };

    class ASTTypeDeclaration : public ASTDeclaration {
    public:
    private:
    };

    class ASTVarDeclaration : public ASTDeclaration {
    public:
    private:


    };

    class ASTConstDeclaration : public ASTDeclaration {
    public:
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

        void addDecl(std::unique_ptr<ASTDeclaration>& new_decl);

        void addFunction(std::unique_ptr<Function>& new_func);

    private:
        std::string name;
        std::vector<std::unique_ptr<ASTDeclaration>> declarations;
        std::vector<std::unique_ptr<Function>> functions;
    };

}

#endif //COMPILER_AST_H
