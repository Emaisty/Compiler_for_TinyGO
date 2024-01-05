#ifndef COMPILER_TYPES_H
#define COMPILER_TYPES_H

class Type {
public:
    bool compare(const Type *other) const;

private:
};

class SimpleType : public Type {

};

class StructType : public Type {

};

class PointerType : public Type {

};

class FunctionType : public Type {

};

#endif //COMPILER_TYPES_H
