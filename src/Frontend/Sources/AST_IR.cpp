#include "AST.h"

std::unique_ptr<IR::IRLine> AST::ASTBinaryOperator::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRArithOp>();
    res->addChildren(left->generateIR(ctx), right->generateIR(ctx));

    return res;
}

IR::IRLine *AST::ASTBinaryOperator::getPointerToIt(IR::Context &ctx) {
    throw std::invalid_argument("Never should happen.");
}

std::unique_ptr<IR::IRLine> AST::ASTUnaryOperator::generateIR(IR::Context &ctx) {

}

IR::IRLine *AST::ASTUnaryOperator::getPointerToIt(IR::Context &ctx) {

}

std::unique_ptr<IR::IRLine> AST::ASTMemberAccess::generateIR(IR::Context &ctx) {

}

IR::IRLine *AST::ASTMemberAccess::getPointerToIt(IR::Context &ctx) {

}

std::unique_ptr<IR::IRLine> AST::ASTIntNumber::generateIR(IR::Context &ctx) {

}

IR::IRLine *AST::ASTIntNumber::getPointerToIt(IR::Context &ctx) {
    throw std::invalid_argument("Never should happen.");
}

std::unique_ptr<IR::IRLine> AST::ASTFloatNumber::generateIR(IR::Context &ctx) {

}

IR::IRLine *AST::ASTFloatNumber::getPointerToIt(IR::Context &ctx) {
    throw std::invalid_argument("Never should happen.");
}

std::unique_ptr<IR::IRLine> AST::ASTBoolNumber::generateIR(IR::Context &ctx) {

}

IR::IRLine *AST::ASTBoolNumber::getPointerToIt(IR::Context &ctx) {
    throw std::invalid_argument("Never should happen.");
}

std::unique_ptr<IR::IRLine> AST::ASTStruct::generateIR(IR::Context &ctx) {

}

IR::IRLine *AST::ASTStruct::getPointerToIt(IR::Context &ctx) {
    throw std::invalid_argument("Never should happen.");
}

std::unique_ptr<IR::IRLine> AST::ASTVar::generateIR(IR::Context &ctx) {

}

IR::IRLine *AST::ASTVar::getPointerToIt(IR::Context &ctx) {

}

std::unique_ptr<IR::IRLine> AST::ASTTypeDeclaration::generateIR(IR::Context &ctx) {
    return nullptr;
}

std::unique_ptr<IR::IRLine> AST::ASTVarDeclaration::generateIR(IR::Context &ctx) {
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
            auto fill_value = std::make_unique<IR::IRStore>();
            Type *type_of_alloca = type ? type->typeOfNode : value[i]->typeOfNode;

            declare->addType(type_of_alloca);

            if (value[i]) {
                fill_value->addValue(value[i]->generateIR(ctx));
            }
            // TODO fill with default

            fill_value->setLink(declare.get());

            tmp_block->addLine(std::move(declare));
            tmp_block->addLine(std::move(fill_value));

            res->addLine(std::move(tmp_block));
        }
    }

    return res;
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
            auto fill_value = std::make_unique<IR::IRStore>();
            Type *type_of_alloca = type ? type->typeOfNode : value[i]->typeOfNode;

            declare->addType(type_of_alloca);

            if (value[i]) {
                fill_value->addValue(value[i]->generateIR(ctx));
            }
            // TODO fill with default

            fill_value->setLink(declare.get());

            tmp_block->addLine(std::move(declare));
            tmp_block->addLine(std::move(fill_value));

            res->addLine(std::move(tmp_block));
        }
    }

    return res;
}

std::unique_ptr<IR::IRLine> AST::ASTBlock::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRBlock>();
    for (auto &i: statements)
        res->addLine(i->generateIR(ctx));
    return res;
}

std::unique_ptr<IR::IRLine> AST::ASTBreak::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRBranch>();
    res->addBrTaken(ctx.break_label);

    return res;
}

std::unique_ptr<IR::IRLine> AST::ASTContinue::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRBranch>();
    res->addBrTaken(ctx.cont_label);

    return res;
}

std::unique_ptr<IR::IRLine> AST::ASTReturn::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRRet>();
    res->addRetVal(this->return_value[0]->generateIR(ctx));

    return res;
}

std::unique_ptr<IR::IRLine> AST::ASTSwitch::generateIR(IR::Context &ctx) {
    // TODO
}

std::unique_ptr<IR::IRLine> AST::ASTIf::generateIR(IR::Context &ctx) {
    if (else_clause) {
        auto res = std::make_unique<IR::IRBlock>();
        auto if_label = std::make_unique<IR::IRLabel>();
        auto else_label = std::make_unique<IR::IRLabel>();
        auto end_label = std::make_unique<IR::IRLabel>();

        auto br = std::make_unique<IR::IRBranch>();
        br->addCond(expr->generateIR(ctx));
        br->addBrTaken(if_label.get());
        br->addBrNTaken(else_label.get());

        auto brEndIf = std::make_unique<IR::IRBranch>();
        brEndIf->addBrTaken(end_label.get());

        res->addLine(std::move(br));
        res->addLine(std::move(if_label));
        res->addLine(std::move(if_clause->generateIR(ctx)));
        res->addLine(std::move(brEndIf));
        res->addLine(std::move(else_label));
        res->addLine(std::move(else_clause->generateIR(ctx)));
        res->addLine(std::move(end_label));
        return res;
    }
    auto res = std::make_unique<IR::IRBlock>();
    auto if_label = std::make_unique<IR::IRLabel>();
    auto end_label = std::make_unique<IR::IRLabel>();

    auto br = std::make_unique<IR::IRBranch>();
    br->addCond(expr->generateIR(ctx));
    br->addBrTaken(if_label.get());
    br->addBrNTaken(end_label.get());

    res->addLine(std::move(br));
    res->addLine(std::move(if_label));
    res->addLine(std::move(if_clause->generateIR(ctx)));
    res->addLine(std::move(end_label));
    return res;
}

std::unique_ptr<IR::IRLine> AST::ASTFor::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRBlock>();

    auto init_stage = std::make_unique<IR::IRBlock>();
    for (auto &i: init_clause)
        init_stage->addLine(i->generateIR(ctx));


    auto start_for_label = std::make_unique<IR::IRLabel>();
    auto body_for_label = std::make_unique<IR::IRLabel>();
    auto iter_clause_label = std::make_unique<IR::IRLabel>();
    auto end_for_label = std::make_unique<IR::IRLabel>();

    ctx.break_label = end_for_label.get();
    ctx.cont_label = iter_clause_label.get();

    auto cond_stage = std::make_unique<IR::IRBranch>();
    cond_stage->addCond(if_clause->generateIR(ctx));
    cond_stage->addBrTaken(body_for_label.get());
    cond_stage->addBrNTaken(end_for_label.get());

    auto body_clause = body->generateIR(ctx);

    auto iter_clause = std::make_unique<IR::IRBlock>();
    for (auto &i: iterate_clause)
        iter_clause->addLine(i->generateIR(ctx));
    auto tmp_branch = std::make_unique<IR::IRBranch>();
    tmp_branch->addBrTaken(start_for_label.get());
    iter_clause->addLine(std::move(tmp_branch));

    res->addLine(std::move(init_stage));
    res->addLine(std::move(start_for_label));
    res->addLine(std::move(cond_stage));
    res->addLine(std::move(body_for_label));
    res->addLine(std::move(body_clause));
    res->addLine(std::move(iter_clause_label));
    res->addLine(std::move(iter_clause));
    res->addLine(std::move(end_for_label));

    return res;
}

std::unique_ptr<IR::IRLine> AST::ASTAssign::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRBlock>();
    for (auto i = 0; i < value.size(); ++i) {
        auto val = value[i]->generateIR(ctx);

        auto store = std::make_unique<IR::IRStore>();
        store->addValue(std::move(val));
        // TODO where to store

        res->addLine(std::move(store));
    }

    return res;
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