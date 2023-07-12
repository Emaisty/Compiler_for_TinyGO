#include "AST.h"

void AST::ASTDeclaration::setName(std::string new_name) {
    name = new_name;
}

void AST::ASTDeclaration::setType(std::unique_ptr<AST::ASTType> new_type) {
    type = std::move(new_type);
}

void AST::Program::setName(std::string new_name) {
    name = new_name;
}

void AST::Program::addDecl(std::unique_ptr<ASTDeclaration>& new_decl) {
    declarations.push_back(std::move(new_decl));
}

void AST::Program::addFunction(std::unique_ptr<Function>& new_func) {
    functions.push_back(std::move(new_func));
}