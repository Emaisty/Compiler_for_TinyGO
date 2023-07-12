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
                // declaration
            }
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


std::unique_ptr<AST::ASTExpression> Parser::parseExpression() {

}

std::unique_ptr<AST::ASTType> Parser::parseType() {

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
        std::vector<AST::ASTConstDeclaration *> temp_vector;
        do {
            match(tok_identifier);
            temp_vector.push_back(new AST::ASTConstDeclaration);
            temp_vector[temp_vector.size() - 1]->setName(lexer.identifierStr());
            cur_tok = lexer.gettok();
        } while (cur_tok == tok_comma && (cur_tok = lexer.gettok()));

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