#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "AST.h"
#include "lexer.h"
#include <vector>
#include <map>
#include <memory>

class Parser {
public:
    Parser(char *str = "");

    ~Parser() = default;

    bool parse();

private:

    bool checkForSeparator();

    bool matchTypes(const std::unique_ptr<AST::ASTType> &type, const std::unique_ptr<AST::ASTType> &reference_type);

    std::vector<std::string> parseIdentifierList();

    std::unique_ptr<AST::ASTTypeStruct> parseStruct();

    std::unique_ptr<AST::ASTExpression> E11();

    std::unique_ptr<AST::ASTExpression> E11_PRIME(std::unique_ptr<AST::ASTExpression> &left);

    std::unique_ptr<AST::ASTExpression> E10();

    std::unique_ptr<AST::ASTExpression> E10_PRIME(std::unique_ptr<AST::ASTExpression> &left);

    std::unique_ptr<AST::ASTExpression> E9();

    std::unique_ptr<AST::ASTExpression> E9_PRIME(std::unique_ptr<AST::ASTExpression> &left);

    std::unique_ptr<AST::ASTExpression> E8();

    std::unique_ptr<AST::ASTExpression> E8_PRIME(std::unique_ptr<AST::ASTExpression> &left);

    std::unique_ptr<AST::ASTExpression> E7();

    std::unique_ptr<AST::ASTExpression> E7_PRIME(std::unique_ptr<AST::ASTExpression> &left);

    std::unique_ptr<AST::ASTExpression> E6();

    std::unique_ptr<AST::ASTExpression> E6_PRIME(std::unique_ptr<AST::ASTExpression> &left);

    std::unique_ptr<AST::ASTExpression> E5();

    std::unique_ptr<AST::ASTExpression> E5_PRIME(std::unique_ptr<AST::ASTExpression> &left);

    std::unique_ptr<AST::ASTExpression> E4();

    std::unique_ptr<AST::ASTExpression> E4_PRIME(std::unique_ptr<AST::ASTExpression> &left);

    //std::unique_ptr<AST::ASTExpression> E3();

    //std::unique_ptr<AST::ASTExpression> E3_PRIME(std::unique_ptr<AST::ASTExpression> &left);

    std::unique_ptr<AST::ASTExpression> E2();

    std::unique_ptr<AST::ASTExpression> E1();

    std::unique_ptr<AST::ASTExpression> E1_PRIME(std::unique_ptr<AST::ASTExpression> &left);

    std::unique_ptr<AST::ASTExpression> E0();


    std::unique_ptr<AST::ASTExpression> parseExpression();

    std::vector<std::unique_ptr<AST::ASTExpression>> parseExpressionList();

    std::unique_ptr<AST::ASTType> parseType();

    std::unique_ptr<AST::Function> parseFunction();


    std::vector<std::unique_ptr<AST::ASTDeclaration> > parseConstDeclarationLine();

    std::vector<std::unique_ptr<AST::ASTDeclaration>> parseTypeDeclarationLine();

    std::vector<std::unique_ptr<AST::ASTDeclaration> > parseVarDeclarationLine();


    std::vector<std::unique_ptr<AST::ASTDeclaration> >
    parseDeclarationBlock(const std::function<std::vector<std::unique_ptr<AST::ASTDeclaration>>()> &type_of_line);

    std::vector<std::unique_ptr<AST::ASTDeclaration> > parseDeclaration();

    void match(Token);

    void matchAndGoNext(Token tok);

    Token cur_tok;

    AST::Program program;

    Lexer lexer;

};


#endif //COMPILER_PARSER_H
