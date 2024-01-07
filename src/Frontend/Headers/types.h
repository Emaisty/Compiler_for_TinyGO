#ifndef COMPILER_TYPES_H
#define COMPILER_TYPES_H

#include <memory>

class Type {
public:
    bool compare(const Type *other) const;

    bool compareSignatures(const Type *other) const;

    virtual std::unique_ptr<Type> makeUnique() = 0;

private:
};

class SimpleType : public Type {
public:

    std::unique_ptr<Type> makeUnique() override;

private:
    int bits;
};

class FloatType : public Type {
public:

    std::unique_ptr<Type> makeUnique() override;

private:
};

class StructType : public Type {
public:
    std::unique_ptr<Type> makeUnique() override;


private:

    std::vector<std::pair<std::string, Type *>> fields;

};

class PointerType : public Type {
public:

    PointerType(Type *new_base) : base_type(new_base) {}

    std::unique_ptr<Type> makeUnique() override;

private:
    Type *base_type;
};

class FunctionType : public Type {
public:
    std::unique_ptr<Type> makeUnique() override;

private:
};

#endif //COMPILER_TYPES_H
