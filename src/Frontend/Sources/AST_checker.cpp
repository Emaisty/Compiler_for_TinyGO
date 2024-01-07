#include "AST.h"

void AST::Context::checkIfNameExist(std::string name) {
    for (auto &i: name_space)
        if (i.name == name)
            throw std::invalid_argument("ERROR. This name already exists");
}

void AST::Context::checkIfTypeExist(std::string name) {
    for (auto &i: type_space)
        if (i.name == name)
            throw std::invalid_argument("ERROR. This type already exists");
}

void AST::Context::goDeeper(bool go_in_loop = false, bool go_in_switch = false) {
    level += 1;
    pr_loop_status.push(in_loop);
    pr_switch_status.push(in_switch);
    in_loop = go_in_loop || in_loop;
    in_switch = go_in_switch || in_switch;
}

void AST::Context::goUp() {
    for (int i = name_space.size() - 1; i >= 0; --i)
        if (name_space[i].level == level)
            name_space.erase(name_space.end() - 1, name_space.end());

    for (int i = type_space.size() - 1; i >= 0; --i)
        if (type_space[i].level == level)
            type_space.erase(type_space.end() - 1, type_space.end());

    level -= 1;
    in_loop = pr_loop_status.top();
    in_switch = pr_switch_status.top();
}

Type *AST::Context::getTypeByTypeName(std::string name) {
    for (auto &i: type_space)
        if (i.name == name)
            return i.type.get();
    return nullptr;
}

Type *AST::Context::getTypeByVarName(std::string name) {
    for (auto &i: name_space)
        if (i.name == name)
            return i.type;
    return nullptr;
}

Type *AST::Context::addType(std::string new_name, std::unique_ptr<Type> &&new_type) {
    if (!new_name.empty()) {
        // add Alias created by user like "type [name] [type]"
        checkIfTypeExist(new_name);
        type_space.emplace_back(new_name, std::move(new_type), level);
        return type_space.back().type.get();
    } else {
        // add type which uses by program but user havent define by himself

        // if it already exists -- lose unique_ptr and return already existed instance
        for (auto &i: type_space) {
            if (i.type->compareSignatures(new_type.get()))
                return i.type.get();
        }

        // if not -- add with unreachable name
        type_space.emplace_back("", std::move(new_type), 0);
        return type_space.back().type.get();
    }
}

void AST::Context::addIntoNameSpace(std::string new_name, Type *new_type) {
    checkIfNameExist(new_name);
    name_space.emplace_back(new_name, new_type, level);
}

Type *AST::Context::getPointer(Type *base_type) {
    auto pointer = pointers.find(base_type);
    if (pointer != pointers.end())
        return pointer->second;

    auto new_pointer = std::make_unique<PointerType>(base_type);
    pointers[base_type] = new_pointer.get();

    addType("", std::move(new_pointer));
    return pointers.find(base_type)->second;
}

bool AST::ASTNode::hasAddress() {
    return false;
}

bool AST::ASTNode::isConst() {
    return false;
}

Type *AST::ASTTypePointer::checker(Context &ctx) {
    return ctx.getPointer(type->checker(ctx));
}

std::set<std::string> AST::ASTTypePointer::getDependencies() {
    return {};
}

Type *AST::ASTTypeStruct::checker(Context &ctx) {

}

std::set<std::string> AST::ASTTypeStruct::getDependencies() {
    std::set<std::string> res;
    for (auto &i: fileds)
        res.merge(i.second->getDependencies());
    return res;
}

Type *AST::ASTTypeNamed::checker(Context &) {

}

std::set<std::string> AST::ASTTypeNamed::getDependencies() {
    if (std::find(AST::Context::base_types.begin(), AST::Context::base_types.end(), name) ==
        AST::Context::base_types.end())
        return {name};
    return {};
}

bool AST::ASTBinaryOperator::isConst() {
    if (left->isConst() && right->isConst())
        return true;
    return false;
}

Type *AST::ASTBinaryOperator::checker(AST::Context &ctx) {

}

std::set<std::string> AST::ASTBinaryOperator::getVarNames() {
    auto res = left->getVarNames();
    res.merge(right->getVarNames());
    return res;
}

bool AST::ASTUnaryOperator::hasAddress() {
    if (op == DEREFER)
        return true;
    return false;
}

bool AST::ASTUnaryOperator::isConst() {
    return value->isConst();
}

Type *AST::ASTUnaryOperator::checker(AST::Context &ctx) {

}

std::set<std::string> AST::ASTUnaryOperator::getVarNames() {
    return value->getVarNames();
}


Type *AST::ASTFunctionCall::checker(AST::Context &ctx) {

}

std::set<std::string> AST::ASTFunctionCall::getVarNames() {
    std::set<std::string> res;
    for (auto &i: arg)
        res.merge(i->getVarNames());
    return res;
}

bool AST::ASTMemberAccess::hasAddress() {

}

bool AST::ASTMemberAccess::isConst() {

}

Type *AST::ASTMemberAccess::checker(AST::Context &ctx) {

}

std::set<std::string> AST::ASTMemberAccess::getVarNames() {
    return name->getVarNames();
}

bool AST::ASTIntNumber::isConst(){
    return true;
}

Type *AST::ASTIntNumber::checker(AST::Context &ctx) {
    return ctx.getTypeByTypeName("int");
}

std::set<std::string> AST::ASTIntNumber::getVarNames() {
    return {};
}

bool AST::ASTFloatNumber::isConst(){
    return true;
}

Type *AST::ASTFloatNumber::checker(AST::Context &ctx) {
    return ctx.getTypeByTypeName("float");
}

std::set<std::string> AST::ASTFloatNumber::getVarNames() {
    return {};
}

bool AST::ASTBoolNumber::isConst(){
    return true;
}


Type *AST::ASTBoolNumber::checker(AST::Context &ctx) {
    return ctx.getTypeByTypeName("bool");
}

std::set<std::string> AST::ASTBoolNumber::getVarNames() {
    return {};
}

bool AST::ASTStruct::isConst() {
    return true;
}

Type *AST::ASTStruct::checker(AST::Context &ctx) {

}

std::set<std::string> AST::ASTStruct::getVarNames() {
    std::set<std::string> res;
    for (auto &i: values)
        res.merge(i.second->getVarNames());
    return res;
}

bool AST::ASTVar::hasAddress() {
    return true;
}

bool AST::ASTVar::isConst() {

}

Type *AST::ASTVar::checker(AST::Context &ctx) {
    ctx.checkIfNameExist(name);
    return ctx.getTypeByVarName(name);
}

std::set<std::string> AST::ASTVar::getVarNames() {
    return {name};
}

std::vector<std::pair<std::string, std::set<std::string>>> AST::ASTTypeDeclaration::globalPreInit() {
    return {std::make_pair(name[0], type->getDependencies())};
}

void AST::ASTTypeDeclaration::globChecker(Context &ctx) {

}

Type *AST::ASTTypeDeclaration::checker(Context &ctx) {
//    if (ctx.level != 0) {
//        ctx.checkIfTypeExist(name[0]);
//        ctx.addType(name[0], type->checker(ctx)->makeUnique());
//    }
//    return nullptr;
//    TODO
}

std::vector<std::pair<std::string, std::set<std::string>>> AST::ASTVarDeclaration::globalPreInit() {
    std::vector<std::pair<std::string, std::set<std::string>>> res(name.size());

    for (int i = 0; i < name.size(); ++i)
        res[i] = std::make_pair(name[i], value[i]->getVarNames());

    return res;
}

Type *AST::ASTVarDeclaration::checker(Context &ctx) {

    return nullptr;
}

std::vector<std::pair<std::string, std::set<std::string>>> AST::ASTConstDeclaration::globalPreInit() {
    std::vector<std::pair<std::string, std::set<std::string>>> res(name.size());

    for (int i = 0; i < name.size(); ++i)
        res[i] = std::make_pair(name[i], value[i]->getVarNames());

    return res;
}

Type *AST::ASTConstDeclaration::checker(Context &ctx) {
    for (int i = 0; i < name.size(); ++i) {
        ctx.checkIfNameExist(name[i]);
        if (!value[i]->isConst())
            throw std::invalid_argument("ERROR. Not constable expression is tried to passed to const var.");

    }
    return nullptr;
}

Type *AST::ASTBlock::checker(Context &ctx) {
    ctx.goDeeper();
    for (auto &i: statements)
        i->checker(ctx);
    ctx.goUp();
    return nullptr;
}

Type *AST::ASTBreak::checker(Context &ctx) {
    if (ctx.in_switch || ctx.in_loop)
        return nullptr;
    throw std::invalid_argument("ERROR. Called Break statement not in switch case or not in loop");
}

Type *AST::ASTContinue::checker(Context &ctx) {
    if (ctx.in_loop)
        return nullptr;
    throw std::invalid_argument("ERROR. Called Continue statement not in loop");
}

Type *AST::ASTReturn::checker(Context &ctx) {
    if (ctx.return_type.size() != return_value.size())
        throw std::invalid_argument("ERROR. Mismatch number of returned values.");

    for (int i = 0; i < return_value.size(); ++i)
        if (!return_value[i]->checker(ctx)->compare(ctx.return_type[i]))
            throw std::invalid_argument("ERROR. Not matching return value");

    return nullptr;
}

Type *AST::ASTSwitch::checker(Context &ctx) {
    auto type_of_stat = expr->checker(ctx);
    for (auto &i: cases) {
        ctx.goDeeper(false, true);
        if (!type_of_stat->compare(i.first->checker(ctx)))
            throw std::invalid_argument("ERROR. Not comparable types between case and expression.");
        i.second->checker(ctx);
        ctx.goUp();
    }

    return nullptr;
}

Type *AST::ASTIf::checker(Context &ctx) {
    auto type_of_clause = if_clause->checker(ctx);
    if (!type_of_clause->compare(ctx.getTypeByTypeName("bool")))
        throw std::invalid_argument("ERROR. If clause cannot be converted to bool");

    if_clause->checker(ctx);
    if (else_clause)
        else_clause->checker(ctx);

    return nullptr;
}

Type *AST::ASTFor::checker(Context &ctx) {
    ctx.goDeeper(true);
    for (auto &i: init_clause)
        i->checker(ctx);

    auto type_of_clause = if_clause->checker(ctx);
    if (!type_of_clause->compare(ctx.getTypeByTypeName("bool")))
        throw std::invalid_argument("ERROR. If clause cannot be converted to bool");

    for (auto &i: iterate_clause)
        i->checker(ctx);

    body->checker(ctx);

    ctx.goUp();
    return nullptr;
}


Type *AST::ASTAssign::checker(Context &ctx) {

    for (int i = 0; i < variable.size(); ++i) {
        auto var_type = variable[i]->checker(ctx);
        if (!variable[i]->hasAddress())
            throw std::invalid_argument("ERROR. Var is not assignable type.");
        auto val_type = value[i]->checker(ctx);
        if (!var_type || !val_type || !var_type->compare(val_type))
            throw std::invalid_argument("ERROR. Var and assigned value not the same types");
    }

    return nullptr;
}

Type *AST::Function::checker(Context &ctx) {
    ctx.goDeeper();
    for (auto &i: params)
        i->checker(ctx);

    for (auto &i: return_type)
        ctx.return_type.emplace_back(i->checker(ctx));

    body->checker(ctx);

    ctx.goUp();

    return nullptr;
}


Type *AST::Program::checker(Context &ctx) {
    std::vector<std::pair<std::string, std::set<std::string>>> declarations;
    for (auto &i: typeDeclarations)
        for (auto &j: i->globalPreInit())
            declarations.push_back(j);

    auto q = topSort(declarations);

    // TODO




//    for (auto &i: declarations)
//        i->checker(ctx);

    for (auto &i: functions)
        i->checker(ctx);

    return nullptr;
}

std::queue<std::string> AST::Program::topSort(std::vector<std::pair<std::string, std::set<std::string>>> &g) {
    std::queue<std::string> res;
    while (!g.empty()) {
        auto pr_size = g.size();
        for (auto i = g.begin(); i != g.end(); i++)
            if (i->second.empty()) {
                res.push(i->first);
                for (auto &j: g)
                    j.second.erase(i->first);
                g.erase(i);
                break;
            }

        if (g.size() == pr_size)
            throw std::invalid_argument("ERROR. Loop declaration.");
    }


    return res;
}