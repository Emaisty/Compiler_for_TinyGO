#ifndef COMPILER_TYPES_H
#define COMPILER_TYPES_H

#include <memory>
#include <vector>
#include <string>

class Type {
public:

    // checks, could convert other to "this" Type
    virtual bool canConvertToThisType(const Type *other) const = 0;

    //requires to not duplicate structures
    virtual bool compareSignatures(const Type *other) const;

private:
};

class IntType : public Type {
public:

    IntType(int new_bits = 32) : bits(new_bits) {}

    bool canConvertToThisType(const Type *other) const override;

private:
    int bits;
};

class BoolType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

private:
};

class FloatType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

private:
};

class StructType : public Type {
public:

    virtual void addNewField(std::string, Type *) = 0;

    virtual Type *getField(std::string) const = 0;

private:


};

class UnNamedStruct : public StructType {
public:

    bool canConvertToThisType(const Type *other) const override;

    bool compareSignatures(const Type *other) const override;

    void addNewField(std::string, Type *) override;

    Type *getField(std::string) const override;

    bool nameAlreadyExists(std::string);

private:

    std::vector<std::pair<std::string, Type *>> fields;
};

class NamedStructure : public StructType {
public:
    NamedStructure(std::string new_name, UnNamedStruct *new_type) : name(new_name), base_struct(new_type) {}

    bool canConvertToThisType(const Type *other) const override;

    void addNewField(std::string, Type *) override;

    Type *getField(std::string) const override;

    UnNamedStruct *base_struct;

private:
    std::string name;

    std::vector<std::pair<std::string, Type *>> methods;
};


class PointerType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

    PointerType(Type *new_base) : base_type(new_base) {}

    Type *getBase();

private:
    Type *base_type;
};

class FunctionType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

    bool compareSignatures(const Type *other) const override;

    bool compareArgs(const std::vector<Type *> &);

    Type *getReturn();

    void setReturn(Type *);

    void addParam(Type *);

private:

    std::vector<Type *> args;

    Type *return_type;
};


//// this class created only for times, when func returns more than 1 type at the same time
//class SeqType : public Type {
//public:
//
//    bool canConvertToThisType(const Type *other) const override;
//
//    bool compareSignatures(const Type *other) const override;
//
//    void addType(Type*);
//
//    std::vector<Type*> getTypes();
//
//private:
//
//    std::vector<Type*> types;
//};

#endif //COMPILER_TYPES_H
