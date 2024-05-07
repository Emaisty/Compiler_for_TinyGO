#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "AST.h"
#include "lexer.h"
#include <vector>
#include <map>
#include <memory>

class Parser {
public:
    Parser(std::string);

    ~Parser() = default;

    std::unique_ptr<AST::Program> parse();

private:

    std::vector<std::string> parseIdentifierList();

    std::unique_ptr<AST::ASTTypeStruct> parseStruct();

    std::unique_ptr<AST::ASTExpression> E11();

    std::unique_ptr<AST::ASTExpression> E11_PRIME(std::unique_ptr<AST::ASTExpression> &&);

    std::unique_ptr<AST::ASTExpression> E10();

    std::unique_ptr<AST::ASTExpression> E10_PRIME(std::unique_ptr<AST::ASTExpression> &&);

    std::unique_ptr<AST::ASTExpression> E9();

    std::unique_ptr<AST::ASTExpression> E9_PRIME(std::unique_ptr<AST::ASTExpression> &&);

    std::unique_ptr<AST::ASTExpression> E8();

    std::unique_ptr<AST::ASTExpression> E8_PRIME(std::unique_ptr<AST::ASTExpression> &&);

    std::unique_ptr<AST::ASTExpression> E7();

    std::unique_ptr<AST::ASTExpression> E7_PRIME(std::unique_ptr<AST::ASTExpression> &&);

    std::unique_ptr<AST::ASTExpression> E6();

    std::unique_ptr<AST::ASTExpression> E6_PRIME(std::unique_ptr<AST::ASTExpression> &&);

    std::unique_ptr<AST::ASTExpression> E5();

    std::unique_ptr<AST::ASTExpression> E5_PRIME(std::unique_ptr<AST::ASTExpression> &&);

    std::unique_ptr<AST::ASTExpression> E4();

    std::unique_ptr<AST::ASTExpression> E4_PRIME(std::unique_ptr<AST::ASTExpression> &&);

    //std::unique_ptr<AST::ASTExpression> E3();

    //std::unique_ptr<AST::ASTExpression> E3_PRIME(std::unique_ptr<AST::ASTExpression> &left);

    std::unique_ptr<AST::ASTExpression> E2();

    std::unique_ptr<AST::ASTExpression> E1();

    std::unique_ptr<AST::ASTExpression> E1_PRIME(std::unique_ptr<AST::ASTExpression> &&);

    std::unique_ptr<AST::ASTExpression> E0();


    std::unique_ptr<AST::ASTExpression> parseExpressionOrNone();

    std::vector<std::unique_ptr<AST::ASTExpression>> parseExpressionListOrNone();

    std::unique_ptr<AST::ASTExpression> parseExpression();

    std::vector<std::unique_ptr<AST::ASTExpression>> parseExpressionList();

    std::vector<std::unique_ptr<AST::Statement>> parseSimpleStat();


    std::unique_ptr<AST::Statement> parseReturn();

    std::unique_ptr<AST::Statement> parseIfStat();

    std::unique_ptr<AST::Statement> parseForLoop();

    std::unique_ptr<AST::Statement> parseSwitch();

    std::unique_ptr<AST::Statement> parseScan();

    std::unique_ptr<AST::Statement> parsePrint();


    std::vector<std::unique_ptr<AST::Statement>> parseStatement();

    std::vector<std::unique_ptr<AST::Statement>> parseStatementList();


    std::unique_ptr<AST::ASTType> parseType();

    std::unique_ptr<AST::ASTBlock> parseBlock();


    void parseFuncSignature(std::unique_ptr<AST::Function> &function);

    std::unique_ptr<AST::Function> parseFunction();


    std::vector<std::unique_ptr<AST::ASTDeclaration> > parseConstDeclarationLine();

    std::vector<std::unique_ptr<AST::ASTDeclaration>> parseTypeDeclarationLine();

    std::vector<std::unique_ptr<AST::ASTDeclaration> > parseVarDeclarationLine();


    std::vector<std::unique_ptr<AST::ASTDeclaration> >
    parseDeclarationBlock(const std::function<std::vector<std::unique_ptr<AST::ASTDeclaration>>()> &type_of_line);

    std::vector<std::unique_ptr<AST::ASTDeclaration> > parseDeclaration();

    void match(Token);

    void matchAndGoNext(Token tok);

    bool checkForSeparatorAndSkip();

    std::map<std::string, std::unique_ptr<AST::ASTType>> named_type;

    Token cur_tok;

    Lexer lexer;

};


#endif //COMPILER_PARSER_H
