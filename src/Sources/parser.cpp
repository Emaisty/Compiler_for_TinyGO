#include "parser.h"

void Parser::match(Token tok) {
    if (cur_tok != tok)
        throw "ERROR. Unmatched tokens";
}

void Parser::matchAndGoNext(Token tok) {
    match(tok);
    cur_tok = lexer.gettok();
}

Parser::Parser(char *str) {
    lexer.InitInput(str);
    cur_tok = lexer.gettok();
}

bool Parser::parse() {
    matchAndGoNext(tok_package);

    match(tok_identifier);
    program.setName(lexer.identifierStr());

    cur_tok = lexer.gettok();

    while (cur_tok != tok_eof) {
        switch (cur_tok) {
            // function
            case tok_func:
                program.addFunction(parseFunction());
                break;
                // declaration
            case tok_const:
            case tok_type:
            case tok_var:
                program.addDecl(parseDeclaration());
                break;
            default:
                return false;
        }

    }
    return true;

}