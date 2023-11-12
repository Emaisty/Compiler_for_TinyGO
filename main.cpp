#include <iostream>

#include "parser.h"

int main() {

    Parser p("../main.go");
    if (!p.parse())
        return 0;

//    p.printIR(std::cout);

    return 0;
}
