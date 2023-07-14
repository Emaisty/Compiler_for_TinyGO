#include "lexer.h"

const struct {
    char *slovo;
    Token symb;
} keyWordTable[] = {
        {"break",    tok_break},
        {"default",  tok_default},
        {"func",     tok_func},
        {"case",     tok_case},
        {"struct",   tok_struct},
        {"else",     tok_else},
        {"package",  tok_package},
        {"switch",   tok_switch},
        {"const",    tok_const},
        {"if",       tok_if},
        {"type",     tok_type},
        {"continue", tok_continue},
        {"for",      tok_for},
        {"return",   tok_return},
        {"var",      tok_var},

        {"int",      tok_int},
        {"int8",     tok_int8},
        {"int32",    tok_int32},
        {"int64",    tok_int64},
        {"float",    tok_float},

};

void Lexer::InitInput(char *name) {
    if (name) {
        file.open(name);
        if (file.is_open())
            open = true;
        else
            std::cout << "input from console is activated" << std::endl;
    }
    cur_symb = inputSymbol();
}

char Lexer::inputSymbol() {
    char c;
    if (!file.eof()) {
        if (open) {
            file >> std::noskipws >> c;
        } else {
            std::cin >> std::noskipws >> c;
        }
        return c;
    } else
        return 0;

}

InputCharType Lexer::type_of_char() {
    if ((cur_symb >= 'A' && cur_symb <= 'Z') || (cur_symb >= 'a' && cur_symb <= 'z'))
        return LETTER;
    else if (cur_symb >= '0' && cur_symb <= '9')
        return NUMBER;
    else if (cur_symb == EOF)
        return END;
    else if (cur_symb == '\n')
        return NEW_LINE;
    else if (cur_symb <= ' ')
        return WHITE_SPACE;
    else if ((cur_symb >= '{' && cur_symb <= '}') || cur_symb == '_' || (cur_symb >= '[' && cur_symb <= ']') ||
             (cur_symb >= ':' && cur_symb <= '?') || (cur_symb >= '!' && cur_symb <= '/'))
        return SPE_SYMB;
    return NO_TYPE;
}

Token Lexer::readSymbol() {
    char pr_symb = cur_symb;
    cur_symb = inputSymbol();
    switch (pr_symb) {
        case '.':
            return tok_dot;
        case ',':
            return tok_comma;
        case '+':
            if (cur_symb == '=') {
                cur_symb = inputSymbol();
                return tok_plusassign;
            }
            if (cur_symb == '+') {
                cur_symb = inputSymbol();
                return tok_incr;
            }
            return tok_plus;
        case '-':
            if (cur_symb == '=') {
                cur_symb = inputSymbol();
                return tok_minassign;
            }
            if (cur_symb == '-') {
                cur_symb = inputSymbol();
                return tok_decr;
            }
            return tok_minus;
        case '*':
            if (cur_symb == '=') {
                cur_symb = inputSymbol();
                return tok_mulassign;
            }
            return tok_asterisk;
        case '/':
            if (cur_symb == '=') {
                cur_symb = inputSymbol();
                return tok_divassign;
            }
            return tok_div;
        case '%':
            if (cur_symb == '=') {
                cur_symb = inputSymbol();
                return tok_modassign;
            }
            return tok_mod;
        case '&':
            if (cur_symb == '&') {
                cur_symb = inputSymbol();
                return tok_and;
            }
            return tok_binand;
        case '|':
            if (cur_symb == '|') {
                cur_symb = inputSymbol();
                return tok_or;
            }
            return tok_binor;
        case '(':
            return tok_opbr;
        case ')':
            return tok_clbr;
        case '[':
            return tok_opsqrbr;
        case ']':
            return tok_clsqrbr;
        case '{':
            return tok_opfigbr;
        case '}':
            return tok_clfigbr;
        case '=':
            if (cur_symb == '=') {
                cur_symb = inputSymbol();
                return tok_eq;
            }
            return tok_assign;
        case '>':
            if (cur_symb == '=') {
                cur_symb = inputSymbol();
                return tok_ge;
            }
            return tok_gt;
        case '<':
            if (cur_symb == '=') {
                cur_symb = inputSymbol();
                return tok_le;
            }
            return tok_lt;
        case '!':
            if (cur_symb == '=') {
                cur_symb = inputSymbol();
                return tok_ne;
            }
            throw tok_excl;
        case ':':
            if (cur_symb == '=') {
                cur_symb = inputSymbol();
                return tok_fastassign;
            }
            return tok_colon;
        case ';':
            return tok_semicolon;
        default:
            throw "ERROR. Unknown symbol " + (char) pr_symb;
    }
}

Token Lexer::readString() {
    std::string str;
    while (type_of_char() == LETTER || type_of_char() == NUMBER || (type_of_char() == SPE_SYMB && cur_symb == '_')) {
        str += (char) cur_symb;
        cur_symb = inputSymbol();
    }

    for (auto &i: keyWordTable) {
        if (i.slovo == str)
            return i.symb;
    }

    m_IdentifierStr = str;
    return tok_identifier;

}

void Lexer::skipOptional_() {
    if (cur_symb == '_')
        cur_symb = inputSymbol();
}

bool Lexer::checkIfCanBe(int cur_symb, int base) {
    return (
            cur_symb == '_'
            ||
            (
                    base == 2
                    &&
                    (cur_symb == '0' || cur_symb == '1')
            )
            ||
            (
                    base == 8
                    &&
                    (cur_symb >= '0' && cur_symb <= '7')
            )
            ||
            (
                    base == 10
                    &&
                    (cur_symb >= '0' && cur_symb <= '9')
            )
            ||
            (
                    base == 16
                    &&
                    ((cur_symb >= '0' && cur_symb <= '9') || (cur_symb >= 'a' && cur_symb <= 'f') ||
                     (cur_symb >= 'A' && cur_symb <= 'F'))
            )
    );
}


void Lexer::inputNumber(int base) {
    cur_symb = inputSymbol();
    skipOptional_();
    m_NumVal *= base;
    if (checkIfCanBe(cur_symb, base)) {
        if (cur_symb >= '0' && cur_symb <= '9')
            m_NumVal += cur_symb - 48;
        if (cur_symb >= 'A' && cur_symb <= 'F')
            m_NumVal += cur_symb - 65;
        if (cur_symb >= 'a' && cur_symb <= 'f')
            m_NumVal += cur_symb - 97;
    } else
        throw "ERROR. Wrong format of number in " + std::to_string(cur_symb) + " base";
    cur_symb = inputSymbol();
    while (cur_symb == '_' || checkIfCanBe(cur_symb, base)) {
        skipOptional_();
        m_NumVal *= base;
        if (cur_symb >= '0' && cur_symb <= '9')
            m_NumVal += cur_symb - 48;
        if (cur_symb >= 'A' && cur_symb <= 'F')
            m_NumVal += cur_symb - 65;
        if (cur_symb >= 'a' && cur_symb <= 'f')
            m_NumVal = cur_symb - 97;
        cur_symb = inputSymbol();
    }

}

Token Lexer::readNumber() {
    m_NumVal = 0;
    m_DouVal = 0;
    if (cur_symb == '0') {
        cur_symb = inputSymbol();
        //float
        if (cur_symb == '.') {
            inputNumber(10);
            m_DouVal = m_NumVal;
            m_NumVal = 0;

            while (m_DouVal > 1) {
                m_DouVal /= 10;
            }

            return tok_num_float;
        }

        // bin
        if (cur_symb == 'b' || cur_symb == 'B')
            inputNumber(2);
        // oct
        if (cur_symb == 'o' || cur_symb == 'O')
            inputNumber(8);
        //hex
        if (cur_symb == 'x' || cur_symb == 'X')
            inputNumber(16);

        return tok_num_int;
    }

    m_NumVal = cur_symb - 48;
    inputNumber(10);
    if (cur_symb == '.') {
        int int_part = m_NumVal;
        m_NumVal = 0;

        inputNumber(10);
        m_DouVal = m_NumVal;
        m_NumVal = 0;

        while (m_DouVal > 1) {
            m_DouVal /= 10;
        }
        m_DouVal += int_part;

        return tok_num_float;
    }
    return tok_num_int;

}

bool Lexer::haveBNL() {
    return newLine;
}

void Lexer::decrease() {
    if (newLine > 0)
        newLine -= 1;
}

Token Lexer::gettok() {
    switch (type_of_char()) {
        case LETTER:
            decrease();
            return readString();
        case NUMBER:
            decrease();
            return readNumber();
        case NEW_LINE:
            newLine = 2;
        case WHITE_SPACE:
            cur_symb = inputSymbol();
            return gettok();
        case END:
            decrease();
            return tok_eof;
        case SPE_SYMB:
            decrease();
            return readSymbol();
        default:
            throw "ERROR. Unknown token";
    }
}

Token Lexer::gettokWhithOutIgnoreNewLine() {
    switch (type_of_char()) {
        case LETTER:
            return readString();
        case NUMBER:
            return readNumber();
        case WHITE_SPACE:
            cur_symb = inputSymbol();
            return gettok();
        case NEW_LINE:
            return tok_newline;
        case END:
            return tok_eof;
        case SPE_SYMB:
            return readSymbol();
        default:
            throw "ERROR. Unknown token";
    }
}