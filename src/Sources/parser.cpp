#include "parser.h"

#include <memory>

void Parser::match(Token tok) {
    if (cur_tok != tok)
        throw "ERROR. Unmatched tokens";
}

void Parser::matchAndGoNext(Token tok) {
    match(tok);
    cur_tok = lexer.gettok();
}

Parser::Parser(char *str) {
    lexer.InitInput(str);
    cur_tok = lexer.gettok();
}

bool Parser::parse() {
    matchAndGoNext(tok_package);

    match(tok_identifier);
    program.setName(lexer.identifierStr());

    cur_tok = lexer.gettok();

    while (cur_tok != tok_eof) {
        switch (cur_tok) {
            // function
            case tok_func: {
                auto tmp = parseFunction();
                program.addFunction(tmp);
                break;
            }
                // declaration
            case tok_const:
            case tok_type:
            case tok_var: {
                auto decl = parseDeclaration();
                for (auto &i: decl)
                    program.addDecl(i);
            }
                break;
            default:
                return false;
        }

    }
    return true;

}

std::vector<std::string> Parser::parseIdentifierList() {
    std::vector<std::string> res;

    do {
        match(tok_identifier);
        res.push_back(lexer.identifierStr());
        cur_tok = lexer.gettok();
    } while (cur_tok == tok_comma && (cur_tok = lexer.gettok()));
    return res;
}

std::unique_ptr<AST::ASTTypeStruct> Parser::parseStruct() {
    matchAndGoNext(tok_struct);

    matchAndGoNext(tok_opfigbr);

    auto res = std::unique_ptr<AST::ASTTypeStruct>();

    while (cur_tok != tok_clfigbr) {
        auto names = parseIdentifierList();

        auto type = parseType();

        for (auto &name: names)
            res->addField(name, type);

        if (cur_tok == tok_semicolon)
            cur_tok = lexer.gettok();
        else if (!lexer.haveBNL())
            throw "ERROR. NO SEPARATOR";
    }

    matchAndGoNext(tok_clfigbr);

    return res;
}


std::unique_ptr<AST::ASTExpression> Parser::parseExpression() {

}

std::unique_ptr<AST::ASTType> Parser::parseType() {
    switch (cur_tok) {
        case tok_int:
        case tok_int32:
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTTypeInt>();
        case tok_int8:
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTTypeInt>(8);
        case tok_int64:
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTTypeInt>(64);
        case tok_float:
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTTypeDouble>();
        case tok_struct:
            return parseStruct();
        case tok_opbr: {
            cur_tok = lexer.gettok();
            auto tmp = parseType();
            matchAndGoNext(tok_clbr);
            return tmp;
        }
        case tok_asterisk:
            matchAndGoNext(tok_asterisk);
            return std::make_unique<AST::ASTTypePointer>(parseType());

        case tok_identifier: {
            auto name = lexer.identifierStr();
            cur_tok = lexer.gettok();
            // TODO check if this type exist
            return std::make_unique<AST::ASTTypeNamed>(name);
        }
        default:
            throw "ERROR. UNPARSABLE TYPE";
    }
}

std::unique_ptr<AST::Function> Parser::parseFunction() {
    matchAndGoNext(tok_func);
    std::unique_ptr<AST::Function> lol(new AST::Function);

    return lol;

}

std::vector<std::unique_ptr<AST::ASTDeclaration>> Parser::parseConstDeclaration() {
    std::vector<std::unique_ptr<AST::ASTDeclaration>> result;

    matchAndGoNext(tok_const);
    if (cur_tok == tok_opbr) {

    } else {
        std::vector<std::unique_ptr<AST::ASTConstDeclaration >> temp_vector;
        do {
            match(tok_identifier);
            temp_vector.push_back(std::make_unique<AST::ASTConstDeclaration>());
            temp_vector[temp_vector.size() - 1]->setName(lexer.identifierStr());
            cur_tok = lexer.gettok();
        } while (cur_tok == tok_comma && (cur_tok = lexer.gettok()));

        // if there is type before assign sign
        if (cur_tok != tok_assign) {
            auto type = parseType();
            for (auto &i: temp_vector) {
                i->setType(type);
            }
        }
        matchAndGoNext(tok_assign);

        int i = 0;
        do {

        }

    }

    return result;
}

std::vector<std::unique_ptr<AST::ASTDeclaration >> Parser::parseTypeDeclaration() {

}

std::vector<std::unique_ptr<AST::ASTDeclaration>> Parser::parseVarDeclaration() {

}

std::vector<std::unique_ptr<AST::ASTDeclaration>> Parser::parseDeclaration() {
    switch (cur_tok) {
        case tok_const:
            return parseConstDeclaration();
        case tok_var:
            return parseVarDeclaration();
        default:
            return parseTypeDeclaration();
    }
}