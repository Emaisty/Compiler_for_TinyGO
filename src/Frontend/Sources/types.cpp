#include "types.h"

bool Type::compare(const Type *other) const {
    return this == other;
}

bool Type::compareSignatures(const Type *other) const {
    return this == other;
}


std::unique_ptr<Type> SimpleType::makeUnique() {
    return std::move(std::make_unique<SimpleType>(*this));
}

std::unique_ptr<Type> FloatType::makeUnique() {
    return std::move(std::make_unique<FloatType>(*this));
}

std::unique_ptr<Type> StructType::makeUnique() {
    return std::move(std::make_unique<StructType>(*this));
}

std::unique_ptr<Type> PointerType::makeUnique() {
    return std::move(std::make_unique<PointerType>(*this));
}

std::unique_ptr<Type> FunctionType::makeUnique() {
    return std::move(std::make_unique<FunctionType>(*this));
}