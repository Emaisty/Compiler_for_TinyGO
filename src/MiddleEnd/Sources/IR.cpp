#include "IR.h"

IR::Value::Value(long long &counter) {
    inner_number = counter++;
}

void IR::Value::addUse(Value *use) {
    uses.emplace_back(use);
}

IR::Context::Context() {
    goDeeper();
}

T86::Context IR::Context::createT86Context() {
    return T86::Context();
}

void IR::Context::goDeeper() {
    variables.emplace_back();
    functions.emplace_back();

    if (cont_label.empty() && break_label.empty()) {
        cont_label.emplace();
        break_label.emplace();
    } else {
        cont_label.emplace(cont_label.top());
        break_label.emplace(break_label.top());
    }
}

void IR::Context::goUp() {
    variables.erase(variables.end() - 1, variables.end());
    functions.erase(functions.end() - 1, functions.end());

    cont_label.pop();
    break_label.pop();
}

void IR::Context::addContinueLabel(Value *new_label) {
    cont_label.emplace(new_label);
}

IR::Value *IR::Context::getContinueLabel() {
    return cont_label.top();
}

void IR::Context::addBreakLabel(Value *new_label) {
    break_label.emplace(new_label);
}

IR::Value *IR::Context::getBreakLabel() {
    return break_label.top();
}

void IR::Context::addFunction(std::string name, IRFunc *func) {
    functions.back()[name] = func;
}

IR::IRFunc *IR::Context::getFunction(std::string name) {
    for (int i = functions.size() - 1; i >= 0; --i)
        if (functions[i].find(name) != functions[i].end())
            return functions[i][name];
    // case that name is in var space
    return nullptr;
}

void IR::Context::addVariable(std::string name, Value *var) {
    variables.back()[name] = var;
}

IR::Value *IR::Context::getVariable(std::string name) {
    for (int i = variables.size() - 1; i >= 0; --i)
        if (variables[i].find(name) != variables[i].end())
            return variables[i][name];
    // case that name is ref to function
    return nullptr;
}

void IR::Context::setFunction(IRFunc *linl_to_func) {
    where_build = linl_to_func;
}

IR::Value *IR::Context::buildInstruction(std::unique_ptr<Value> &&new_instruction) {
    if (dynamic_cast<IR::IRAlloca *>(new_instruction.get())) {
        where_build->addAlloca(std::move(new_instruction));
        return nullptr;
    }
    where_build->addInstToBody(std::move(new_instruction));
    return where_build->getLinkToBody()->back().get();
}

void IR::Context::deleteLastRow() {
    where_build->getLinkToBody()->erase(where_build->getLinkToBody()->end() - 1, where_build->getLinkToBody()->end());
}

std::unique_ptr<IR::Const> IR::Context::getBasicValue(Type *type) {
    long long nothing = 0;
    if (dynamic_cast<IntType *>(type))
        return std::make_unique<IR::IntConst>(nothing);
    if (dynamic_cast<FloatType *>(type))
        return std::make_unique<IR::DoubleConst>(nothing);
    // TODO if struct
    if (dynamic_cast<PointerType *>(type))
        return std::make_unique<IR::Nullptr>(nothing);

}

void IR::IntConst::addValue(long long val) {
    value = val;
}

void IR::IntConst::print(std::ostream &oss) {
    oss << "   " << "%" << inner_number << " = create int constant " << value << std::endl;
}

std::string IR::IntConst::toString() {
    return std::to_string(value);
}

void IR::DoubleConst::addValue(double val) {
    value = val;
}

void IR::DoubleConst::print(std::ostream &oss) {
    oss << "   " << "%" << inner_number << " = create double constant " << value << std::endl;
}

std::string IR::DoubleConst::toString() {
    return std::to_string(value);
}

void IR::Nullptr::print(std::ostream &oss) {
    oss << "   " << "%" << inner_number << " = create nullptr" << std::endl;
}

std::string IR::Nullptr::toString() {
    return "nullptr";
}

void IR::IRArithOp::addChildren(Value *n_left, Value *n_right) {
    left = n_left;
    right = n_right;
}

void IR::IRArithOp::setTypeOfOperation(Operator new_operator) {
    op = new_operator;
}

void IR::IRArithOp::setTypeOfResult(Type *new_type) {
    result_type = new_type;
}

void IR::IRArithOp::print(std::ostream &oss) {
    std::string name_of_operation = operator_to_str.find(op)->second;
    if (dynamic_cast<FloatType *>(result_type))
        name_of_operation = "f " + name_of_operation;
    oss << "   " << "%" << inner_number << " = " << name_of_operation << " ; left: %" << left->inner_number
        << " ; right: %" << right->inner_number << std::endl;
}

void IR::IRLabel::print(std::ostream &oss) {
    oss << "label %" << inner_number << ":" << std::endl;
}

void IR::IRLoad::addLoadFrom(Value *new_link) {
    where = new_link;
}

IR::Value *IR::IRLoad::getPointer() {
    return where;
}

void IR::IRLoad::print(std::ostream &oss) {
    oss << "   " << "%" << inner_number << " = load from:%" << where->inner_number << std::endl;
}

void IR::IRStore::addStoreWhere(Value *new_link) {
    where = new_link;
}

void IR::IRStore::addStoreWhat(Value *new_link) {
    what = new_link;
}

void IR::IRStore::print(std::ostream &oss) {
    oss << "   " << "store - what: %" << what->inner_number << " ; where: %" << where->inner_number << std::endl;
}

void IR::IRAlloca::addType(Type *new_type) {
    type = new_type;
}

void IR::IRAlloca::addBasicValue(std::unique_ptr<Const> &&new_value) {
    basicValue = std::move(new_value);
}

void IR::IRAlloca::print(std::ostream &oss) {
    // TODO basic value
    oss << "   " << "%" << inner_number << " = alloca '" << type->toString() << "'; ";
//    value is " << basicValue->toString()
    if (basicValue)
        oss << "default value is " << basicValue->toString() << std::endl;
    else
        oss << "no default is set, cause some value set by user" << std::endl;
}

void IR::IRGlobal::addValue(Value *n_value) {
    value = n_value;
}

void IR::IRGlobal::addType(Type *new_type) {
    type = new_type;
}

void IR::IRGlobal::print(std::ostream &oss) {
    // TODO basic value
    oss << "%" << inner_number << " = global '" << type->toString() << "' ; value is " << std::endl << std::endl
        << std::endl;
}

void IR::IRBranch::addCond(Value *new_val) {
    result = new_val;
}

void IR::IRBranch::addBrTaken(Value *taken_label) {
    brT = taken_label;
}

void IR::IRBranch::addBrNTaken(Value *Ntaken_label) {
    brNT = Ntaken_label;
}

void IR::IRBranch::print(std::ostream &oss) {
    if (result) {
        oss << "   " << "cond jmp Cond: %" << result->inner_number << "; If true: %" << brT->inner_number
            << "; false: %" << brNT->inner_number << std::endl;
        return;
    }
    oss << "   " << "jmp %" << brT->inner_number << std::endl;
}

void IR::IRRet::addRetVal(Value *new_val) {
    res = new_val;
}

void IR::IRRet::print(std::ostream &oss) {
    if (res)
        oss << "   " << "ret %" << res->inner_number << std::endl;
    else
        oss << "   " << "ret 'nothing'" << std::endl;
}

void IR::IRCall::addLinkToFunc(IRFunc *new_func) {
    function = new_func;
}

void IR::IRCall::addFunctionName(std::string name) {
    name_of_function = name;
}

void IR::IRCall::addArg(Value *new_arg) {
    arguments.emplace_back(new_arg);
}

void IR::IRCall::print(std::ostream &oss) {
    oss << "   " << "%" << inner_number << " = call %" << name_of_function << " with arguments : (";
    for (auto i = 0; i < arguments.size(); ++i) {
        oss << "%" << arguments[i]->inner_number;

        if (i != arguments.size() - 1)
            oss << ", ";
    }
    oss << ")" << std::endl;
}

void IR::IRCast::addExpr(Value *new_expr) {
    expr = new_expr;
}

void IR::IRCast::addTypeToFrom(Type *new_to, Type *new_from) {
    to = new_to;
    from = new_from;
}

void IR::IRCast::print(std::ostream &oss) {
    oss << "   " << "%" << inner_number << " = cast what: %" << expr->inner_number << "; to: '" << to->toString() << "'"
        << std::endl;
}

void IR::IRFuncArg::addType(Type *new_type) {
    type = new_type;
}

void IR::IRFuncArg::addOrder(int num) {
    order_of_arg = num;
}

void IR::IRFuncArg::print(std::ostream &oss) {
    oss << "'" << type->toString() << "' %" << inner_number;
}

void IR::IRFunc::addReturnType(Type *new_type) {
    return_type = new_type;
}

void IR::IRFunc::addArg(std::unique_ptr<IRFuncArg> &&new_arg) {
    arguments.emplace_back(std::move(new_arg));
}

void IR::IRFunc::addAlloca(std::unique_ptr<Value> &&new_alloca) {
    allocas.emplace_back(std::move(new_alloca));
}

void IR::IRFunc::addInstToBody(std::unique_ptr<Value> &&new_instruction) {
    body.emplace_back(std::move(new_instruction));
}

void IR::IRFunc::setName(std::string new_name) {
    name = new_name;
}

std::string IR::IRFunc::getName() {
    return name;
}

std::vector<std::unique_ptr<IR::Value>> *IR::IRFunc::getLinkToBody() {
    return &body;
}

void IR::IRFunc::print(std::ostream &oss) {
    oss << "function %" << name << "; arguments: (";
    for (auto i = 0; i < arguments.size(); ++i) {
        arguments[i]->print(oss);
        if (i != arguments.size() - 1)
            oss << ", ";
    }
    oss << "); return: '";

    if (return_type)
        oss << return_type->toString();
    else
        oss << "nothing";

    oss << "' {" << std::endl;
    for (auto &i: allocas)
        i->print(oss);
    for (auto &i: body)
        i->print(oss);
    oss << "}" << std::endl << std::endl << std::endl;

}

void IR::IRProgram::addGlobDecl(std::unique_ptr<Value> &&new_decl) {
    globalDecl.emplace_back(std::move(new_decl));
}

void IR::IRProgram::addFunc(std::unique_ptr<Value> &&new_function) {
    functions.emplace_back(std::move(new_function));
}

void IR::IRProgram::print(std::ostream &oss) {
    for (auto &i: globalDecl)
        i->print(oss);

    for (auto &i: functions)
        i->print(oss);
}

IR::IRComment *IR::IRComment::addText(std::string new_text) {
    comment = new_text;
    return this;
}


void IR::IRComment::print(std::ostream &oss) {
    oss << "// " << comment << std::endl;
}

