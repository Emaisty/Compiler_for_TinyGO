#include "AST.h"

void AST::ASTNode::addLineNumber(int line_num) {
    line = line_num;
}


AST::ASTTypePointer::ASTTypePointer(std::unique_ptr<ASTType> &&new_type) {
    type = std::move(new_type);
}

AST::ASTType *AST::ASTTypePointer::getValue() {
    return type.get();
}

void AST::ASTTypeStruct::addField(std::vector<std::string> &&name, std::unique_ptr<AST::ASTType> &&type) {
    for (auto &i : name)
        for (auto &[j,_] : fileds)
            for (auto &k : j)
                if (i == k)
                    throw std::invalid_argument("ERROR. 2 or more fields in structure with the same names");
    fileds.emplace_back(std::move(name), std::move(type));
}

AST::ASTType *AST::ASTTypeStruct::findField(std::string name) const {
    for (auto &[i,j] : fileds)
        for (auto &k : i)
            if (name == k)
                return j.get();
    return nullptr;
}

AST::ASTBinaryOperator::ASTBinaryOperator(std::unique_ptr<AST::ASTExpression> &&new_left,
                                          std::unique_ptr<AST::ASTExpression> &&new_right,
                                          IR::IRArithOp::Operator new_op) {
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

void AST::ASTFunctionCall::setArgs(std::vector<std::unique_ptr<ASTExpression>>&&new_args){
    arg = std::move(new_args);
}

std::vector<std::unique_ptr<AST::ASTExpression>> AST::ASTFunctionCall::resetArgs(){
    return std::move(arg);
}


AST::ASTMemberAccess::ASTMemberAccess(std::unique_ptr<AST::ASTExpression> &&new_name,
                                      std::string new_member) {
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
                          std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>

                          > &new_values) {
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

void AST::ASTScan::addExpression(std::unique_ptr<AST::ASTExpression>&& new_expr) {
    expression = std::move(new_expr);
}

void AST::ASTPrint::addExpression(std::unique_ptr<AST::ASTExpression>&& new_expr) {
    expression = std::move(new_expr);
}

void AST::Function::setName(std::string new_name) {
    name = new_name;
}

void AST::Function::addParam(std::vector<std::string> &&new_names, std::unique_ptr<ASTType> &&new_type) {
    params.emplace_back(std::move(new_names), std::move(new_type));
}

std::vector<Type*> AST::Function::getListOfArgTypes(){
    std::vector<Type*> res;
    for (auto &i : params)
        for(auto &j : i.first)
            res.emplace_back(i.second->typeOfNode);
}

void AST::Function::setMethod(std::string new_name, std::unique_ptr<ASTType> &&new_type) {
    inner_name = new_name;
    type_of_method = std::move(new_type);
}

void AST::Function::addReturn(std::unique_ptr<AST::ASTType> &&new_return) {
    return_type.emplace_back(std::move(new_return));
}

void AST::Function::setBody(std::unique_ptr<AST::ASTBlock> &&new_body) {
    body = std::move(new_body);
}

void AST::Program::addVarDecl(std::unique_ptr<ASTDeclaration> &&new_decl) {
    varDeclarations.emplace_back(std::move(new_decl));
}

void AST::Program::addTypeDecl(std::unique_ptr<ASTDeclaration> &&new_decl) {
    typeDeclarations.emplace_back(std::move(new_decl));
}

void AST::Program::addFunction(std::unique_ptr<Function> &&new_func) {
    functions.push_back(std::move(new_func));
}