#include "AST.h"

std::shared_ptr<IR::IRType> AST::ASTTypeNull::generateIR() {
    return std::make_shared<IR::IRTypeNull>();
}

std::shared_ptr<IR::IRType> AST::ASTTypeInt::generateIR() {
    return std::make_shared<IR::IRTypeInt>(bits);
}

std::shared_ptr<IR::IRType> AST::ASTTypeFloat::generateIR() {
    return std::make_shared<IR::IRTypeFloat>();
}

std::shared_ptr<IR::IRType> AST::ASTTypeBool::generateIR() {
    return std::make_shared<IR::IRTypeInt>(1);
}

std::shared_ptr<IR::IRType> AST::ASTTypePointer::generateIR() {
    return std::make_shared<IR::IRTypePointer>(type->generateIR());
}

std::shared_ptr<IR::IRType> AST::ASTTypeStruct::generateIR() {
    auto res = std::make_shared<IR::IRTypeStruct>();
    for (auto &i: fileds) {
        res->addFiled(i.second->generateIR());
    }
    return res;
}

std::shared_ptr<IR::IRType> AST::ASTTypeNamed::generateIR() {

}

std::shared_ptr<IR::IRGlobalDecl> AST::ASTTypeDeclaration::generateIRGlobal() const {
    auto res = std::make_shared<IR::IRGlobalTypeDecl>();
    res->addName(name);
    res->addType(type->generateIR());

    return res;
}

std::shared_ptr<IR::IRGlobalDecl> AST::ASTVarDeclaration::generateIRGlobal() const {
    auto res = std::make_shared<IR::IRGlobalVarDecl>();
    res->addName(name);
    res->addType(type->generateIR());
    // TODO value

    return res;
}

std::shared_ptr<IR::IRGlobalDecl> AST::ASTConstDeclaration::generateIRGlobal() const {
    auto res = std::make_shared<IR::IRGlobalVarDecl>();
    res->addName(name);
    res->addType(type->generateIR());
    // TODO value

    return res;
}

std::shared_ptr<IR::IRFunc> AST::Function::generateFunc() {
    auto res = std::make_shared<IR::IRFunc>();
    res->addName(name);
    res->addReturn(return_type->generateIR());
    for (auto &i: params)
        res->addArg(i.second->generateIR());

    return res;
}

std::unique_ptr<IR::IRProgram> AST::Program::generateIR() {
    auto res = std::make_unique<IR::IRProgram>();

    for (auto &i: declarations)
        res->addDecl(i->generateIRGlobal());

    for (auto &i: functions)
        res->addFunc(i->generateFunc());

    return std::move(res);
}

