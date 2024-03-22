#include "AST.h"

std::unique_ptr<IR::IRLine> AST::ASTTypeDeclaration::generateIR(IR::Context &ctx) {
    return nullptr;
}

std::unique_ptr<IR::IRLine> AST::ASTVarDeclaration::generateIR(IR::Context &ctx) {

}

std::unique_ptr<IR::IRLine> AST::ASTConstDeclaration::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRBlock>();

    for (auto i = 0; i < name.size(); ++i) {
        if (ctx.Global) {
            auto single_decl = std::make_unique<IR::IRGlobal>();
            if (type)
                single_decl->addType(type->typeOfNode);
            else
                single_decl->addType(value[i]->typeOfNode);
            // TODO value

            res->addLine(std::move(single_decl));
        } else {
            auto tmp_block = std::make_unique<IR::IRBlock>();
            auto declare = std::make_unique<IR::IRAlloca>();
            auto fill_default = std::make_unique<IR::IRStore>();
            if (type) {
                declare->addType(type->typeOfNode);

            } else {

            }
        }
    }

    return res;
}

std::unique_ptr<IR::IRLine> AST::ASTAssign::generateIR(IR::Context&){

}


std::unique_ptr<IR::IRLine> AST::Function::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRFunc>();
    res->addReturnType(return_type[0]->typeOfNode);

    for (auto &i: params)
        res->addTypeOfArg(i.second->typeOfNode);

    res->addBody(body->generateIR(ctx));

    return res;
}

std::unique_ptr<IR::IRLine> AST::Program::generateIR(IR::Context &ctx) {
    auto program = std::make_unique<IR::IRProgram>();

    for (auto &i: varDeclarations)
        program->addLine(i->generateIR(ctx));


    for (auto &i: functions)
        program->addLine(i->generateIR(ctx));


    return program;
}