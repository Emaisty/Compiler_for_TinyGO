#include <iostream>

#include "parser.h"

int main() {

    Parser p("../main.go");
    auto tree = p.parse();
    if (!tree){
        std::cerr << "Unsuccessful parsing." << std::endl;
        return 0;
    }



    return 0;
}
