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

    virtual std::string toString() = 0;

private:
};

class IntType : public Type {
public:

    IntType(int new_bits = 32) : bits(new_bits) {}

    bool canConvertToThisType(const Type *other) const override;

    std::string toString() override;

private:
    int bits;
};

class BoolType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

    std::string toString() override;

private:
};

class FloatType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

    std::string toString() override;

private:
};

class StructType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

    bool compareSignatures(const Type *other) const override;

    void addNewField(std::string, Type *) ;

    Type *getField(std::string) const;

    Type** getDoubleLinkToField(std::string);

    bool nameAlreadyExists(std::string);

    std::string toString() override;

private:
    std::vector<std::pair<std::string, Type *>> fields;

};

class PointerType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

    PointerType(Type *new_base) : base_type(new_base) {}

    Type *getBase();

    std::string toString() override;

private:
    Type *base_type;
};

class FunctionType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

    bool compareSignatures(const Type *other) const override;

    bool compareArgs(const std::vector<Type *> &);

    std::vector<Type* > getArgs();

    Type *getReturn();

    void setReturn(Type *);

    void addParam(Type *);

    std::string toString() override;

private:

    std::vector<Type *> args;

    Type *return_type;
};

//class NamedType : public Type {
//public:
//
//    NamedType(std::string, Type*);
//
//    bool canConvertToThisType(const Type *other) const override;
//
//    bool compareSignatures(const Type *other) const override;
//
//private:
//    std::string name;
//    Type *type;
//};


//// this class created only for times, when func returns more than 1 type at the same time
class SeqType : public Type {
public:

    StructType* corespStruct;

    bool canConvertToThisType(const Type *other) const override;

    bool compareSignatures(const Type *other) const override;

    void addType(Type *);

    std::vector<Type *> getTypes();

    std::string toString() override;

private:

    std::vector<Type *> types;
};

#endif //COMPILER_TYPES_H
