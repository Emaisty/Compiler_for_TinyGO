#include "IR.h"

void IR::IRValueVar::print(std::ostream &os) const {
    os << "%" << name << ' ';
}

void IR::IRValueIntNumber::print(std::ostream &os) const {
    os << number << ' ';
}

void IR::IRValueFloatNumber::print(std::ostream &os) const {
    os << number << ' ';
}

void IR::IRValueNullPrt::print(std::ostream &os) const {
    os << "nullptr ";
}

void IR::IRValueNone::print(std::ostream &os) const {
    os << "void ";
}

std::shared_ptr<IR::IRValue> IR::IRTypeInt::getDefaultValue() {
    return std::make_shared<IRValueIntNumber>();
}

void IR::IRTypeInt::print(std::ostream &os) const {
    os << "i" << bits << ' ';
}

std::shared_ptr<IR::IRValue> IR::IRTypeFloat::getDefaultValue() {
    return std::make_shared<IRValueFloatNumber>();
}

void IR::IRTypeFloat::print(std::ostream &os) const {
    os << "float ";
}

void IR::IRTypeStruct::addFiled(std::shared_ptr<IRType> &&new_field) {
    fields.emplace_back(new_field);
}

std::shared_ptr<IR::IRValue> IR::IRTypeStruct::getDefaultValue() {
    return std::make_shared<IRValueNullPrt>();
}

void IR::IRTypeStruct::print(std::ostream &os) const {
    os << "{ ";
    for (auto i = 0; i < fields.size(); ++i)
        os << *fields[i] << ((i != fields.size() - 1) ? ", " : "");
    os << "} ";
}

std::shared_ptr<IR::IRValue> IR::IRTypeNull::getDefaultValue() {
    return std::make_shared<IRValueNone>();
}

void IR::IRTypeNull::print(std::ostream &os) const {
    os << "void ";
}

std::shared_ptr<IR::IRValue> IR::IRTypePointer::getDefaultValue() {
    return std::make_shared<IRValueNullPrt>();
}

IR::IRTypePointer::IRTypePointer(std::shared_ptr<IRType> &&new_type) {
    type = new_type;
}

void IR::IRTypePointer::print(std::ostream &os) const {
    os << "*" << *type;
}

void IR::IRDecl::addName(std::string new_name) {
    name = new_name;
}

void IR::IRDecl::addType(std::shared_ptr<IR::IRType> &&new_type) {
    type = new_type;
}

std::shared_ptr<IR::IRValue> IR::IRGlobalDecl::getName() {
    throw std::invalid_argument("ERROR. Declarations do not provide return value.");
}

void IR::IRGlobalVarDecl::print(std::ostream &os) const {
    os << "@" << name << " = dso_local global " << *type << *type->getDefaultValue() << std::endl;
}

void IR::IRGlobalTypeDecl::print(std::ostream &os) const {
    os << "%class." << name << " = " << *type << std::endl;
}

void IR::IRProgram::addDecl(std::shared_ptr<IRGlobalDecl> &&new_decl) {
    declarations.emplace_back(new_decl);
}

void IR::IRFunc::addName(std::string new_name) {
    name = new_name;
}

void IR::IRFunc::addReturn(std::shared_ptr<IRType> &&new_type) {
    return_type = new_type;
}

void IR::IRFunc::addArg(std::shared_ptr<IRType> &&arg) {
    arguments.emplace_back(arg);
}

void IR::IRFunc::addStatement(std::shared_ptr<IRLine> &&statement) {
    body.emplace_back(statement);
}

void IR::IRProgram::addFunc(std::shared_ptr<IRFunc> &&new_func) {
    functions.emplace_back(new_func);
}


namespace IR {

    std::ostream &operator<<(std::ostream &os, const IRValue &value) {
        value.print(os);
        return os;
    }


    std::ostream &operator<<(std::ostream &os, const IRType &type) {
        type.print(os);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const IRLine &line) {
        line.print(os);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const IRGlobalDecl &decl) {
        decl.print(os);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const IRFunc &func) {
        os << "define " << *func.return_type << "@" << func.name << " (";
        for (auto i = 0; i < func.arguments.size(); ++i)
            os << *func.arguments[i] << "%" << i << ((i != func.arguments.size() - 1) ? ", " : "");
        os << ") {" << std::endl;

//        for (auto &i: func.body)
//            os << *i;
        // TODO add return with default value
//        os << "ret " << *func.return_type << *func.return_type->getDefaultValue() << std::endl;

        os << "}" << std::endl << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const IRProgram &program) {
        for (auto &i: program.declarations)
            os << *i;

        for (auto &i: program.functions)
            os << *i;

        return os;
    }
}