#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>


/*
 * Lexer returns tokens [0-255] if it is an unknown character, otherwise one of these for known things.
 * Here are all valid tokens:
 */
enum Token {
    tok_eof,   //0

    // keywords
    tok_break,
    tok_default,
    tok_func,
    tok_case,
    tok_struct, // 5
    tok_else,
    tok_package,
    tok_switch,
    tok_const,
    tok_if,
    tok_type,    // 11
    tok_continue,
    tok_for,
    tok_return,
    tok_var,   //15


    // numbers and identifiers
    tok_identifier,
    tok_int8,
    tok_int32,
    tok_int,
    tok_int64,   // 20
    tok_float,
    tok_bool,

    //
    tok_num_int,
    tok_num_float, //24

    //
    tok_semicolon,
    tok_newline,

    tok_ge,
    tok_gt,
    tok_le,
    tok_lt,
    tok_eq,  //31
    tok_ne,

    tok_and,
    tok_or,
    tok_binand,
    tok_binor,

    tok_assign,
    tok_plusassign,
    tok_minassign,
    tok_mulassign,
    tok_divassign,    //41
    tok_modassign,
    tok_fastassign,
    tok_colon,
    tok_plus,
    tok_minus,
    tok_div,
    tok_asterisk,     //48
    tok_mod,
    tok_inc,
    tok_dec,

    tok_opfigbr,
    tok_clfigbr,
    tok_opbr,
    tok_clbr,
    tok_opsqrbr,
    tok_clsqrbr,        //57

    tok_comma,
    tok_dot,

    tok_excl,


    tok_true,
    tok_false,




    tok_scan,
    tok_print
};


const inline std::map<Token,std::string> tokens_to_string = {
        {tok_eof,"end of file"},
        {tok_break,"break"},
        {tok_default,"default"},
        {tok_func,"func"},
        {tok_case,"case"},
        {tok_struct,"struct"},
        {tok_else,"else"},
        {tok_package,"package"},
        {tok_switch,"switch"},
        {tok_const,"const"},
        {tok_if,"if"},
        {tok_type,"type"},
        {tok_continue,"continue"},
        {tok_for,"for"},
        {tok_return,"return"},
        {tok_var,"var"},
        {tok_identifier,"identifier"},
        {tok_int8,"int"},
        {tok_int32,"int"},
        {tok_int,"int"},
        {tok_int64,"int"},
        {tok_float,"float"},
        {tok_bool,"bool"},
        {tok_num_int,"integer number"},
        {tok_num_float,"float number"},
        {tok_semicolon,"semicolon (;)"},
        {tok_newline," new line(\\n)"},
        {tok_ge,"greater or equal (>=)"},
        {tok_gt,"greater than (>)"},
        {tok_le,"less or  equal (<=)"},
        {tok_lt,"less then (<)"},
        {tok_eq,"equal (==)"},
        {tok_ne,"not equal (!=)"},
        {tok_and,"logic and (&&)"},
        {tok_or,"logic or (||)"},
        {tok_binand,"binary and (&)"},
        {tok_binor,"binary or (|)"},
        {tok_assign,"assign sign (=)"},
        {tok_plusassign,"plus value and assign (+=)"},
        {tok_minassign,"minus value and assign (-=)"},
        {tok_mulassign,"mult value and assign (*=)"},
        {tok_divassign,"div value and assign (/=)"},
        {tok_modassign,"modulus value and assign (%=)"},
        {tok_fastassign,"declare var from a value (:=)"},
        {tok_colon,"colon (:)"},
        {tok_plus,"plus (+)"},
        {tok_minus,"minus (-)"},
        {tok_div,"divide (/)"},
        {tok_asterisk,"asterisk (*)"},
        {tok_mod,"modulus (%)"},
        {tok_inc,"increment (++)"},
        {tok_dec,"decrement (--)"},
        {tok_opfigbr,"open figure bracket ({)"},
        {tok_clfigbr,"close figure bracket (})"},
        {tok_opbr,"open bracket \"(\""},
        {tok_clbr,"close bracket \")\""},
        {tok_opsqrbr,"open square bracket ([)"},
        {tok_clsqrbr,"close square bracket (])"},
        {tok_comma,"comma (,)"},
        {tok_dot,"dot (.)"},
        {tok_excl,"!"},
        {tok_true,"true"},
        {tok_false,"false"},
        {tok_scan,"scan"},
        {tok_print,"print"},
};

// type of input chars
enum InputCharType {
    LETTER, NUMBER, WHITE_SPACE, NEW_LINE, END, NO_TYPE, SPE_SYMB
};

class Lexer {
public:
    Lexer() = default;

    ~Lexer() = default;

    Token gettok();

    const std::string &identifierStr() const { return this->m_IdentifierStr; }

    int numVal() { return this->m_NumVal; }

    double douVal() { return this->m_DouVal; }

    int getLineNumber() { return this->line_number; }

    void InitInput(std::string);

    char inputSymbol();

    InputCharType type_of_char();

    Token readString();

    Token readNumber();

    Token readSymbol();

    void skipOptional_();

    bool checkIfCanBe(int cur_symb, int base);

    void inputNumber(int base, bool mandatoryToBe = true);

private:
    std::ifstream file;
    char open = 0;

    int cur_symb = -1;

    std::string m_IdentifierStr;
    long long m_NumVal;
    double m_DouVal;

    int line_number = 1;

    int if_new_line_appear = 0;
};

#endif //COMPILER_LEXER_H
