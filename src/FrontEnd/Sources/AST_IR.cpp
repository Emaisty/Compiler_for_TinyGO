#include "AST.h"

IR::Context AST::Context::createIRContext() {
    IR::Context ctx;

    return ctx;
}

IR::Value *AST::ASTType::generateIR(IR::Context &ctx) {
    throw std::invalid_argument("Never should happen.");
}

IR::Value *AST::ASTBinaryOperator::generateIR(IR::Context &ctx) {
    auto left_pointer = left->generateIR(ctx);
    auto right_pointer = right->generateIR(ctx);
    auto res = std::make_unique<IR::IRArithOp>(ctx.counter);

    res->setTypeOfOperation(op);
    res->addChildren(left_pointer, right_pointer);
    res->setTypeOfResult(typeOfNode);

    return ctx.buildInstruction(std::move(res));
}

IR::Value *AST::ASTUnaryOperator::generateIR(IR::Context &ctx) {
    switch (op) {
        case NOT: {
            auto one = std::make_unique<IR::IntConst>(ctx.counter);
            one->addValue(1);
            auto one_pointer = ctx.buildInstruction(std::move(one));
            auto value_pointer = value->generateIR(ctx);
            auto res = std::make_unique<IR::IRArithOp>(ctx.counter);

            res->setTypeOfOperation(IR::IRArithOp::XOR);
            res->addChildren(one_pointer, value_pointer);
            res->setTypeOfResult(value->typeOfNode);

            return ctx.buildInstruction(std::move(res));
        }
        case PLUS:
            return value->generateIR(ctx);

        case MINUS: {
            IR::Value *zero;
            if (dynamic_cast<FloatType *>(value->typeOfNode))
                zero = ctx.buildInstruction(std::make_unique<IR::DoubleConst>(ctx.counter));
            else
                zero = ctx.buildInstruction(std::make_unique<IR::IntConst>(ctx.counter));
            auto value_pointer = value->generateIR(ctx);
            auto res = std::make_unique<IR::IRArithOp>(ctx.counter);

            res->setTypeOfOperation(IR::IRArithOp::MINUS);
            res->addChildren(zero, value_pointer);
            res->setTypeOfResult(value->typeOfNode);

            return ctx.buildInstruction(std::move(res));
        }
        case PREINC:
        case PREDEC:
        case POSTINC:
        case POSTDEC: {
            auto value_pointer = value->generateIR(ctx);
            IR::Value *one_pointer;
            if (dynamic_cast<FloatType *>(value->typeOfNode)) {
                auto one = std::make_unique<IR::DoubleConst>(ctx.counter);
                one->addValue(1);
                one_pointer = ctx.buildInstruction(std::move(one));
            } else {
                auto one = std::make_unique<IR::IntConst>(ctx.counter);
                one->addValue(1);
                one_pointer = ctx.buildInstruction(std::move(one));
            }

            auto res = std::make_unique<IR::IRArithOp>(ctx.counter);
            if (op == PREINC || op == POSTINC)
                res->setTypeOfOperation(IR::IRArithOp::PLUS);
            else
                res->setTypeOfOperation(IR::IRArithOp::MINUS);
            res->addChildren(value_pointer, one_pointer);
            res->setTypeOfResult(value->typeOfNode);

            auto val = ctx.buildInstruction(std::move(res));

            auto store = std::make_unique<IR::IRStore>(ctx.counter);

            // TODO Is it true??
            auto variable = dynamic_cast<IR::IRLoad *>(value_pointer);

            store->addStoreWhat(val);
            store->addStoreWhere(variable->getPointer());

            ctx.buildInstruction(std::move(store));
            if (op == PREINC || op == PREDEC)
                return val;
            else
                return value_pointer;
        }
        case REFER: {

        }
        case DEREFER: {

        }
    }
}

IR::Value *AST::ASTFunctionCall::generateIR(IR::Context &ctx) {
    std::vector<IR::Value *> arguments;
    for (auto &i: arg)
        arguments.emplace_back(i->generateIR(ctx));

    auto func_pointer = dynamic_cast<IR::IRCall *>(name->generateIR(ctx));
    for (auto &i: arguments)
        func_pointer->addArg(i);
    return func_pointer;
}

IR::Value *AST::ASTMemberAccess::generateIR(IR::Context &ctx) {
    if (auto func_type = dynamic_cast<FunctionType*>(typeOfNode)){
        IR::Value* method;
        if (func_type->isPointer())
            ctx.l_value = true;
        method = name->generateIR(ctx);

        auto res = std::make_unique<IR::IRCall>(ctx.counter);
        res->addArg(method);
        res->addFunctionName("_"+func_type->innerName()+"_"+member);
        return ctx.buildInstruction(std::move(res));
    }


    bool flag = ctx.l_value;
    auto pointer_to_member = std::make_unique<IR::IRMembCall>(ctx.counter);
    ctx.l_value = true;
    pointer_to_member->addCallWhere(name->generateIR(ctx));
    if (auto struc = dynamic_cast<StructType*>(name->typeOfNode))
        pointer_to_member->addCallWhat(struc->getFieldOrder(member));
    else
        pointer_to_member->addCallWhat(dynamic_cast<StructType*>(dynamic_cast<PointerType*>(name->typeOfNode)->getBase())->getFieldOrder(member));


    if (flag)
        return ctx.buildInstruction(std::move(pointer_to_member));

    auto link_to_member = ctx.buildInstruction(std::move(pointer_to_member));

    auto load = std::make_unique<IR::IRLoad>(ctx.counter);
    load->addLoadFrom(link_to_member);
    return ctx.buildInstruction(std::move(load));
}

IR::Value *AST::ASTIntNumber::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IntConst>(ctx.counter);
    res->addValue(value);

    return ctx.buildInstruction(std::move(res));
}

IR::Value *AST::ASTFloatNumber::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::DoubleConst>(ctx.counter);
    res->addValue(value);

    return ctx.buildInstruction(std::move(res));
}

IR::Value *AST::ASTBoolNumber::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IntConst>(ctx.counter);
    res->addValue(value ? 1 : 0);

    return ctx.buildInstruction(std::move(res));
}

IR::Value *AST::ASTStruct::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::StructConst>(ctx.counter);
    std::map<std::string, IR::Value*> predefined_fields;
    for (auto &i : values)
        predefined_fields[i.first] = i.second->generateIR(ctx);

    for(auto &i : dynamic_cast<StructType*>(typeOfNode)->getFields())
        if (predefined_fields.find(i.first) == predefined_fields.end())
            res->addConst(ctx.getBasicValue(i.second));
        else
            res->addValue(predefined_fields[i.first]);

    return ctx.buildInstruction(std::move(res));
}

IR::Value *AST::ASTVar::generateIR(IR::Context &ctx) {

    // Variable
    if (auto link = ctx.getVariable(name)) {
        if (ctx.l_value) {
            ctx.l_value = false;
            return link;
        }
        auto res = std::make_unique<IR::IRLoad>(ctx.counter);
        res->addLoadFrom(link);

        return ctx.buildInstruction(std::move(res));
    }

    // Function
    auto res = std::make_unique<IR::IRCall>(ctx.counter);
//    res->addLinkToFunc(ctx.getFunction(name));
    res->addFunctionName(name);
    return ctx.buildInstruction(std::move(res));
}

IR::Value *AST::ASTTypeDeclaration::generateIR(IR::Context &ctx) {
    return nullptr;
}

IR::Value *AST::ASTVarDeclaration::generateIR(IR::Context &ctx) {
    if (ctx.Global) {
        for (auto i = 0; i < name.size(); ++i) {
            auto res = std::make_unique<IR::IRGlobal>(ctx.counter);

            if (type)
                res->addType(type->typeOfNode);
            else
                res->addType(value[i]->typeOfNode);

            ctx.addVariable(name[i], res.get());
            // TODO basic value
            ctx.program->addGlobDecl(std::move(res));
        }
        return nullptr;
    }

    if (function_dispatch)
        function_dispatch->generateIR(ctx);

    for (auto i = 0; i < name.size(); ++i) {
        auto res = std::make_unique<IR::IRAlloca>(ctx.counter);


        Type *type_of_alloca;
        if (type)
            type_of_alloca = type->typeOfNode;
        else
            type_of_alloca = value[i]->typeOfNode;

        res->addType(type_of_alloca);
        if (value.empty())
            res->addBasicValue(ctx.getBasicValue(type_of_alloca));

        ctx.addVariable(name[i], res.get());
        ctx.buildInstruction(std::move(res));
        if (!value.empty() && value[i]) {
            auto val_pointer = value[i]->generateIR(ctx);
            auto store = std::make_unique<IR::IRStore>(ctx.counter);
            store->addStoreWhat(val_pointer);
            store->addStoreWhere(ctx.getVariable(name[i]));
            ctx.buildInstruction(std::move(store));
        }
    }
    return nullptr;
}

IR::Value *AST::ASTConstDeclaration::generateIR(IR::Context &ctx) {
    if (ctx.Global) {
        // TODO value later
        for (auto i = 0; i < name.size(); ++i) {
            auto res = std::make_unique<IR::IRGlobal>(ctx.counter);

            if (type)
                res->addType(type->typeOfNode);
            else
                res->addType(value[i]->typeOfNode);

            ctx.addVariable(name[i], res.get());
            ctx.program->addGlobDecl(std::move(res));
        }
        return nullptr;
    }
    for (auto i = 0; i < name.size(); ++i) {
        auto res = std::make_unique<IR::IRAlloca>(ctx.counter);

        if (type)
            res->addType(type->typeOfNode);
        else
            res->addType(value[i]->typeOfNode);


        ctx.addVariable(name[i], res.get());
        ctx.buildInstruction(std::move(res));
    }
    return nullptr;
}

IR::Value *AST::ASTBlock::generateIR(IR::Context &ctx) {
    ctx.goDeeper();
    for (auto &i: statements)
        i->generateIR(ctx);
    ctx.goUp();

    return nullptr;
}

IR::Value *AST::ASTBreak::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRBranch>(ctx.counter);

    res->addBrTaken(ctx.getBreakLabel());

    ctx.buildInstruction(std::move(res));
    return nullptr;
}

IR::Value *AST::ASTContinue::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRBranch>(ctx.counter);

    res->addBrTaken(ctx.getContinueLabel());

    ctx.buildInstruction(std::move(res));
    return nullptr;
}

IR::Value *AST::ASTReturn::generateIR(IR::Context &ctx) {
    auto res = std::make_unique<IR::IRRet>(ctx.counter);


    if (!return_value.empty()) {
        if (return_value.size() == 1)
            res->addRetVal(return_value[0]->generateIR(ctx));
        else{
            std::vector<IR::Value*> values;
            for (auto &i : return_value)
                values.emplace_back(i->generateIR(ctx));

            auto structure = std::make_unique<IR::StructConst>(ctx.counter);

            for (auto &i : values)
                structure->addValue(i);

            res->addRetVal(ctx.buildInstruction(std::move(structure)));
        }
    }

    ctx.buildInstruction(std::move(res));
    return nullptr;
}

IR::Value *AST::ASTSwitch::generateIR(IR::Context &ctx) {

}

IR::Value *AST::ASTIf::generateIR(IR::Context &ctx) {
    auto condition_pointer = expr->generateIR(ctx);

    auto ifTrue_label = std::make_unique<IR::IRLabel>(ctx.counter);
    auto ifFalse_label = std::make_unique<IR::IRLabel>(ctx.counter);
    auto end_label = std::make_unique<IR::IRLabel>(ctx.counter);

    auto condition_jmp = std::make_unique<IR::IRBranch>(ctx.counter);
    condition_jmp->addCond(condition_pointer);
    condition_jmp->addBrTaken(ifTrue_label.get());
    if (else_clause)
        condition_jmp->addBrNTaken(ifFalse_label.get());
    else
        condition_jmp->addBrNTaken(end_label.get());


    ctx.buildInstruction(std::move(condition_jmp));
    ctx.buildInstruction(std::move(ifTrue_label));

    ctx.goDeeper();
    if_clause->generateIR(ctx);
    ctx.goUp();

    auto jump_from_if = std::make_unique<IR::IRBranch>(ctx.counter);
    jump_from_if->addBrTaken(end_label.get());
    ctx.buildInstruction(std::move(jump_from_if));

    if (else_clause) {
        ctx.buildInstruction(std::move(ifFalse_label));

        ctx.goDeeper();
        else_clause->generateIR(ctx);
        ctx.goUp();

        auto jump_from_else = std::make_unique<IR::IRBranch>(ctx.counter);
        jump_from_else->addBrTaken(end_label.get());
        ctx.buildInstruction(std::move(jump_from_else));
    }
    ctx.buildInstruction(std::move(end_label));

    return nullptr;
}

IR::Value *AST::ASTFor::generateIR(IR::Context &ctx) {
    ctx.goDeeper();
    for (auto &i: init_clause)
        i->generateIR(ctx);

    auto begin_loop_label = std::make_unique<IR::IRLabel>(ctx.counter);
    auto jmp_to_begin = std::make_unique<IR::IRBranch>(ctx.counter);
    jmp_to_begin->addBrTaken(begin_loop_label.get());
    ctx.buildInstruction(std::move(jmp_to_begin));

    auto begin_loop_label_pointer = ctx.buildInstruction(std::move(begin_loop_label));

    auto val = if_clause->generateIR(ctx);

    auto jmp_condition = std::make_unique<IR::IRBranch>(ctx.counter);

    auto begin_loop_body_label = std::make_unique<IR::IRLabel>(ctx.counter);
    auto end_loop_label = std::make_unique<IR::IRLabel>(ctx.counter);
    auto begin_loop_post_label = std::make_unique<IR::IRLabel>(ctx.counter);

    jmp_condition->addCond(val);
    jmp_condition->addBrTaken(begin_loop_body_label.get());
    jmp_condition->addBrNTaken(end_loop_label.get());

    ctx.buildInstruction(std::move(jmp_condition));

    ctx.buildInstruction(std::move(begin_loop_body_label));
    ctx.addBreakLabel(end_loop_label.get());
    ctx.addContinueLabel(begin_loop_post_label.get());

    body->generateIR(ctx);

    auto jmp_to_post = std::make_unique<IR::IRBranch>(ctx.counter);
    jmp_to_post->addBrTaken(begin_loop_post_label.get());

    ctx.buildInstruction(std::move(jmp_to_post));

    ctx.buildInstruction(std::move(begin_loop_post_label));

    for (auto &i: iterate_clause)
        i->generateIR(ctx);

    auto jmp_to_condition = std::make_unique<IR::IRBranch>(ctx.counter);
    jmp_to_condition->addBrTaken(begin_loop_label_pointer);

    ctx.buildInstruction(std::move(jmp_to_condition));

    ctx.buildInstruction(std::move(end_loop_label));

    return nullptr;
}

IR::Value *AST::ASTAssign::generateIR(IR::Context &ctx) {
    if (function_dispatch)
        function_dispatch->generateIR(ctx);


    for (auto i = 0; i < value.size(); ++i) {
        switch (type) {
            case ASSIGN: {
                auto value_pointer = value[i]->generateIR(ctx);
                auto res = std::make_unique<IR::IRStore>(ctx.counter);
                res->addStoreWhat(value_pointer);
                //TODO prove it. Maybe it does not work???
                //Also check in case of function
                ctx.l_value = true;
                auto loader = variable[i]->generateIR(ctx);
                res->addStoreWhere(loader);
                ctx.buildInstruction(std::move(res));
                break;
            }
            case PLUSASSIGN:
            case MINUSASSIGN:
            case MULTASSIGN:
            case DIVASSIGN:
            case MODASSIGN: {
                auto val_of_var = variable[i]->generateIR(ctx);
                auto var_pointer = dynamic_cast<IR::IRLoad *>(val_of_var)->getPointer();
                auto val = value[i]->generateIR(ctx);
                auto result = std::make_unique<IR::IRArithOp>(ctx.counter);

                if (type == PLUSASSIGN)
                    result->setTypeOfOperation(IR::IRArithOp::PLUS);
                if (type == MINUSASSIGN)
                    result->setTypeOfOperation(IR::IRArithOp::MINUS);
                if (type == MULTASSIGN)
                    result->setTypeOfOperation(IR::IRArithOp::MUL);
                if (type == DIVASSIGN)
                    result->setTypeOfOperation(IR::IRArithOp::DIV);
                if (type == MODASSIGN)
                    result->setTypeOfOperation(IR::IRArithOp::MOD);
                result->addChildren(val_of_var, val);
                result->setTypeOfResult(variable[i]->typeOfNode);

                auto new_val = ctx.buildInstruction(std::move(result));

                auto store = std::make_unique<IR::IRStore>(ctx.counter);
                store->addStoreWhere(var_pointer);
                store->addStoreWhat(new_val);

                ctx.buildInstruction(std::move(store));
                break;
            }


        }

    }
    return nullptr;
}


IR::Value *AST::Function::generateIR(IR::Context &ctx) {

    auto res = std::make_unique<IR::IRFunc>(ctx.counter);
    ctx.addFunction(name,res.get());

    if (!return_type.empty())
        res->addReturnType(typeOfNode);

    if (type_of_method)
        res->setName("_" + *type_of_method->getDependencies().begin()+"_"+name);
    else
        res->setName(name);

    ctx.goDeeper();

    ctx.setFunction(res.get());

    // arguments
    int place_of_arg = 0;

    if (type_of_method){
        auto argument = std::make_unique<IR::IRFuncArg>(ctx.counter);
        argument->addType(type_of_method->typeOfNode);
        argument->addOrder(place_of_arg++);
        auto alloca = std::make_unique<IR::IRAlloca>(ctx.counter);
        alloca->addType(type_of_method->typeOfNode);
        auto store = std::make_unique<IR::IRStore>(ctx.counter);
        store->addStoreWhat(argument.get());
        store->addStoreWhere(alloca.get());

        ctx.addVariable(inner_name, alloca.get());

        ctx.buildInstruction(std::move(alloca));
        ctx.buildInstruction(std::move(store));

        res->addArg(std::move(argument));
    }


    for (auto &[i, j]: params)
        for (auto &var_name: i) {
            auto argument = std::make_unique<IR::IRFuncArg>(ctx.counter);
            argument->addType(j->typeOfNode);
            argument->addOrder(place_of_arg++);
            auto alloca = std::make_unique<IR::IRAlloca>(ctx.counter);
            alloca->addType(j->typeOfNode);
            auto store = std::make_unique<IR::IRStore>(ctx.counter);
            store->addStoreWhat(argument.get());
            store->addStoreWhere(alloca.get());

            ctx.addVariable(var_name, alloca.get());

            ctx.buildInstruction(std::move(alloca));
            ctx.buildInstruction(std::move(store));

            res->addArg(std::move(argument));
        }


    body->generateIR(ctx);
    if (return_type.empty())
        ctx.buildInstruction(std::make_unique<IR::IRRet>(ctx.counter));

    ctx.program->addFunc(std::move(res));

    ctx.goUp();

    return nullptr;
}

IR::Value *AST::Program::generateIR(IR::Context &ctx) {
    ctx.program = std::make_unique<IR::IRProgram>(ctx.counter);

    for (auto &i: varDeclarations)
        i->generateIR(ctx);

    ctx.Global = false;

    for (auto &i: functions)
        i->generateIR(ctx);

    return ctx.program.get();
}

IR::Value *AST::ASTCast::generateIR(IR::Context &ctx) {
    auto value_pointer = expr->generateIR(ctx);
    auto cast_node = std::make_unique<IR::IRCast>(ctx.counter);
    cast_node->addTypeToFrom(cast_to, expr->typeOfNode);

    cast_node->addExpr(value_pointer);

    auto res = cast_node.get();
    ctx.buildInstruction(std::move(cast_node));
    return res;
}
