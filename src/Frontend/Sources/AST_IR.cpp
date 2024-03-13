#include "AST.h"

std::unique_ptr<IR::IRLine> AST::ASTTypeDeclaration::generateIR(ContextForIR &ctx) {

}

std::unique_ptr<IR::IRLine> AST::ASTVarDeclaration::generateIR(ContextForIR &ctx) {

}

std::unique_ptr<IR::IRLine> AST::ASTConstDeclaration::generateIR(ContextForIR &ctx) {
    if (!ctx.topFunc) {
        auto globAlloca = std::make_unique<IR::IRGlobal>();

    }

    return nullptr;
}

std::unique_ptr<IR::IRLine> AST::Function::generateIR(ContextForIR &ctx) {

}

std::unique_ptr<IR::IRLine> AST::Program::generateIR(ContextForIR &ctx) {
    auto program = std::make_unique<IR::IRProgram>();

    for (auto &i: varDeclarations)
        program->addLine(i->generateIR(ctx));

    for (auto &i: functions)
        program->addLine(i->generateIR(ctx));

    return program;
}
