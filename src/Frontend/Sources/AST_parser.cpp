#include "AST.h"

std::unique_ptr<AST::ASTType> AST::ASTType::set_addressability(bool flag) {
    is_variable = flag;
    return this->clone();
}

std::unique_ptr<AST::ASTType> AST::ASTType::set_constability(bool flag) {
    is_const = flag;
    return this->clone();
}

bool AST::ASTType::addressable() {
    return is_variable;
}

bool AST::ASTType::constable() {
    return is_const;
}

std::unique_ptr<AST::ASTType> AST::ASTTypeNull::clone() const {
    return std::make_unique<AST::ASTTypeNull>(*this);
}

bool AST::ASTTypeNull::operator==(const std::unique_ptr<ASTType> &&type) const {
    if (dynamic_cast<AST::ASTTypeNull *>(type.get()))
        return true;
    return false;
}

AST::ASTTypeInt::ASTTypeInt(int new_bits) : bits(new_bits) {}

std::unique_ptr<AST::ASTType> AST::ASTTypeInt::clone() const {

    return std::make_unique<ASTTypeInt>(*this);
}

bool AST::ASTTypeInt::operator==(const std::unique_ptr<ASTType> &&type) const {
    if (dynamic_cast<AST::ASTTypeInt *>(type.get()) || dynamic_cast<AST::ASTTypeFloat *>(type.get()))
        return true;
    return false;
}


std::unique_ptr<AST::ASTType> AST::ASTTypeFloat::clone() const {
    return std::make_unique<ASTTypeFloat>(*this);
}

bool AST::ASTTypeFloat::operator==(const std::unique_ptr<ASTType> &&type) const {
    if (dynamic_cast<AST::ASTTypeInt *>(type.get()) || dynamic_cast<AST::ASTTypeFloat *>(type.get()))
        return true;
    return false;
}


std::unique_ptr<AST::ASTType> AST::ASTTypeBool::clone() const {
    return std::make_unique<ASTTypeBool>(*this);
}

bool AST::ASTTypeBool::operator==(const std::unique_ptr<ASTType> &&type) const {
    if (dynamic_cast<AST::ASTTypeBool *>(type.get()))
        return true;
    return false;
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

bool AST::ASTTypePointer::operator==(const std::unique_ptr<ASTType> &&type) const {
    auto pointer = dynamic_cast<AST::ASTTypePointer *>(type.get());
    if (pointer)
        return this->type == pointer->type;
    return false;
}

std::unique_ptr<AST::ASTType> AST::ASTTypePointer::getValue() {
    return type->clone();
}

AST::ASTTypeStruct::ASTTypeStruct(const AST::ASTTypeStruct &old_pointer) {
    for (auto &i: old_pointer.fileds)
        fileds.emplace_back(i.first, i.second->clone());
}

std::unique_ptr<AST::ASTType> AST::ASTTypeStruct::clone() const {
    return std::make_unique<ASTTypeStruct>(*this);
}

void AST::ASTTypeStruct::addField(std::string name, std::unique_ptr<AST::ASTType> &type) {
    for (auto &i: fileds)
        if (name == i.first && name != "")
            throw std::invalid_argument("ERROR. 2 or more fields in structure with the same names");
    fileds.emplace_back(name, type->clone());
}

bool AST::ASTTypeStruct::operator==(const std::unique_ptr<ASTType> &&type) const {
    auto struc = dynamic_cast<AST::ASTTypeStruct *>(type.get());
    if (!struc)
        return false;

    if (this->fileds.size() != struc->fileds.size())
        return false;
    for (int i = 0; i < fileds.size(); ++i)
        if (!(fileds[i].second == struc->fileds[0].second))
            return false;

    return true;
}

std::unique_ptr<AST::ASTType> AST::ASTTypeStruct::findField(std::string name) {
    for (auto &i: fileds)
        if (i.first == name)
            return i.second->clone();

    throw std::invalid_argument("ERROR. Field with such name does not exists.");
    return nullptr;
}

void AST::Statement::addLineNumber(int num) {
    line_number = num;
}

AST::ASTBinaryOperator::ASTBinaryOperator(std::unique_ptr<AST::ASTExpression> &new_left,
                                          std::unique_ptr<AST::ASTExpression> &new_right, Operator new_op) {
    op = new_op;
    left = std::move(new_left);
    right = std::move(new_right);
}

AST::ASTUnaryOperator::ASTUnaryOperator(std::unique_ptr<AST::ASTExpression> &&new_value, Operator new_op) {
    op = new_op;
    value = std::move(new_value);
}

AST::ASTFunctionCall::ASTFunctionCall(std::unique_ptr<AST::ASTExpression> &new_name,
                                      std::vector<std::unique_ptr<AST::ASTExpression>> &new_args) {
    name = std::move(new_name);
    for (auto &i: new_args)
        arg.push_back(std::move(i));
}


AST::ASTMemberAccess::ASTMemberAccess(std::unique_ptr<AST::ASTExpression> &new_name,
                                      std::unique_ptr<AST::ASTExpression> &new_member) {
    name = std::move(new_name);
    member = std::move(new_member);
}

AST::ASTGetPointer::ASTGetPointer(std::unique_ptr<ASTExpression> &new_var) {
    var = std::move(new_var);
}

AST::ASTGetValue::ASTGetValue(std::unique_ptr<ASTExpression> &new_var) {
    var = std::move(new_var);
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

AST::ASTStruct::ASTStruct(std::unique_ptr<ASTTypeStruct> &new_type,
                          std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> &new_values) {
    type = new_type->clone();
    for (auto &i: new_values)
        values.emplace_back(i.first, std::move(i.second));
}

AST::ASTVar::ASTVar(const std::string new_name) {
    name = new_name;
}

std::string AST::ASTVar::getName() {
    return name;
}

AST::ASTDeclaration::ASTDeclaration(const std::string new_name, std::unique_ptr<AST::ASTExpression> &&new_value,
                                    std::unique_ptr<AST::ASTType> &&new_type) {
    name = new_name;
    value = std::move(new_value);
    type = new_type->clone();
}

AST::ASTDeclaration::ASTDeclaration(const std::string new_name, std::unique_ptr<ASTExpression> &&new_value) {
    name = new_name;
    value = std::move(new_value);

    type = nullptr;
}

AST::ASTDeclaration::ASTDeclaration(const std::string new_name, std::unique_ptr<AST::ASTType> &&new_type) {
    name = new_name;
    type = new_type->clone();

    value = nullptr;
}

void AST::Program::setName(std::string new_name) {
    name = new_name;
}


void AST::ASTBlock::addStatement(std::unique_ptr<AST::Statement> &&stat) {
    statements.emplace_back(std::move(stat));
}


void AST::ASTReturn::addReturnValue(std::unique_ptr<ASTExpression> &&new_value) {
    return_value = std::move(new_value);
}

void AST::ASTSwitch::addExpr(std::unique_ptr<AST::ASTExpression> &&new_expr) {
    expr = std::move(new_expr);
}

void AST::ASTSwitch::addCase(std::unique_ptr<AST::ASTExpression> &new_expr, std::unique_ptr<AST::Statement> &block) {
    if (expr == nullptr)
        for (auto &i: cases)
            if (i.first == nullptr)
                throw std::invalid_argument("ERROR. TWO DEFAULT CASES");

    cases.emplace_back(std::make_pair(std::move(new_expr), std::move(block)));
}

void AST::ASTIf::addExpr(std::unique_ptr<AST::ASTExpression> &new_expr) {
    expr = std::move(new_expr);
}

void AST::ASTIf::addIfClause(std::unique_ptr<AST::ASTBlock> &new_if_clause) {
    if_clause = std::move(new_if_clause);
}

void AST::ASTIf::addElseClause(std::unique_ptr<AST::ASTBlock> &new_else_clause) {
    else_clause = std::move(new_else_clause);
}

void AST::ASTFor::addInitClause(std::vector<std::unique_ptr<AST::Statement>> &new_init_clause) {
    for (auto &i: new_init_clause)
        init_clause.emplace_back(std::move(i));
}

void AST::ASTFor::addIterClause(std::vector<std::unique_ptr<AST::Statement>> &new_iter_clause) {
    for (auto &i: new_iter_clause)
        iterate_clause.emplace_back(std::move(i));
}

void AST::ASTFor::addCondClause(std::unique_ptr<AST::ASTExpression> &&new_condition) {
    if_clause = std::move(new_condition);
}

void AST::ASTFor::addBody(std::unique_ptr<AST::Statement> &&new_body) {
    body = std::move(new_body);
}

AST::ASTAssign::ASTAssign(std::unique_ptr<AST::ASTExpression> &&new_variable,
                          std::unique_ptr<AST::ASTExpression> &&new_value, Type new_type) {
    variable = std::move(new_variable);
    value = std::move(new_value);
    type = new_type;
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
    return_type = new_return->clone();
}

void AST::Function::setBody(std::unique_ptr<AST::Statement> &new_body) {
    body = std::move(new_body);
}

void AST::Function::addLineNumber(int num) {
    line_number = num;

}

void AST::Program::addDecl(std::unique_ptr<ASTDeclaration> &new_decl) {
    declarations.push_back(std::move(new_decl));
}

void AST::Program::addFunction(std::unique_ptr<Function> &new_func) {
    functions.push_back(std::move(new_func));
}