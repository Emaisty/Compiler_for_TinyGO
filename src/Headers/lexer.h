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

    //
    tok_num_int,
    tok_num_float, //23

    //
    tok_semicolon,
    tok_newline,

    tok_ge,
    tok_gt,
    tok_le,
    tok_lt,
    tok_eq,  //30
    tok_ne,

    tok_and,
    tok_or,
    tok_binand,
    tok_binor,

    tok_assign,
    tok_plusassign,
    tok_minassign,
    tok_mulassign,
    tok_divassign,    //40
    tok_modassign,
    tok_fastassign,
    tok_colon,
    tok_plus,
    tok_minus,
    tok_div,
    tok_asterisk,     //47
    tok_mod,
    tok_incr,
    tok_decr,

    tok_opfigbr,
    tok_clfigbr,
    tok_opbr,
    tok_clbr,
    tok_opsqrbr,
    tok_clsqrbr,        //56

    tok_comma,
    tok_dot,

    tok_excl
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

    Token gettokWhithOutIgnoreNewLine();

    const std::string &identifierStr() const { return this->m_IdentifierStr; }

    int numVal() { return this->m_NumVal; }

    double douVal() { return this->m_DouVal; }

    void InitInput(char *name);

    char inputSymbol();

    InputCharType type_of_char();

    Token readString();

    Token readNumber();

    Token readSymbol();

    void skipOptional_();

    bool checkIfCanBe(int cur_symb, int base);

    void inputNumber(int base, bool mandatoryToBe = true);

    bool haveBNL();

    void decrease();

private:
    std::ifstream file;
    char open = 0;

    int newLine = 0;

    int cur_symb = -1;

    std::string m_IdentifierStr;
    long long m_NumVal;
    double m_DouVal;
};

#endif //COMPILER_LEXER_H
