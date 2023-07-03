#include <iostream>
#include "lexer.h"

int main() {

    Lexer lex;
    char *t;
    t = "../main.go";
    lex.InitInput(t);

    Token tok;
    tok = lex.gettok();
    for (int i = 0; i < 10; ++i) {
        std::cout << tok << std::endl;
        tok = lex.gettok();
    }


    return 0;
}
