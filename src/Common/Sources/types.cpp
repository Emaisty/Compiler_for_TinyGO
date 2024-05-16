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

long long IntType::size() {
    if (bits <= 32)
        return 1;
    return 2;
}

bool BoolType::canConvertToThisType(const Type *other) const {
    if (this == other)
        return true;
    return false;
}

std::string BoolType::toString() {
    return "i1";
}

long long BoolType::size() {
    return 1;
}

bool FloatType::canConvertToThisType(const Type *other) const {
    if (dynamic_cast<const IntType *>(other) || this == other)
        return true;
    return false;
}

std::string FloatType::toString() {
    return "float";
}

long long FloatType::size() {
    return 1;
}

bool StructType::canConvertToThisType(const Type *other) const {
    if (!dynamic_cast<const StructType *>(other))
        return false;
    auto another_struct = dynamic_cast<const StructType *>(other);

    if (this->fields.size() != another_struct->fields.size())
        return false;

    for (auto i = 0; i < fields.size();++i) {
        if (fields[i].first != another_struct->fields[i].first ||
            fields[i].second != another_struct->fields[i].second)
            return false;
    }

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

int StructType::getFieldOrder(std::string name) const{
    for (int i = 0; i < fields.size(); ++i)
        if (name == fields[i].first)
            return i;
    return -1;
}

std::vector<std::pair<std::string, Type *>> StructType::getFields(){
    std::vector<std::pair<std::string, Type *>> res;
    for (auto &i : fields)
        if (!dynamic_cast<FunctionType*>(i.second))
            res.emplace_back(i);
    return res;
}

Type **StructType::getDoubleLinkToField(std::string name) {
    for (auto &i: fields)
        if (i.first == name)
            return &i.second;
    return nullptr;
}

bool StructType::nameAlreadyExists(std::string name) {
    for (auto &[i,_]: fields)
        if (i == name)
            return true;
    return false;
}

std::string StructType::toString() {
    std::string res = "{";
    for (unsigned long long i = 0; i < fields.size(); ++i){
        if (dynamic_cast<FunctionType*>(fields[i].second))
            continue;
        if (i != 0)
            res += ", ";
        res += fields[i].second->toString();
    }
    res += "}";
    return res;
}

long long StructType::size() {
    long long res = 0;
    for (auto &[_,i] : fields)
        res += i->size();
    return res;
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

long long PointerType::size() {
    return 1;
}

bool FunctionType::canConvertToThisType(const Type *other) const {
    return false;
}

bool FunctionType::compareSignatures(const Type *other) const {
    if (!dynamic_cast<const FunctionType *>(other))
        return false;
    auto other_func = dynamic_cast<const FunctionType *>(other);

    if (inner_name_of_method != other_func->inner_name_of_method)
        return false;

    if (is_method_pointer != other_func->is_method_pointer)
        return false;

    if ((other_func->return_type && !return_type) || (!other_func->return_type && return_type))
        return false;

    if (return_type && !return_type->canConvertToThisType(other_func->return_type))
        return false;

    if (return_as_arg != other_func->return_as_arg)
        return false;

    if (type_of_return_arg != other_func->type_of_return_arg)
        return false;

    if (args.size() != other_func->args.size())
        return false;
    for (auto i = 0; i < args.size(); ++i)
        if (args[i] != other_func->args[i])
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

std::vector<Type* > FunctionType::getArgs(){
    return args;
}

Type *FunctionType::getReturn() {
    return return_type;
}

void FunctionType::setReturn(Type *new_type) {
    return_type = new_type;
}

void FunctionType::setInnerName(std::string new_name){
    inner_name_of_method = new_name;
}

void FunctionType::setIsPointer(bool flag){
    is_method_pointer = flag;
}

std::string FunctionType::innerName(){
    return inner_name_of_method;
}

void FunctionType::setReturnArg(std::string name) {
    return_as_arg = name;
}

void FunctionType::setReturnArgType(StructType* type) {
    type_of_return_arg = type;
}

std::string FunctionType::getReturnArg() {
    return return_as_arg;
}

StructType* FunctionType::getReturnArgType(){
    return type_of_return_arg;
}

bool FunctionType::isPointer(){
    return is_method_pointer;
}

void FunctionType::addParam(Type *new_type) {
    args.emplace_back(new_type);
}

std::string FunctionType::toString() {
    return "ERROR. Function type called to string";
}

long long FunctionType::size() {
    return 0;
}

bool SeqType::canConvertToThisType(const Type *other) const{
    return false;
}

bool SeqType::compareSignatures(const Type *other) const{
    auto another_seq = dynamic_cast<const SeqType*>(other);
    if (!another_seq)
        return false;
    if (types.size() != another_seq->types.size())
        return false;
    for (unsigned long long i = 0; i < types.size(); ++i)
        if (types[i] != another_seq->types[i])
            return false;

    return true;
}

void SeqType::addType(Type * new_type){
    types.emplace_back(new_type);
}

std::vector<Type *> SeqType::getTypes(){
    return types;
}

std::string SeqType::toString(){
    std::string res = "{";
    for (unsigned long long i = 0; i < types.size(); ++i){
        res += types[i]->toString();
        if (i != types.size() - 1)
            res += ", ";
    }
    res += "}";
    return res;
}

long long SeqType::size() {
    return 0;
}