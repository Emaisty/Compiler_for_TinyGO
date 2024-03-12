#include "types.h"

bool Type::compareSignatures(const Type *other) const {
    return this == other;
}

bool IntType::canConvertToThisType(const Type *other) const {
    if (this == other || dynamic_cast<const FloatType *>(other))
        return true;
    return false;
}

bool BoolType::canConvertToThisType(const Type *other) const {
    if (dynamic_cast<const BoolType *>(other))
        return true;
    return false;
}

bool FloatType::canConvertToThisType(const Type *other) const {
    if (dynamic_cast<const IntType *>(other) || this == other)
        return true;
    return false;
}

bool UnNamedStruct::canConvertToThisType(const Type *other) const {
    if (this == other || (dynamic_cast<const NamedStructure *>(other) &&
                          dynamic_cast<const NamedStructure *>(other)->base_struct == other))
        return true;

    return false;
}

bool UnNamedStruct::compareSignatures(const Type *other) const {
    //TODO comp
}

void UnNamedStruct::addNewField(std::string new_name, Type *new_type) {
    if (nameAlreadyExists(new_name))
        throw std::invalid_argument("ERROR. Field with such name already exists.");

    fields.emplace_back(new_name, new_type);
}

Type *UnNamedStruct::getField(std::string name) const {
    for (auto &i: fields)
        if (i.first == name)
            return i.second;

    return nullptr;
}

bool UnNamedStruct::nameAlreadyExists(std::string name) {
    for (auto &i: fields)
        if (i.first == name)
            return true;
    return false;
}

bool NamedStructure::canConvertToThisType(const Type *other) const {
    if (this == other || base_struct == other)
        return true;
    return false;
}

void NamedStructure::addNewField(std::string new_name, Type *new_type) {
    if (base_struct->nameAlreadyExists(new_name))
        throw std::invalid_argument("ERROR. Field with such name already exists.");
    for (auto &i: methods)
        if (i.first == name)
            throw std::invalid_argument("ERROR. Method with such name already exists.");

    methods.emplace_back(new_name, new_type);

}

Type *NamedStructure::getField(std::string name) const {
    if (base_struct->getField(name))
        return base_struct->getField(name);

    for (auto &i: methods)
        if (i.first == name)
            return i.second;

    return nullptr;
}


bool PointerType::canConvertToThisType(const Type *other) const {
    if (dynamic_cast<const PointerType *>(other) && dynamic_cast<const PointerType *>(other)->base_type == base_type)
        return true;
    return false;
}

bool FunctionType::canConvertToThisType(const Type *other) const {
    //TODO IDK
}

bool FunctionType::compareSignatures(const Type *other) const {

}

bool FunctionType::compareArgs(const std::vector<Type *> &) {

}

Type *FunctionType::getReturn() {

}

void FunctionType::setReturn(Type *) {

}

void FunctionType::addParam(Type *) {

}

