#include <iostream>

#include "parser.h"

int main() {

    Parser p("../main.go");
    p.parse();

    return 0;
}
