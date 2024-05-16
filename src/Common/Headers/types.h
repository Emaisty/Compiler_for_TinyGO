#ifndef COMPILER_TYPES_H
#define COMPILER_TYPES_H

#include <memory>
#include <vector>
#include <string>


/**
 * Abstract class of the inner class system
 */
class Type {
public:

    // checks, could convert other to "this" Type
    // uses in a typechecker, confirming that it is possible to use the operations
    // above the both types
    virtual bool canConvertToThisType(const Type *other) const = 0;

    // requires to not duplicate structures.
    // uses by the AST::Context, when a new type is coming.
    // it compares type, to be exact the same
    virtual bool compareSignatures(const Type *other) const;

    virtual std::string toString() = 0;

    virtual long long size() = 0;

private:
};

class IntType : public Type {
public:

    IntType(int new_bits = 32) : bits(new_bits) {}

    bool canConvertToThisType(const Type *other) const override;

    std::string toString() override;

    long long size() override;

private:
    int bits;
};

class BoolType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

    std::string toString() override;

    long long size() override;

private:
};

class FloatType : public Type {
public:
    bool canConvertToThisType(const Type *other) const override;

    std::string toString() override;

    long long size() override;

private:

};

class StructType : public Type {
public:
    bool canConvertToThisType(const Type *other) const override;

    bool compareSignatures(const Type *other) const override;

    // adds a new field to the type
    void addNewField(std::string, Type *) ;

    // get the field by the string
    Type *getField(std::string) const;

    // returns the position of the field in the vector of all fields
    // uses in IR for the 'get member' instruction
    int getFieldOrder(std::string) const;

    // return all fields
    std::vector<std::pair<std::string, Type *>> getFields();

    // returns link to the type
    // it uses by the type checker, when type is not implemented yet
    // and will be fulfilled later
    Type** getDoubleLinkToField(std::string);

    // checks if name already exists
    bool nameAlreadyExists(std::string);

    std::string toString() override;

    long long size() override;

private:
    std::vector<std::pair<std::string, Type *>> fields;

};

class PointerType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

    PointerType(Type *new_base) : base_type(new_base) {}

    Type *getBase();

    std::string toString() override;

    long long size() override;

private:
    Type *base_type;
};

class FunctionType : public Type {
public:

    bool canConvertToThisType(const Type *other) const override;

    bool compareSignatures(const Type *other) const override;

    bool compareArgs(const std::vector<Type *> &);

    // get the arguments of the function
    std::vector<Type* > getArgs();

    Type *getReturn();

    void setReturn(Type *);

    // sets the name of the structure, by which it is going
    // to be referenced inside
    void setInnerName(std::string);

    // sets if the structure passed by pointer
    void setIsPointer(bool);

    std::string innerName();

    // if function returns structure or multiple variable
    // -- it refactors to accept the return as the argument
    // this method sets the name for it
    void setReturnArg(std::string);

    // sets the return arguments type
    void setReturnArgType(StructType*);

    std::string getReturnArg();

    StructType* getReturnArgType();

    bool isPointer();

    void addParam(Type *);

    std::string toString() override;

    long long size() override;

private:
    std::vector<Type *> args;

    Type *return_type;

    std::string inner_name_of_method;

    bool is_method_pointer = false;

    std::string return_as_arg;

    StructType* type_of_return_arg;

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


/**
 * This classed used only by the compiler type system itself
 * Represents the sequence of the types, when function returns
 * multiple of them at the same time
*/
class SeqType : public Type {
public:

    StructType* corespStruct;

    bool canConvertToThisType(const Type *other) const override;

    bool compareSignatures(const Type *other) const override;

    void addType(Type *);

    std::vector<Type *> getTypes();

    std::string toString() override;

    long long size() override;

private:

    std::vector<Type *> types;
};

#endif //COMPILER_TYPES_H
