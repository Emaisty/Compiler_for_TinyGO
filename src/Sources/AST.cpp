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


std::unique_ptr<AST::Statement> AST::ASTExpression::clone() const {
    return this->clone_expr();
}

AST::ASTBinaryOperator::ASTBinaryOperator(const ASTBinaryOperator &old_expr) {
    op = old_expr.op;
    left = old_expr.left->clone_expr();
    right = old_expr.right->clone_expr();

}

std::unique_ptr<AST::ASTExpression> AST::ASTBinaryOperator::clone_expr() const {
    return std::make_unique<ASTBinaryOperator>(*this);
}

AST::ASTBinaryOperator::ASTBinaryOperator(std::unique_ptr<AST::ASTExpression> &new_left,
                                          std::unique_ptr<AST::ASTExpression> &new_right, Operator new_op) {
    op = new_op;
    left = new_left->clone_expr();
    right = new_right->clone_expr();
}


AST::ASTUnaryOperator::ASTUnaryOperator(const AST::ASTUnaryOperator &old_expr) {
    op = old_expr.op;
    value = old_expr.value->clone_expr();
}

std::unique_ptr<AST::ASTExpression> AST::ASTUnaryOperator::clone_expr() const {
    return std::make_unique<ASTUnaryOperator>(*this);
}

AST::ASTUnaryOperator::ASTUnaryOperator(std::unique_ptr<AST::ASTExpression> &new_value, Operator new_op) {
    op = new_op;
    value = new_value->clone_expr();
}

AST::ASTFunctionCall::ASTFunctionCall(const AST::ASTFunctionCall &old_func) {
    name = old_func.name->clone_expr();
    for (auto &i: old_func.arg)
        arg.push_back(i->clone_expr());
}

std::unique_ptr<AST::ASTExpression> AST::ASTFunctionCall::clone_expr() const {
    return std::make_unique<ASTFunctionCall>(*this);
}

AST::ASTFunctionCall::ASTFunctionCall(const std::unique_ptr<AST::ASTExpression> &new_name,
                                      const std::vector<std::unique_ptr<AST::ASTExpression>> &new_args) {
    name = new_name->clone_expr();
    for (auto &i: new_args)
        arg.push_back(i->clone_expr());
}

AST::ASTMemberAccess::ASTMemberAccess(const AST::ASTMemberAccess &old_access) {
    name = old_access.name->clone_expr();
    member = old_access.member->clone_expr();
}

std::unique_ptr<AST::ASTExpression> AST::ASTMemberAccess::clone_expr() const {
    return std::make_unique<AST::ASTMemberAccess>(*this);
}

AST::ASTMemberAccess::ASTMemberAccess(const std::unique_ptr<AST::ASTExpression> &new_name,
                                      const std::unique_ptr<AST::ASTExpression> &new_member) {
    name = new_name->clone_expr();
    member = new_member->clone_expr();
}

std::unique_ptr<AST::ASTExpression> AST::ASTIntNumber::clone_expr() const {
    return std::make_unique<AST::ASTIntNumber>(*this);
}

AST::ASTIntNumber::ASTIntNumber(const int new_value) {
    value = new_value;
}

std::unique_ptr<AST::ASTExpression> AST::ASTFloatNumber::clone_expr() const {
    return std::make_unique<AST::ASTFloatNumber>(*this);
}

AST::ASTFloatNumber::ASTFloatNumber(const double new_value) {
    value = new_value;
}

std::unique_ptr<AST::ASTExpression> AST::ASTVar::clone_expr() const {
    return std::make_unique<AST::ASTVar>(*this);
}

AST::ASTVar::ASTVar(const std::string new_name) {
    name = new_name;
}


AST::ASTDeclaration::ASTDeclaration(const ASTDeclaration &pr_decl) {
    name = pr_decl.name;
    value = pr_decl.value->clone_expr();
}

std::unique_ptr<AST::Statement> AST::ASTDeclaration::clone() const {
    return this->clone_decl();
}

AST::ASTDeclaration::ASTDeclaration(std::string new_name, std::unique_ptr<AST::ASTExpression> &new_value) {
    name = new_name;
    value = new_value->clone_expr();
}

void AST::ASTDeclaration::setName(std::string new_name) {
    name = new_name;
}

void AST::ASTDeclaration::setValue(std::unique_ptr<AST::ASTExpression> &new_type) {
    value = new_type->clone_expr();
}

void AST::Program::setName(std::string new_name) {
    name = new_name;
}


std::unique_ptr<AST::ASTDeclaration> AST::ASTTypeDeclaration::clone_decl() const {
    return std::make_unique<AST::ASTTypeDeclaration>(*this);
}

std::unique_ptr<AST::ASTDeclaration> AST::ASTVarDeclaration::clone_decl() const {
    return std::make_unique<AST::ASTVarDeclaration>(*this);
}

std::unique_ptr<AST::ASTDeclaration> AST::ASTConstDeclaration::clone_decl() const {
    return std::make_unique<AST::ASTConstDeclaration>(*this);
}

void AST::Program::addDecl(std::unique_ptr<ASTDeclaration> &new_decl) {
    declarations.push_back(std::move(new_decl));
}

void AST::Program::addFunction(std::unique_ptr<Function> &new_func) {
    functions.push_back(std::move(new_func));
}