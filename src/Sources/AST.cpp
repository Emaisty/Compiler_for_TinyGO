#include "AST.h"

AST::ASTTypeInt::ASTTypeInt(int new_bits) : bits(new_bits) {}

std::unique_ptr<AST::ASTType> AST::ASTTypeInt::clone() const {

    return std::make_unique<ASTTypeInt>(*this);
}


std::unique_ptr<AST::ASTType> AST::ASTTypeDouble::clone() const {

    return std::make_unique<ASTTypeDouble>(*this);
}


AST::ASTTypePointer::ASTTypePointer(std::unique_ptr<ASTType> new_type) {
    type = new_type->clone();
}


AST::ASTTypePointer::ASTTypePointer(const AST::ASTTypePointer &old_pointer) {
    type = old_pointer.type->clone();
}

std::unique_ptr<AST::ASTType> AST::ASTTypePointer::clone() const {
    return std::make_unique<ASTTypePointer>(*this);
}


AST::ASTTypeStruct::ASTTypeStruct(const AST::ASTTypeStruct &old_pointer) {
    for (auto &i: old_pointer.fileds)
        fileds.emplace_back(i.first, i.second->clone());
}

std::unique_ptr<AST::ASTType> AST::ASTTypeStruct::clone() const {
    return std::make_unique<ASTTypeStruct>(*this);
}

void AST::ASTTypeStruct::addField(std::string name, std::unique_ptr<AST::ASTType> &type) {
    fileds.emplace_back(name, type->clone());
}


AST::ASTTypeNamed::ASTTypeNamed(std::string new_name) {
    name = new_name;
}

std::unique_ptr<AST::ASTType> AST::ASTTypeNamed::clone() const {

    return std::make_unique<ASTTypeNamed>(*this);
}


void AST::ASTDeclaration::setName(std::string new_name) {
    name = new_name;
}

void AST::ASTDeclaration::setType(std::unique_ptr<AST::ASTType> &new_type) {
    type = new_type->clone();
}

void AST::Program::setName(std::string new_name) {
    name = new_name;
}

void AST::Program::addDecl(std::unique_ptr<ASTDeclaration> &new_decl) {
    declarations.push_back(std::move(new_decl));
}

void AST::Program::addFunction(std::unique_ptr<Function> &new_func) {
    functions.push_back(std::move(new_func));
}