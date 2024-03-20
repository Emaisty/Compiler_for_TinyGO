#include "AST.h"

void AST::ASTTypeDeclaration::generateIR(ContextForIR &ctx) {

}

void AST::ASTVarDeclaration::generateIR(ContextForIR &ctx) {
    if (!ctx.topFunc) {
        auto globAlloca = std::make_unique<IR::IRGlobal>();

        return;
    }
    auto block = std::make_unique<IR::IRBlock>();


    ctx.topFunc->addDecl(std::move(block));
    return nullptr;
}

void AST::ASTConstDeclaration::generateIR(ContextForIR &ctx) {
    if (!ctx.topFunc) {
        auto globAlloca = std::make_unique<IR::IRGlobal>();

        return globAlloca;
    }
    auto block = std::make_unique<IR::IRBlock>();


    ctx.topFunc->addDecl(std::move(block));
    return nullptr;
}



void AST::ASTIf::generateIR(ContextForIR &ctx) {

}

void AST::ASTFor::generateIR(ContextForIR &ctx) {

}

void AST::ASTAssign::generateIR(ContextForIR &ctx) {
    auto store = std::make_unique<IR::IRStore>();

    return;
}

void AST::Function::generateIR(ContextForIR &ctx) {

}

void AST::Program::generateIR(ContextForIR &ctx) {
    auto program = std::make_unique<IR::IRProgram>();

    for (auto &i: varDeclarations)
        program->addLine(i->generateIR(ctx));

    for (auto &i: functions)
        program->addLine(i->generateIR(ctx));

    return program;
}
