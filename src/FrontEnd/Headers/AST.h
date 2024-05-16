#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <vector>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <string>
#include <memory>
#include <algorithm>

#include "types.h"
#include "IR.h"


namespace AST {

    /**
     * AST Node for the types
     * It is a semantic representation of the any node, that contains type
     * Does not represents type by itself. Type systems is implemented in types.h
     */
    class ASTType;

    /**
     * AST Node for expressions
     * Can be calculated and executed
     */
    class ASTExpression;


    struct ItemInNameSpace {
        ItemInNameSpace(Type *new_type = nullptr, bool new_is_const = false) :
                type(new_type), is_const(new_is_const) {}

        Type *type;
        bool is_const;
    };

    /**
     * Info space for the type checker
     * Contains all the info about types, variables, functions, etc.
     * Each node, when traverse by the type checker pass it as an argument
     */
    struct Context {
        Context();

        // if current command in the loop or not
        bool in_loop = false;

        // if current command in the switch ot not
        bool in_switch = false;

        // if right now we are checking the global variables and functions
        // uses to not raise an error, if we have a pointer to a structure,
        // which is not implemented yet
        bool GlobalInit = true;

        // if we are inside the function or not
        bool body_of_function = false;

        // saves the types, which return statement should return
        std::vector<Type *> return_type;

        // helps to name the tmp variables with the different names
        unsigned long long tmp_count = 0;

        // checks if the variable with such name exists in the nearest scope
        bool checkIfNameExist(std::string);

        // checks if the named type with such name exists or not
        bool checkIfTypeExist(std::string);

        // Go into loop/switch and increase the scope
        void goDeeper(bool, bool);

        // Go up from the scope
        void goUp();

        Type *getTypeByTypeName(std::string);

        ItemInNameSpace *getInfByVarName(std::string);

        // add the new type into the type space. If type with exact same signature exists
        // deletes the new one and returns the old one
        Type *addType(std::unique_ptr<Type> &&);

        // add named type
        Type *addAliasType(std::string, Type *);

        // adds the variable into the space of them.
        // accepts name, type and if it is const or not
        void addIntoNameSpace(std::string, Type *, bool);

        // get the pointer type to the given type
        Type *getPointer(Type *);

        // saves type node, which cannot be implemented yet for later
        Type *addForLater(std::unique_ptr<Type> &&);

        // if filed of the structure cannot be completed yet, and it is global decl
        // add it for later
        void addFieldFillInLater(ASTType *, Type **);

        // fill the types, which was left for later
        void fillUpEmptyField();

        void transFromTmpTypes();

        inline static std::vector<std::string> base_types = {"int8", "int32", "int", "int64", "bool", "float"};

        // return does the type if int or not
        bool isInt(const Type *);

        // accepts two int types. Returns the greater one
        Type *greaterInt(const Type *, const Type *);

        // return does the type if float or not
        bool isFloat(const Type *);

        // return does the type if bool or not
        bool isBool(const Type *);

        // if the given type is not the exact the same as to be needed -- add the cast node
        std::unique_ptr<ASTExpression> convertTypeTo(std::unique_ptr<ASTExpression> &&, Type *);

        // accepts the two nodes. Find, to which type one of them should be casted and adds above it cast node
        std::pair<std::unique_ptr<ASTExpression>, std::unique_ptr<ASTExpression>>
        convertTypesEq(std::unique_ptr<ASTExpression> &&, std::unique_ptr<ASTExpression> &&);

        // which type have a greater priority
        // float => int64 => int32 => int16 => int1
        bool typeGreater(Type *, Type *);

        // Creates a context space for the IR generation
        IR::Context createIRContext();

    private:

        // exist unique types
        std::vector<std::unique_ptr<Type>> existItems;

        // variables of the program
        std::vector<std::map<std::string, ItemInNameSpace>> nameSpace;

        // types of the program
        std::vector<std::map<std::string, Type *>> typeSpace;

        // space of the pointers
        std::map<Type *, PointerType *> pointers;

        // status of the inside loops and switches in the program
        std::stack<bool> pr_loop_status;
        std::stack<bool> pr_switch_status;

        // vector for not finished types.
        // Such types, which cannot have its field yet, because it havent checked it yet
        std::vector<std::unique_ptr<Type>> notYetFinishedTypes;

        // not finished fields of the strutures
        std::vector<std::pair<ASTType *, Type **>> notYetFinishedFields;

        // links from the non yet finished types to where it is used
        std::map<Type *, std::vector<Type **>> linksToType;

    };

/**
 * Root node of all AST
 * Represents any node in the tree
 */
    class ASTNode {
    public:
        ASTNode() = default;

        virtual ~ASTNode() = default;

        // needs for the debugger and for break pointer
        void addLineNumber(int line_num);

        // does node is a l-value and have address
        virtual bool hasAddress();

        // does node is a constant
        virtual bool isConst();

        // checks the type of the node and its children
        virtual Type *checker(Context &) = 0;

        // type, which node returns
        Type *typeOfNode = nullptr;

        // generate the corespond IR lines to this node
        virtual IR::Value * generateIR(IR::Context &) = 0;

    private:
        int line;
    };

    class ASTType : public ASTNode {
    public:

        // gets the set of the names, which must be decl before this type
        virtual std::set<std::string> getDependencies() = 0;

        IR::Value * generateIR(IR::Context &) override;


    protected:
    };


    class ASTTypePointer : public ASTType {
    public:

        ASTTypePointer() = default;

        ASTTypePointer(std::unique_ptr<ASTType> &&new_type);

        ASTType *getValue();

        Type *checker(Context &) override;

        std::set<std::string> getDependencies() override;

    private:
        std::unique_ptr<ASTType> type;
    };

    class ASTTypeStruct : public ASTType {
    public:

        ASTTypeStruct() = default;

        void addField(std::vector<std::string> &&, std::unique_ptr<ASTType> &&);

        ASTType *findField(std::string name) const;

        Type *checker(Context &) override;

        std::set<std::string> getDependencies() override;

    private:
        std::vector<std::pair<std::vector<std::string>, std::unique_ptr<ASTType >>> fileds;
    };

    /**
     * Any type, which is refered by the name
     */
    class ASTTypeNamed : public ASTType {
    public:
        ASTTypeNamed(std::string new_name) : name(new_name) {};

        Type *checker(Context &) override;

        std::set<std::string> getDependencies() override;

    private:
        std::string name;
    };

    /**
     * Class of the node, which can be a statement in the programm
     * (can be by itself and can be in a function body)
     */
    class Statement : public ASTNode {
    public:

        Statement() = default;

        virtual ~Statement() = default;

    private:

    };

    class ASTExpression : public Statement {
    public:

        // gets the set of the names, which must be decl before this variable
        virtual std::set<std::string> getVarNames() = 0;

    private:
    };

    /**
     * Binary operations
     * accepts 2 children and an operator of the operation
     */
    class ASTBinaryOperator : public ASTExpression {
    public:

        ASTBinaryOperator(std::unique_ptr<ASTExpression> &&, std::unique_ptr<ASTExpression> &&,
                          IR::IRArithOp::Operator new_op = IR::IRArithOp::Operator::PLUS);

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

        IR::Value * generateIR(IR::Context &) override;


    private:
        std::unique_ptr<ASTExpression> left, right;
        IR::IRArithOp::Operator op;
    };

    /**
     * Unary operation
     * Has a one child and an operator of the operation
     */
    class ASTUnaryOperator : public ASTExpression {
    public:
        enum Operator {
            NOT, PLUS, MINUS, PREINC, PREDEC, POSTINC, POSTDEC, REFER, DEREFER
        };

        ASTUnaryOperator(std::unique_ptr<ASTExpression> &&, Operator new_op = PLUS);

        bool hasAddress() override;

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

        IR::Value * generateIR(IR::Context &) override;


    private:
        Operator op;
        std::unique_ptr<ASTExpression> value;
    };

    /**
     * Function call
     * Has a left side -- some how a caller of the function (can be a name of the function, or method call)
     * on the right side -- vector of arguemtns
     */
    class ASTFunctionCall : public ASTExpression {
    public:

        ASTFunctionCall(std::unique_ptr<ASTExpression> &&, std::vector<std::unique_ptr<ASTExpression>> &);

        void setArgs(std::vector<std::unique_ptr<ASTExpression>>&&);

        std::vector<std::unique_ptr<ASTExpression>> resetArgs();

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

        IR::Value * generateIR(IR::Context &) override;


    private:
        std::unique_ptr<ASTExpression> name;

        std::vector<std::unique_ptr<ASTExpression>> arg;

        // if function is going to have more than 1 return type -- it is going to return it as structure, and return
        // is going to be made by argument
        // name for such return argument
        std::string name_for_return_arg;

        // type of the return argument
        StructType* type_for_return_arg;
    };


    /**
     * Caller of the field of the structure
     * have an address of the structure of the left side and a member name of the right
     */
    class ASTMemberAccess : public ASTExpression {
    public:

        ASTMemberAccess(std::unique_ptr<ASTExpression> &&, std::string );

        bool hasAddress() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

        IR::Value * generateIR(IR::Context &) override;


    private:
        std::unique_ptr<ASTExpression> name;

        std::string member;
    };


    class ASTIntNumber : public ASTExpression {
    public:

        ASTIntNumber(const int new_value = 0);

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

        IR::Value * generateIR(IR::Context &) override;


    private:
        long long int value = 0;
    };

    class ASTFloatNumber : public ASTExpression {
    public:

        ASTFloatNumber(double new_value = 0);

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

        IR::Value * generateIR(IR::Context &) override;


    private:
        double value = 0;
    };

    class ASTBoolNumber : public ASTExpression {
    public:

        ASTBoolNumber(const bool new_value = false);

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

        IR::Value * generateIR(IR::Context &) override;


    private:
        bool value = false;
    };

    class ASTStruct : public ASTExpression {
    public:

        ASTStruct(std::unique_ptr<ASTTypeStruct> &&,
                  std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> &);

        bool isConst() override;

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

        IR::Value * generateIR(IR::Context &) override;


    private:
        // type of the structure
        std::unique_ptr<ASTType> type;

        // set of the values for the structure fields
        std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>> values;

    };

    class ASTVar : public ASTExpression {
    public:

        ASTVar(const std::string new_name);

        bool hasAddress() override;

        bool isConst() override;

        std::string getName();

        Type *checker(Context &) override;

        std::set<std::string> getVarNames() override;

        IR::Value * generateIR(IR::Context &) override;


    private:
        std::string name;

        bool is_const = false;
    };

    /**
     * Class of any declaration
     * Has a name of declared item, and type or/and value of it
     * Might have more than one declaration inside of itself
     */
    class ASTDeclaration;

    /**
     * Item for the dispached declaration.
     * Represent the same as declaration, but it is garanteed, that is has only one declaration
     */
    struct dispatchedDecl {
        //name of declared
        std::string name;

        //link from which this declaration is dispached
        ASTDeclaration *decl;

        // set of the names, on which this declaration is depended
        std::set<std::string> depends;

        // expression of the declaration. Might be empty
        ASTExpression *expr;

        // type of the declaration. Might be empty
        ASTType *type;

        // does this variable const or not
        bool const_var;

        dispatchedDecl(std::string new_name = "", ASTDeclaration *new_decl = nullptr,
                       std::set<std::string> &&depend = {}, ASTExpression *new_expr = nullptr,
                       ASTType *new_type = nullptr, bool is_const = false) : name(
                new_name), decl(new_decl), depends(depend), expr(new_expr), type(new_type), const_var(is_const) {}

        void declareVars(Context &);
    };


    class ASTDeclaration : public Statement {
    public:

        ASTDeclaration(std::vector<std::string> &&new_name, std::vector<std::unique_ptr<ASTExpression>> &&,
                       std::unique_ptr<ASTType> &&);

        ASTDeclaration(std::vector<std::string> &&new_name, std::vector<std::unique_ptr<ASTExpression>> &&);

        ASTDeclaration(std::vector<std::string> &&new_name, std::unique_ptr<ASTType> &&);

        virtual std::vector<dispatchedDecl> globalPreInit() = 0;

        // It used, then declaration created by the program itself and servers to accept structure type from a function
        // and after that assign it to each individuals
        bool dispatcher = false;

    protected:
        std::vector<std::string> name;
        std::vector<std::unique_ptr<ASTExpression>> value;
        std::unique_ptr<ASTType> type;

        // In case, that there is a multiple variables on the left side and single function
        // on the right side, with multiple returns -- make a new declaration, where a tmp structure
        // passed to the function and then values is going to be a members of it
        std::vector<std::unique_ptr<ASTDeclaration>> dispatchedDeclarations;

    };


    /**
     * Represents the declaration of the named type.
     * Always name.size() == 1 and have a type. type.size() == 1
     * value.size() == 0
     */
    class ASTTypeDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        std::vector<dispatchedDecl> globalPreInit() override;

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

    private:
    };

    /**
     * Declaration of the variable
     * type should exists, or values should be or both
     */
    class ASTVarDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        std::vector<dispatchedDecl> globalPreInit() override;

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

    private:
        std::unique_ptr<ASTVarDeclaration> function_dispatch;

    };


/**
 * Declaration of the constant variable
 * value.size() == name.size()
 * type might not be declared
 */
    class ASTConstDeclaration : public ASTDeclaration {
    public:
        using ASTDeclaration::ASTDeclaration;

        std::vector<dispatchedDecl> globalPreInit() override;

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

    private:
    };

    class ASTBlock : public Statement {
    public:

        ASTBlock() = default;

        void addStatement(std::unique_ptr<AST::Statement> &&);

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

    private:
        std::vector<std::unique_ptr<AST::Statement>> statements;
    };

    class ASTBreak : public Statement {
    public:

        ASTBreak() = default;

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

    private:

    };

    class ASTContinue : public Statement {
    public:

        ASTContinue() = default;

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

    private:

    };

    class ASTReturn : public Statement {
    public:

        ASTReturn() = default;

        void addReturnValue(std::unique_ptr<ASTExpression> &&);

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

    private:
        // might have a multiple returns
        // in that case, it will be converted to a structure later
        std::vector<std::unique_ptr<AST::ASTExpression>> return_value;

     };

    class ASTSwitch : public Statement {
    public:

        ASTSwitch() = default;

        void addExpr(std::unique_ptr<ASTExpression> &&);

        void addCase(std::unique_ptr<ASTExpression> &&, std::unique_ptr<AST::ASTBlock> &&);

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

    private:
        std::unique_ptr<ASTExpression> expr;

        std::vector<std::pair<std::unique_ptr<AST::ASTExpression>, std::unique_ptr<AST::Statement>>> cases;
    };


    class ASTIf : public Statement {
    public:

        ASTIf() = default;

        void addExpr(std::unique_ptr<AST::ASTExpression> &&);

        void addIfClause(std::unique_ptr<AST::ASTBlock> &&);

        void addElseClause(std::unique_ptr<AST::ASTBlock> &&);

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

    private:
        std::unique_ptr<AST::ASTExpression> expr;

        std::unique_ptr<AST::Statement> if_clause, else_clause;
    };

    class ASTFor : public Statement {
    public:

        ASTFor() = default;

        void addInitClause(std::vector<std::unique_ptr<AST::Statement>> &&);

        void addIterClause(std::vector<std::unique_ptr<AST::Statement>> &&);

        void addCondClause(std::unique_ptr<AST::ASTExpression> &&);

        void addBody(std::unique_ptr<AST::ASTBlock> &&);

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

    private:
        std::vector<std::unique_ptr<AST::Statement>> init_clause, iterate_clause;

        std::unique_ptr<AST::ASTExpression> if_clause;

        std::unique_ptr<AST::Statement> body;
    };

    class ASTAssign : public Statement {
    public:
        enum TypeOfAssign {
            ASSIGN, PLUSASSIGN, MINUSASSIGN, MULTASSIGN, DIVASSIGN, MODASSIGN
        };

        ASTAssign() = default;

        ASTAssign(std::vector<std::unique_ptr<AST::ASTExpression>> &&,
                  std::vector<std::unique_ptr<AST::ASTExpression>> &&,
                  TypeOfAssign new_type);

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

        // It used then assign it a part of a dispatch variable and it does not need to be assigned.
        bool dispatcher = false;

    private:
        std::vector<std::unique_ptr<AST::ASTExpression>> variable, value;

        TypeOfAssign type;

        // In case, that there is a multiple variables on the left side and single function
        // on the right side, with multiple returns -- make a new declaration, where a tmp structure
        // passed to the function and then values is going to be a members of it
        std::unique_ptr<ASTVarDeclaration> function_dispatch;

    };

    /**
     * Scan_char. Takes as an arguments a pointer to a integer and puts scans a char into it from a stream
     */
    class ASTScan : public Statement {
    public:
        ASTScan() = default;

        void addExpression(std::unique_ptr<AST::ASTExpression>&&);

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;
    private:
        std::unique_ptr<AST::ASTExpression> expression;

    };
/**
 * Prints a value to the terminal
 */
    class ASTPrint : public Statement {
    public:
        ASTPrint() = default;

        void addExpression(std::unique_ptr<AST::ASTExpression>&&);

        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;
    private:
        std::unique_ptr<AST::ASTExpression> expression;

    };


    class Function : public ASTNode {
    public:
        void setName(std::string new_name);

        void addParam(std::vector<std::string> &&, std::unique_ptr<ASTType> &&);

        std::vector<Type*> getListOfArgTypes();

        void setMethod(std::string, std::unique_ptr<ASTType> &&);

        void addReturn(std::unique_ptr<AST::ASTType> &&);

        void setBody(std::unique_ptr<AST::ASTBlock> &&);

        Type *checker(Context &) override;

        void globalPreInit(Context &);

        IR::Value * generateIR(IR::Context &) override;

    private:
        std::string name;

        std::vector<std::pair<std::vector<std::string>, std::unique_ptr<ASTType>>> params;

        std::vector<std::unique_ptr<AST::ASTType>> return_type;

        std::unique_ptr<AST::Statement> body;

        // variables for inner state and use.
        // name for the arguments, which plays the role of the returns
        std::string name_for_return;

        // type of the return arguments
        Type* type_for_return_arg;

        // list of has been arguments modified.
        // all structures, that are passed by the values is changes to accepts it by the reference
        // caller will create a copy and will pass a pointer to in
        std::vector<bool> was_arg_modified;

        // type of the method, if function is a method
        std::unique_ptr<ASTType> type_of_method;

        // name of the reference to the inner values of the class, while it is a method
        std::string inner_name;

    };


    class Program : public ASTNode {
    public:

        Program() = default;

        ~Program() = default;

        void setName(std::string new_name);

        void addVarDecl(std::unique_ptr<ASTDeclaration> &&);

        void addTypeDecl(std::unique_ptr<ASTDeclaration> &&);

        void addFunction(std::unique_ptr<Function> &&);

        Type *checker(Context &) override;

        std::queue<dispatchedDecl> topSort(std::vector<dispatchedDecl>);

        IR::Value * generateIR(IR::Context &) override;

    private:
        std::string name;

        std::vector<std::unique_ptr<ASTDeclaration>> typeDeclarations;

        std::vector<std::unique_ptr<ASTDeclaration>> varDeclarations;

        std::vector<std::unique_ptr<Function>> functions;

    };


    class ASTCast : public ASTExpression {
    public:
        Type *checker(Context &) override;

        IR::Value * generateIR(IR::Context &) override;

        std::set<std::string> getVarNames() override;


        void setChild(std::unique_ptr<AST::ASTExpression> &&);

        void setTypeCastTo(Type *);

    private:
        std::unique_ptr<AST::ASTExpression> expr;
        Type *cast_to;
    };

}

#endif //COMPILER_AST_H
