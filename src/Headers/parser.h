#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "AST.h"
#include "lexer.h"

class Parser {
public:
    Parser(char *str = "");

    ~Parser();

    bool parse();

private:

    Function *parseFunction();

    Declaration *parseDeclaration();

    void match(Token);

    void matchAndGoNext(Token tok);

    Token cur_tok;

    Program program;

    Lexer lexer;

};


#endif //COMPILER_PARSER_H
