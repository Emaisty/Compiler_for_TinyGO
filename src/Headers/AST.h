#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <vector>
#include <string>


class ASTType {
public:
private:
    bool constt = false;
};

class Int : public ASTType {
public:
private:
    int bits;
};

class Double : public ASTType {

};

class Pointer : public ASTType {
public:

};

class Statement {
};

class Function {
public:
private:
    std::string name;
    std::vector<Statement *> body;
};

class Declaration : public Statement {
public:

private:
};


class Program {
public:

    Program() = default;

    ~Program();

    void setName(std::string new_name);

    void addDecl(Declaration* new_decl);

    void addFunction(Function* new_func);

private:
    std::string name;
    std::vector<Declaration *> declarations;
    std::vector<Function *> functions;
};


#endif //COMPILER_AST_H
