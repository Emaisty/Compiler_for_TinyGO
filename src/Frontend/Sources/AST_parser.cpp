#include "AST.h"

void AST::ASTNode::addLineNumber(int line_num) {
    line = line_num;
}

bool AST::ASTNode::hasAddress() {
    return false;
}

bool AST::ASTNode::isConst() {
    return false;
}

AST::ASTTypePointer::ASTTypePointer(std::unique_ptr<ASTType> &&new_type) {
    type = std::move(new_type);
}

AST::ASTType *AST::ASTTypePointer::getValue() {
    return type.get();
}

void AST::ASTTypeStruct::addField(std::string name, std::unique_ptr<AST::ASTType> &&type) {
    for (auto &i: fileds)
        if (name == i.first && name != "")
            throw std::invalid_argument("ERROR. 2 or more fields in structure with the same names");
    fileds.emplace_back(name, std::move(type));
}

AST::ASTType *AST::ASTTypeStruct::findField(std::string name) {
    for (auto &i: fileds)
        if (i.first == name)
            return i.second.get();

    return nullptr;
}

AST::ASTBinaryOperator::ASTBinaryOperator(std::unique_ptr<AST::ASTExpression> &&new_left,
                                          std::unique_ptr<AST::ASTExpression> &&new_right, Operator new_op) {
    op = new_op;
    left = std::move(new_left);
    right = std::move(new_right);
}

AST::ASTUnaryOperator::ASTUnaryOperator(std::unique_ptr<AST::ASTExpression> &&new_value, Operator new_op) {
    op = new_op;
    value = std::move(new_value);
}

AST::ASTFunctionCall::ASTFunctionCall(std::unique_ptr<AST::ASTExpression> &&new_name,
                                      std::vector<std::unique_ptr<AST::ASTExpression>> &new_args) {
    name = std::move(new_name);
    arg = std::move(new_args);
}


AST::ASTMemberAccess::ASTMemberAccess(std::unique_ptr<AST::ASTExpression> &&new_name,
                                      std::string &new_member) {
    name = std::move(new_name);
    member = new_member;
}

AST::ASTIntNumber::ASTIntNumber(const int new_value) {
    value = new_value;
}

AST::ASTFloatNumber::ASTFloatNumber(const double new_value) {
    value = new_value;
}

AST::ASTBoolNumber::ASTBoolNumber(const bool new_value) {
    value = new_value;
}

AST::ASTStruct::ASTStruct(std::unique_ptr<ASTTypeStruct> &&new_type,
                          std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> &new_values) {
    type = std::move(new_type);
    for (auto &i: new_values)
        values.emplace_back(i.first, std::move(i.second));
}

AST::ASTVar::ASTVar(const std::string new_name) {
    name = new_name;
}

std::string AST::ASTVar::getName() {
    return name;
}

AST::ASTDeclaration::ASTDeclaration(std::vector<std::string> &&new_name,
                                    std::vector<std::unique_ptr<AST::ASTExpression>> &&new_value,
                                    std::unique_ptr<AST::ASTType> &&new_type) {
    name = new_name;
    value = std::move(new_value);
    type = std::move(new_type);
}

AST::ASTDeclaration::ASTDeclaration(std::vector<std::string> &&new_name,
                                    std::vector<std::unique_ptr<ASTExpression>> &&new_value) {
    name = new_name;
    value = std::move(new_value);
    type = nullptr;
}

AST::ASTDeclaration::ASTDeclaration(std::vector<std::string> &&new_name, std::unique_ptr<AST::ASTType> &&new_type) {
    name = new_name;
    type = std::move(new_type);
}

void AST::Program::setName(std::string new_name) {
    name = new_name;
}


void AST::ASTBlock::addStatement(std::unique_ptr<AST::Statement> &&stat) {
    statements.emplace_back(std::move(stat));
}


void AST::ASTReturn::addReturnValue(std::unique_ptr<ASTExpression> &&new_value) {
    return_value.emplace_back(std::move(new_value));
}

void AST::ASTSwitch::addExpr(std::unique_ptr<AST::ASTExpression> &&new_expr) {
    expr = std::move(new_expr);
}

void AST::ASTSwitch::addCase(std::unique_ptr<AST::ASTExpression> &&new_expr, std::unique_ptr<AST::ASTBlock> &&block) {
    if (expr == nullptr)
        for (auto &i: cases)
            if (i.first == nullptr)
                throw std::invalid_argument("ERROR. TWO DEFAULT CASES");

    cases.emplace_back(std::make_pair(std::move(new_expr), std::move(block)));
}

void AST::ASTIf::addExpr(std::unique_ptr<AST::ASTExpression> &&new_expr) {
    expr = std::move(new_expr);
}

void AST::ASTIf::addIfClause(std::unique_ptr<AST::ASTBlock> &&new_if_clause) {
    if_clause = std::move(new_if_clause);
}

void AST::ASTIf::addElseClause(std::unique_ptr<AST::ASTBlock> &&new_else_clause) {
    else_clause = std::move(new_else_clause);
}

void AST::ASTFor::addInitClause(std::vector<std::unique_ptr<AST::Statement>> &&new_init_clause) {
    init_clause = std::move(new_init_clause);
}

void AST::ASTFor::addIterClause(std::vector<std::unique_ptr<AST::Statement>> &&new_iter_clause) {
    iterate_clause = std::move(new_iter_clause);
}

void AST::ASTFor::addCondClause(std::unique_ptr<AST::ASTExpression> &&new_condition) {
    if_clause = std::move(new_condition);
}

void AST::ASTFor::addBody(std::unique_ptr<AST::ASTBlock> &&new_body) {
    body = std::move(new_body);
}

AST::ASTAssign::ASTAssign(std::vector<std::unique_ptr<AST::ASTExpression>> &&new_variable,
                          std::vector<std::unique_ptr<AST::ASTExpression>> &&new_value, TypeOfAssign new_type) {
    variable = std::move(new_variable);
    value = std::move(new_value);
    type = new_type;
}

void AST::Function::setName(std::string new_name) {
    name = new_name;
}

void AST::Function::setParams(std::vector<std::unique_ptr<ASTDeclaration>> &&new_params) {
    params = std::move(new_params);
}

void AST::Function::addParam(std::unique_ptr<ASTDeclaration> &&new_param) {
    params.emplace_back(std::move(new_param));
}

void AST::Function::addReturn(std::unique_ptr<AST::ASTType> &&new_return) {
    return_type.emplace_back(std::move(new_return));
}

void AST::Function::setBody(std::unique_ptr<AST::ASTBlock> &&new_body) {
    body = std::move(new_body);
}

void AST::Program::addDecl(std::unique_ptr<ASTDeclaration> &&new_decl) {
    declarations.push_back(std::move(new_decl));
}

void AST::Program::addFunction(std::unique_ptr<Function> &&new_func) {
    functions.push_back(std::move(new_func));
}