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


AST::ASTBinaryOperator::ASTBinaryOperator(const ASTBinaryOperator &old_expr) {
    op = old_expr.op;
    left = old_expr.left->clone();
    right = old_expr.right->clone();

}

std::unique_ptr<AST::ASTExpression> AST::ASTBinaryOperator::clone() const {
    return std::make_unique<ASTBinaryOperator>(*this);
}

AST::ASTBinaryOperator::ASTBinaryOperator(std::unique_ptr<AST::ASTExpression> &new_left,
                                          std::unique_ptr<AST::ASTExpression> &new_right, Operator new_op) {
    op = new_op;
    left = new_left->clone();
    right = new_right->clone();
}

std::unique_ptr<AST::ASTExpression> AST::ASTUnaryOperator::clone() const {
    return std::make_unique<ASTUnaryOperator>(*this);
}

AST::ASTUnaryOperator::ASTUnaryOperator(std::unique_ptr<AST::ASTExpression> &new_value, Operator new_op) {
    op = new_op;
    value = new_value->clone();
}

AST::ASTFunctionCall::ASTFunctionCall(const AST::ASTFunctionCall &old_func) {
    name = old_func.name->clone();
    for (auto &i: old_func.arg)
        arg.push_back(i->clone());
}

std::unique_ptr<AST::ASTExpression> AST::ASTFunctionCall::clone() const {
    return std::make_unique<ASTFunctionCall>(*this);
}

AST::ASTFunctionCall::ASTFunctionCall(const std::unique_ptr<AST::ASTExpression> &new_name,
                                      const std::vector<std::unique_ptr<AST::ASTExpression>> &new_args) {
    name = new_name->clone();
    for (auto &i: new_args)
        arg.push_back(i->clone());
}

AST::ASTMemberAccess::ASTMemberAccess(const AST::ASTMemberAccess &old_access) {
    name = old_access.name->clone();
    member = old_access.member->clone();
}

std::unique_ptr<AST::ASTExpression> AST::ASTMemberAccess::clone() const {
    return std::make_unique<AST::ASTMemberAccess>(*this);
}

AST::ASTMemberAccess::ASTMemberAccess(const std::unique_ptr<AST::ASTExpression> &new_name,
                                      const std::unique_ptr<AST::ASTExpression> &new_member) {
    name = new_name->clone();
    member = new_member->clone();
}

void AST::ASTDeclaration::setName(std::string new_name) {
    name = new_name;
}

void AST::ASTDeclaration::setType(std::unique_ptr<AST::ASTType> &new_type) {
    type = new_type->clone();
}

void AST::Program::setName(std::string new_name) {
    name = new_name;
}

void AST::Program::addDecl(std::unique_ptr<ASTDeclaration> &new_decl) {
    declarations.push_back(std::move(new_decl));
}

void AST::Program::addFunction(std::unique_ptr<Function> &new_func) {
    functions.push_back(std::move(new_func));
}