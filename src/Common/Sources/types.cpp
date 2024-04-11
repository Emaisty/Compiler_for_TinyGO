#include "types.h"

bool Type::compareSignatures(const Type *other) const {
    return this == other;
}

bool IntType::canConvertToThisType(const Type *other) const {
    if (dynamic_cast<const IntType *>(other) || dynamic_cast<const FloatType *>(other))
        return true;
    return false;
}

std::string IntType::toString() {
    return "i" + std::to_string(bits);
}

bool BoolType::canConvertToThisType(const Type *other) const {
    if (this == other)
        return true;
    return false;
}

std::string BoolType::toString() {
    return "i1";
}

bool FloatType::canConvertToThisType(const Type *other) const {
    if (dynamic_cast<const IntType *>(other) || this == other)
        return true;
    return false;
}

std::string FloatType::toString() {
    return "float";
}


bool StructType::canConvertToThisType(const Type *other) const {
    if (!dynamic_cast<const StructType *>(other))
        return false;
    auto another_struct = dynamic_cast<const StructType *>(other);

    if (this->fields.size() != another_struct->fields.size())
        return false;

    for (auto i = 0; i < fields.size(); ++i)
        if (fields[i].first != another_struct->fields[i].first ||
            !fields[i].second->canConvertToThisType(another_struct->fields[i].second))
            return false;

    return true;
}

bool StructType::compareSignatures(const Type *other) const {
    if (dynamic_cast<const StructType *>(other)) {
        auto other_struct = dynamic_cast<const StructType *>(other);
        if (fields.size() != other_struct->fields.size())
            return false;

        for (auto i = 0; i < fields.size(); ++i)
            if (fields[i].first != other_struct->fields[i].first || fields[i].second != other_struct->fields[i].second)
                return false;


        return true;
    }
    return false;
}

void StructType::addNewField(std::string new_name, Type *new_type) {
    if (nameAlreadyExists(new_name))
        throw std::invalid_argument("ERROR. Field with such name already exists.");

    fields.emplace_back(new_name, new_type);
}

Type *StructType::getField(std::string name) const {
    for (auto &i: fields)
        if (i.first == name)
            return i.second;
    return nullptr;
}

Type **StructType::getDoubleLinkToField(std::string name) {
    for (auto &i: fields)
        if (i.first == name)
            return &i.second;
    return nullptr;
}

bool StructType::nameAlreadyExists(std::string name) {
    for (auto &i: fields)
        if (i.first == name)
            return true;
    return false;
}

std::string StructType::toString() {
    // TODO later
}


bool PointerType::canConvertToThisType(const Type *other) const {
    if (dynamic_cast<const PointerType *>(other) && dynamic_cast<const PointerType *>(other)->base_type == base_type)
        return true;
    return false;
}

Type *PointerType::getBase() {
    return base_type;
}

std::string PointerType::toString() {
    return "ptr";
}

bool FunctionType::canConvertToThisType(const Type *other) const {
    if (this == other)
        return true;
    return false;
}

bool FunctionType::compareSignatures(const Type *other) const {
    if (!dynamic_cast<const FunctionType *>(other))
        return false;
    auto other_func = dynamic_cast<const FunctionType *>(other);

    if ((other_func->return_type && !return_type) || (!other_func->return_type && return_type))
        return false;

    if (return_type && !return_type->canConvertToThisType(other_func->return_type))
        return false;

    if (args.size() != other_func->args.size())
        return false;
    for (auto i = 0; i < args.size(); ++i)
        if (!args[i]->canConvertToThisType(other_func->args[i]))
            return false;

    return true;
}

bool FunctionType::compareArgs(const std::vector<Type *> &other_args) {
    if (args.size() != other_args.size())
        return false;
    for (auto i = 0; i < args.size(); ++i)
        if (!args[i]->canConvertToThisType(other_args[i]))
            return false;

    return true;
}

Type *FunctionType::getReturn() {
    return return_type;
}

void FunctionType::setReturn(Type *new_type) {
    return_type = new_type;
}

void FunctionType::addParam(Type *new_type) {
    args.emplace_back(new_type);
}

std::string FunctionType::toString() {

}

//NamedType::NamedType(std::string new_name, Type *new_type) {
//    name = new_name;
//    type = new_type;
//}
//
//bool NamedType::canConvertToThisType(const Type *other) const {
//
//}
//
//bool NamedType::compareSignatures(const Type *other) const {
//
//}