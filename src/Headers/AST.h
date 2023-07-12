#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <vector>
#include <string>


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

        std::unique_ptr<ASTType> clone() const override;

        std::unique_ptr<AST::ASTTypeInt> kek();

    private:
        int bits;
    };

    class ASTTypeDouble : public ASTType {
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

    class ASTTypeNamed : public ASTType {
    public:

        ASTTypeNamed(std::string new_name);


        [[nodiscard]] std::unique_ptr<ASTType> clone() const override;

    private:
        std::string name;
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

        void setType(std::unique_ptr<ASTType> &new_type);

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

        void addDecl(std::unique_ptr<ASTDeclaration> &new_decl);

        void addFunction(std::unique_ptr<Function> &new_func);

    private:
        std::string name;
        std::vector<std::unique_ptr<ASTDeclaration>> declarations;
        std::vector<std::unique_ptr<Function>> functions;
    };

}

#endif //COMPILER_AST_H
