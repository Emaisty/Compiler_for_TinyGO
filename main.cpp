#include <iostream>

#include "parser.h"

int main() {

    Parser p("../main.go");
    auto root = p.parse();
    if (!root)
        return 0;

    AST::Context ctx;
    root->checker(ctx);


    IR::Context ctx1;
    auto IR = root->generateIR(ctx1);

    return 0;
}
