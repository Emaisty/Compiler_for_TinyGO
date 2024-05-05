#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <iostream>
#include <fstream>
#include <string>


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
};

#endif //COMPILER_LEXER_H
