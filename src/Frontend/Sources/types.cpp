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


bool StructType::canConvertToThisType(const Type *other) const {

    // TODO
    if (this == other)
        return true;

    return false;
}

bool StructType::compareSignatures(const Type *other) const {
    if (dynamic_cast<const StructType *>(other)) {
        auto other_struct = dynamic_cast<const StructType *>(other);
        if (fields.size() != other_struct->fields.size())
            return false;

        for (auto i = 0; i < fields.size(); ++i)
            if (fields[i].first != other_struct->fields[0].first || fields[i].second != other_struct->fields[0].second)
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


bool PointerType::canConvertToThisType(const Type *other) const {
    if (dynamic_cast<const PointerType *>(other) && dynamic_cast<const PointerType *>(other)->base_type == base_type)
        return true;
    return false;
}

Type *PointerType::getBase() {
    return base_type;
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