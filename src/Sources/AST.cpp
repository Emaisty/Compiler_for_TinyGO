#include "AST.h"

AST::ASTTypeInt::ASTTypeInt(int new_bits) : bits(new_bits) {}

std::unique_ptr<AST::ASTType> AST::ASTTypeInt::clone() const {

    return std::make_unique<ASTTypeInt>(*this);
}


std::unique_ptr<AST::ASTType> AST::ASTTypeFloat::clone() const {
    return std::make_unique<ASTTypeFloat>(*this);
}


std::unique_ptr<AST::ASTType> AST::ASTTypeBool::clone() const {
    return std::make_unique<ASTTypeBool>(*this);
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


std::unique_ptr<AST::Statement> AST::ASTExpression::clone() const {
    return this->cloneExpr();
}

AST::ASTBinaryOperator::ASTBinaryOperator(const ASTBinaryOperator &old_expr) {
    op = old_expr.op;
    left = old_expr.left->cloneExpr();
    right = old_expr.right->cloneExpr();

}

std::unique_ptr<AST::ASTExpression> AST::ASTBinaryOperator::cloneExpr() const {
    return std::make_unique<ASTBinaryOperator>(*this);
}

std::unique_ptr<AST::ASTType> AST::ASTBinaryOperator::getType() const {
    //TODO
}

AST::ASTBinaryOperator::ASTBinaryOperator(std::unique_ptr<AST::ASTExpression> &new_left,
                                          std::unique_ptr<AST::ASTExpression> &new_right, Operator new_op) {
    op = new_op;
    left = new_left->cloneExpr();
    right = new_right->cloneExpr();
}


AST::ASTUnaryOperator::ASTUnaryOperator(const AST::ASTUnaryOperator &old_expr) {
    op = old_expr.op;
    value = old_expr.value->cloneExpr();
}

std::unique_ptr<AST::ASTExpression> AST::ASTUnaryOperator::cloneExpr() const {
    return std::make_unique<ASTUnaryOperator>(*this);
}

std::unique_ptr<AST::ASTType> AST::ASTUnaryOperator::getType() const {
    if (op == AST::ASTUnaryOperator::NOT)
        return std::make_unique<AST::ASTTypeBool>();
    return std::make_unique<AST::ASTTypeInt>();
}

AST::ASTUnaryOperator::ASTUnaryOperator(std::unique_ptr<AST::ASTExpression> &new_value, Operator new_op) {
    op = new_op;
    value = new_value->cloneExpr();
}

AST::ASTFunctionCall::ASTFunctionCall(const AST::ASTFunctionCall &old_func) {
    name = old_func.name->cloneExpr();
    for (auto &i: old_func.arg)
        arg.push_back(i->cloneExpr());
}

std::unique_ptr<AST::ASTExpression> AST::ASTFunctionCall::cloneExpr() const {
    return std::make_unique<ASTFunctionCall>(*this);
}

std::unique_ptr<AST::ASTType> AST::ASTFunctionCall::getType() const {
    //TODO
}

AST::ASTFunctionCall::ASTFunctionCall(const std::unique_ptr<AST::ASTExpression> &new_name,
                                      const std::vector<std::unique_ptr<AST::ASTExpression>> &new_args) {
    name = new_name->cloneExpr();
    for (auto &i: new_args)
        arg.push_back(i->cloneExpr());
}

AST::ASTMemberAccess::ASTMemberAccess(const AST::ASTMemberAccess &old_access) {
    name = old_access.name->cloneExpr();
    member = old_access.member->cloneExpr();
}

std::unique_ptr<AST::ASTExpression> AST::ASTMemberAccess::cloneExpr() const {
    return std::make_unique<AST::ASTMemberAccess>(*this);
}

std::unique_ptr<AST::ASTType> AST::ASTMemberAccess::getType() const {
    //TODO
}

AST::ASTMemberAccess::ASTMemberAccess(const std::unique_ptr<AST::ASTExpression> &new_name,
                                      const std::unique_ptr<AST::ASTExpression> &new_member) {
    name = new_name->cloneExpr();
    member = new_member->cloneExpr();
}

std::unique_ptr<AST::ASTExpression> AST::ASTIntNumber::cloneExpr() const {
    return std::make_unique<AST::ASTIntNumber>(*this);
}

std::unique_ptr<AST::ASTType> AST::ASTIntNumber::getType() const {
    return std::make_unique<AST::ASTTypeInt>();
}

AST::ASTIntNumber::ASTIntNumber(const int new_value) {
    value = new_value;
}

std::unique_ptr<AST::ASTExpression> AST::ASTFloatNumber::cloneExpr() const {
    return std::make_unique<AST::ASTFloatNumber>(*this);
}

std::unique_ptr<AST::ASTType> AST::ASTFloatNumber::getType() const {
    return std::make_unique<AST::ASTTypeFloat>();
}

AST::ASTFloatNumber::ASTFloatNumber(const double new_value) {
    value = new_value;
}

std::unique_ptr<AST::ASTExpression> AST::ASTBoolNumber::cloneExpr() const {
    return std::make_unique<AST::ASTBoolNumber>(*this);
}

std::unique_ptr<AST::ASTType> AST::ASTBoolNumber::getType() const {
    return std::make_unique<AST::ASTTypeBool>();
}

AST::ASTBoolNumber::ASTBoolNumber(const bool new_value) {
    value = new_value;
}

std::unique_ptr<AST::ASTExpression> AST::ASTVar::cloneExpr() const {
    return std::make_unique<AST::ASTVar>(*this);
}

std::unique_ptr<AST::ASTType> AST::ASTVar::getType() const {
//TODO
}

AST::ASTVar::ASTVar(const std::string new_name) {
    name = new_name;
}


AST::ASTDeclaration::ASTDeclaration(const ASTDeclaration &pr_decl) {
    name = pr_decl.name;
    type = pr_decl.type->clone();
    if (pr_decl.value)
        value = pr_decl.value->cloneExpr();
}

std::unique_ptr<AST::Statement> AST::ASTDeclaration::clone() const {
    return this->cloneDecl();
}

AST::ASTDeclaration::ASTDeclaration(const std::string new_name, const std::unique_ptr<AST::ASTExpression> &new_value,
                                    const std::unique_ptr<AST::ASTType> &new_type) {
    name = new_name;
    value = new_value->cloneExpr();
    type = new_type->clone();
}

AST::ASTDeclaration::ASTDeclaration(const std::string new_name, const std::unique_ptr<AST::ASTType> &new_type) {
    name = new_name;
    type = new_type->clone();

    value = nullptr;
}

void AST::Program::setName(std::string new_name) {
    name = new_name;
}


std::unique_ptr<AST::ASTDeclaration> AST::ASTTypeDeclaration::cloneDecl() const {
    return std::make_unique<AST::ASTTypeDeclaration>(*this);
}

std::unique_ptr<AST::ASTDeclaration> AST::ASTVarDeclaration::cloneDecl() const {
    return std::make_unique<AST::ASTVarDeclaration>(*this);
}

std::unique_ptr<AST::ASTDeclaration> AST::ASTConstDeclaration::cloneDecl() const {
    return std::make_unique<AST::ASTConstDeclaration>(*this);
}

std::unique_ptr<AST::Statement> AST::ASTBlock::clone() const {
    return std::make_unique<AST::ASTBlock>(*this);
}

AST::ASTBlock::ASTBlock(const ASTBlock &old_block) {
    for (auto &i: old_block.statements)
        statements.emplace_back(i->clone());
}

void AST::ASTBlock::addStatement(std::unique_ptr<AST::Statement> &stat) {
    statements.emplace_back(stat->clone());
}

std::unique_ptr<AST::Statement> AST::ASTSwitch::clone() const {
    return std::make_unique<AST::ASTSwitch>(*this);
}

AST::ASTSwitch::ASTSwitch(const ASTSwitch &old_switch) {
    expr = old_switch.expr->cloneExpr();
    for (auto &i: old_switch.cases) cases.emplace_back(std::make_pair(i.first->cloneExpr(), i.second->clone()));
}

void AST::ASTSwitch::addExpr(std::unique_ptr<AST::ASTExpression> &new_expr) {
    expr = new_expr->cloneExpr();
}

void AST::ASTSwitch::addCase(std::unique_ptr<AST::ASTExpression> &new_expr, std::unique_ptr<AST::Statement> &block) {
    if (expr == nullptr)
        for (auto &i: cases)
            if (i.first == nullptr)
                throw std::invalid_argument("ERROR. TWO DEFAULT CASES");

    cases.emplace_back(std::make_pair(new_expr->cloneExpr(), block->clone()));
}

std::unique_ptr<AST::Statement> AST::ASTIf::clone() const {
    return std::make_unique<AST::ASTIf>(*this);
}

AST::ASTIf::ASTIf(const ASTIf &old_if) {
    expr = old_if.expr->cloneExpr();
    if_clause = old_if.if_clause->clone();
    if (old_if.else_clause != nullptr)
        else_clause = old_if.else_clause->clone();
}

void AST::ASTIf::addExpr(std::unique_ptr<AST::ASTExpression> &new_expr) {
    expr = new_expr->cloneExpr();
}

void AST::ASTIf::addIfClause(std::unique_ptr<AST::ASTBlock> &new_if_clause) {
    if_clause = new_if_clause->clone();
}

void AST::ASTIf::addElseClause(std::unique_ptr<AST::ASTBlock> &new_else_clause) {
    else_clause = new_else_clause->clone();
}

std::unique_ptr<AST::Statement> AST::ASTFor::clone() const {
    return std::make_unique<AST::ASTFor>(*this);
}

AST::ASTFor::ASTFor(const ASTFor &old_for) {
    for (auto &i: old_for.init_clause) init_clause.emplace_back(i->clone());
    for (auto &i: old_for.iterate_clause) iterate_clause.emplace_back(i->clone());
    if_clause = old_for.if_clause->cloneExpr();
    body = old_for.body->clone();
}

void AST::ASTFor::addInitClause(std::vector<std::unique_ptr<AST::Statement>> &new_init_clause) {
    for (auto &i: new_init_clause)
        init_clause.emplace_back(i->clone());
}

void AST::ASTFor::addIterClause(std::vector<std::unique_ptr<AST::Statement>> &new_iter_clause) {
    for (auto &i: new_iter_clause)
        iterate_clause.emplace_back(i->clone());
}

void AST::ASTFor::addCondClause(std::unique_ptr<AST::ASTExpression> &new_condition) {
    if_clause = new_condition->cloneExpr();
}

void AST::ASTFor::addBody(std::unique_ptr<AST::Statement> &new_body) {
    body = new_body->clone();
}

AST::ASTAssign::ASTAssign(std::string new_name, std::unique_ptr<AST::ASTExpression> &new_value, Type new_type) {
    name = new_name;
    value = new_value->cloneExpr();
    type = new_type;
}

std::unique_ptr<AST::Statement> AST::ASTAssign::clone() const {
    return std::make_unique<ASTAssign>(*this);
}

AST::ASTAssign::ASTAssign(const ASTAssign &old_assign) {
    name = old_assign.name;
    value = old_assign.value->cloneExpr();
    type = old_assign.type;
}

void AST::Function::setName(std::string new_name) {
    name = new_name;
}

void AST::Function::setParams(std::vector<std::pair<std::string, std::unique_ptr<AST::ASTType>>> new_params) {
    for (auto &i: new_params)
        params.emplace_back(i.first, i.second->clone());
}

void AST::Function::addParam(std::string new_name, std::unique_ptr<AST::ASTType> &type) {
    params.emplace_back(new_name, type->clone());
}

void AST::Function::addReturn(std::unique_ptr<AST::ASTType> &new_return) {
    return_type.emplace_back(new_return->clone());
}

void AST::Function::setBody(std::unique_ptr<AST::Statement> &new_body) {
    body = new_body->clone();
}

void AST::Program::addDecl(std::unique_ptr<ASTDeclaration> &new_decl) {
    declarations.push_back(new_decl->cloneDecl());
}

void AST::Program::addFunction(std::unique_ptr<Function> &new_func) {
    functions.push_back(std::move(new_func));
}