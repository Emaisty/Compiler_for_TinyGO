#include "AST.h"

AST::ASTTypeInt::ASTTypeInt(int new_bits) : bits(new_bits) {}

std::unique_ptr<AST::ASTType> AST::ASTTypeInt::clone() const {

    return std::make_unique<ASTTypeInt>(*this);
}


std::unique_ptr<AST::ASTType> AST::ASTTypeFloat::clone() const {

    return std::make_unique<ASTTypeFloat>(*this);
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
    //TODO
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

std::unique_ptr<AST::ASTExpression> AST::ASTVar::cloneExpr() const {
    return std::make_unique<AST::ASTVar>(*this);
}

AST::ASTVar::ASTVar(const std::string new_name) {
    name = new_name;
}


AST::ASTDeclaration::ASTDeclaration(const ASTDeclaration &pr_decl) {
    name = pr_decl.name;
    value = pr_decl.value->cloneExpr();
    type = pr_decl.type->clone();
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

void AST::Program::addDecl(std::unique_ptr<ASTDeclaration> &new_decl) {
    declarations.push_back(std::move(new_decl));
}

void AST::Program::addFunction(std::unique_ptr<Function> &new_func) {
    functions.push_back(std::move(new_func));
}