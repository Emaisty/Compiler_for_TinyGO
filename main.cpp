#include <iostream>

#include "parser.h"

int main() {

    Parser p("../main.go");
    auto root = p.parse();
    if (!root)
        return 0;

    AST::Context ctx;
    root->checker(ctx);

    auto IRctx = ctx.createIRContext();

    auto IR = root->generateIR(IRctx);

    IR->print(std::cout);

    return 0;
}
