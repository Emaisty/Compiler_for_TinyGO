#include "AST.h"

Program::~Program() {
    for (auto &i: declarations)
        delete i;

    for (auto &i: functions)
        delete i;
}

void Program::setName(std::string new_name) {
    name = new_name;
}

void Program::addDecl(Declaration *new_decl) {
    declarations.push_back(new_decl);
}

void Program::addFunction(Function *new_func) {
    functions.push_back(new_func);
}