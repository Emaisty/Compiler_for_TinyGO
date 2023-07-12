#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "AST.h"
#include "lexer.h"
#include <vector>

class Parser {
public:
    Parser(char *str = "");

    ~Parser() = default;

    bool parse();

private:

    std::vector<std::string> parseIdentifierList();

    std::unique_ptr<AST::ASTTypeStruct> parseStruct();

    std::unique_ptr<AST::ASTExpression> parseExpression();

    std::unique_ptr<AST::ASTType> parseType();

    std::unique_ptr<AST::Function> parseFunction();

    std::vector<std::unique_ptr<AST::ASTDeclaration> > parseConstDeclaration();

    std::vector<std::unique_ptr<AST::ASTDeclaration> > parseTypeDeclaration();

    std::vector<std::unique_ptr<AST::ASTDeclaration> > parseVarDeclaration();

    std::vector<std::unique_ptr<AST::ASTDeclaration> > parseDeclaration();

    void match(Token);

    void matchAndGoNext(Token tok);

    Token cur_tok;

    AST::Program program;

    Lexer lexer;

};


#endif //COMPILER_PARSER_H
