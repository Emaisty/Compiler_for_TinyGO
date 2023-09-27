#include "parser.h"

std::map<std::string, std::unique_ptr<AST::ASTType>> named_type;

void Parser::match(Token tok) {
    if (cur_tok != tok)
        throw std::invalid_argument("ERROR. Unmatched tokens");
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

bool Parser::checkForSeparator() {
    if (cur_tok == tok_semicolon) {
        cur_tok = lexer.gettok();
        return true;
    } else if (lexer.haveBNL()) {
        return true;
    }
    return false;
}

bool
Parser::matchTypes(const std::unique_ptr<AST::ASTType> &type, const std::unique_ptr<AST::ASTType> &reference_type) {
    //TODO
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

        if (!checkForSeparator()) {
            match(tok_clfigbr);
            break;
        }
    }

    matchAndGoNext(tok_clfigbr);

    return res;
}

std::unique_ptr<AST::ASTExpression> Parser::E11() {
    auto tmp = E10();
    return E11_PRIME(tmp);
}

std::unique_ptr<AST::ASTExpression> Parser::E11_PRIME(std::unique_ptr<AST::ASTExpression> &left) {
    if (cur_tok == tok_or) {
        matchAndGoNext(tok_or);

        auto right = E10();
        auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::OR)->cloneExpr();

        return E11_PRIME(tmp);

    }

    return left->cloneExpr();
}

std::unique_ptr<AST::ASTExpression> Parser::E10() {
    auto tmp = E9();
    return E10_PRIME(tmp);
}

std::unique_ptr<AST::ASTExpression> Parser::E10_PRIME(std::unique_ptr<AST::ASTExpression> &left) {
    if (cur_tok == tok_and) {
        cur_tok = lexer.gettok();

        auto right = E9();
        auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::AND)->cloneExpr();

        return E10_PRIME(tmp);

    }

    return left->cloneExpr();
}

std::unique_ptr<AST::ASTExpression> Parser::E9() {
    auto tmp = E8();
    return E9_PRIME(tmp);
}

std::unique_ptr<AST::ASTExpression> Parser::E9_PRIME(std::unique_ptr<AST::ASTExpression> &left) {
    if (cur_tok == tok_binor) {
        cur_tok = lexer.gettok();

        auto right = E8();
        auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::BINOR)->cloneExpr();

        return E9_PRIME(tmp);

    }

    return left->cloneExpr();
}

std::unique_ptr<AST::ASTExpression> Parser::E8() {
    auto tmp = E7();
    return E8_PRIME(tmp);
}

std::unique_ptr<AST::ASTExpression> Parser::E8_PRIME(std::unique_ptr<AST::ASTExpression> &left) {
    if (cur_tok == tok_binand) {
        cur_tok = lexer.gettok();

        auto right = E7();
        auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::BINAND)->cloneExpr();

        return E8_PRIME(tmp);

    }

    return left->cloneExpr();
}

std::unique_ptr<AST::ASTExpression> Parser::E7() {
    auto tmp = E6();
    return E7_PRIME(tmp);
}

std::unique_ptr<AST::ASTExpression> Parser::E7_PRIME(std::unique_ptr<AST::ASTExpression> &left) {
    switch (cur_tok) {
        case tok_eq: {
            cur_tok = lexer.gettok();

            auto right = E6();
            auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::EQ)->cloneExpr();

            return E7_PRIME(tmp);
        }
        case tok_ne: {
            cur_tok = lexer.gettok();

            auto right = E6();
            auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::NE)->cloneExpr();

            return E7_PRIME(tmp);
        }
        default:
            return left->cloneExpr();
    }
}

std::unique_ptr<AST::ASTExpression> Parser::E6() {
    auto tmp = E5();
    return E6_PRIME(tmp);
}

std::unique_ptr<AST::ASTExpression> Parser::E6_PRIME(std::unique_ptr<AST::ASTExpression> &left) {
    switch (cur_tok) {
        case tok_gt: {
            cur_tok = lexer.gettok();

            auto right = E5();
            auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::GT)->cloneExpr();

            return E6_PRIME(tmp);
        }
        case tok_ge: {
            cur_tok = lexer.gettok();

            auto right = E5();
            auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::GE)->cloneExpr();

            return E6_PRIME(tmp);
        }
        case tok_lt: {
            cur_tok = lexer.gettok();

            auto right = E5();
            auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::LT)->cloneExpr();

            return E6_PRIME(tmp);
        }
        case tok_le: {
            cur_tok = lexer.gettok();

            auto right = E5();
            auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::LE)->cloneExpr();

            return E6_PRIME(tmp);
        }
        default:
            return left->cloneExpr();
    }
}

std::unique_ptr<AST::ASTExpression> Parser::E5() {
    auto tmp = E4();
    return E5_PRIME(tmp);
}

std::unique_ptr<AST::ASTExpression> Parser::E5_PRIME(std::unique_ptr<AST::ASTExpression> &left) {
    switch (cur_tok) {
        case tok_plus: {
            cur_tok = lexer.gettok();

            auto right = E4();
            auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right,
                                                                AST::ASTBinaryOperator::PLUS)->cloneExpr();

            return E5_PRIME(tmp);
        }
        case tok_minus: {
            cur_tok = lexer.gettok();

            auto right = E4();
            auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right,
                                                                AST::ASTBinaryOperator::MINUS)->cloneExpr();

            return E5_PRIME(tmp);
        }
        default:
            return left->cloneExpr();
    }
}

std::unique_ptr<AST::ASTExpression> Parser::E4() {
    auto tmp = E2();
    return E4_PRIME(tmp);
}

std::unique_ptr<AST::ASTExpression> Parser::E4_PRIME(std::unique_ptr<AST::ASTExpression> &left) {
    switch (cur_tok) {
        case tok_asterisk: {
            cur_tok = lexer.gettok();

            auto right = E2();
            auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::MUL)->cloneExpr();

            return E4_PRIME(tmp);
        }
        case tok_div: {
            cur_tok = lexer.gettok();

            auto right = E2();
            auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::DIV)->cloneExpr();

            return E4_PRIME(tmp);
        }
        case tok_mod: {
            cur_tok = lexer.gettok();

            auto right = E2();
            auto tmp = std::make_unique<AST::ASTBinaryOperator>(left, right, AST::ASTBinaryOperator::MOD)->cloneExpr();

            return E4_PRIME(tmp);
        }
        default:
            return left->cloneExpr();
    }
}

std::unique_ptr<AST::ASTExpression> Parser::E2() {
    switch (cur_tok) {
        case tok_minus: {
            cur_tok = lexer.gettok();
            auto arg = E2();
            return std::make_unique<AST::ASTUnaryOperator>(arg, AST::ASTUnaryOperator::Operator::MINUS);
        }
        case tok_plus: {
            cur_tok = lexer.gettok();
            auto arg = E2();
            return std::make_unique<AST::ASTUnaryOperator>(arg, AST::ASTUnaryOperator::Operator::PLUS);
        }
        case tok_excl: {
            cur_tok = lexer.gettok();
            auto arg = E2();
            return std::make_unique<AST::ASTUnaryOperator>(arg, AST::ASTUnaryOperator::Operator::NOT);
        }
        default: {
            return E1();
        }

    }
}

std::unique_ptr<AST::ASTExpression> Parser::E1() {
    auto tmp = E0();
    return E1_PRIME(tmp);
}

std::unique_ptr<AST::ASTExpression> Parser::E1_PRIME(std::unique_ptr<AST::ASTExpression> &left) {
    switch (cur_tok) {
        case tok_opbr: {
            cur_tok = lexer.gettok();
            auto arg = parseExpressionList();
            matchAndGoNext(tok_clbr);
            auto tmp = std::make_unique<AST::ASTFunctionCall>(left, arg)->cloneExpr();
            return E1_PRIME(tmp);
        }
        case tok_dot: {
            cur_tok = lexer.gettok();
            auto member = E0();
            auto tmp = std::make_unique<AST::ASTMemberAccess>(left, member)->cloneExpr();
            return E1_PRIME(tmp);
        }
        default:
            return left->cloneExpr();
    }
}

std::unique_ptr<AST::ASTExpression> Parser::E0() {
    switch (cur_tok) {
        case tok_opbr: {
            matchAndGoNext(tok_opbr);
            auto res = parseExpression()->cloneExpr();
            matchAndGoNext(tok_clbr);
            return res;
        }
        case tok_num_int: {
            auto res = std::make_unique<AST::ASTIntNumber>(lexer.numVal())->cloneExpr();
            cur_tok = lexer.gettok();
            return res;
        }
        case tok_false:
        case tok_true: {
            auto res = std::make_unique<AST::ASTBoolNumber>(cur_tok == tok_true)->cloneExpr();
            cur_tok = lexer.gettok();
            return res;
        }
        case tok_num_float: {
            auto res = std::make_unique<AST::ASTFloatNumber>(lexer.douVal())->cloneExpr();
            cur_tok = lexer.gettok();
            return res;
        }
        default: {
            match(tok_identifier);
            auto res = std::make_unique<AST::ASTVar>(lexer.identifierStr())->cloneExpr();
            cur_tok = lexer.gettok();
            return res;
        }
    }

}


std::unique_ptr<AST::ASTExpression> Parser::parseExpression() {
    return E11();
}

std::vector<std::unique_ptr<AST::ASTExpression>> Parser::parseExpressionList() {
    std::vector<std::unique_ptr<AST::ASTExpression>> res;

    do {
        res.push_back(parseExpression());
    } while (cur_tok == tok_comma && (cur_tok = lexer.gettok()));
    return res;
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
            return std::make_unique<AST::ASTTypeFloat>();
        case tok_bool:
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTTypeBool>();
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

            if (named_type.find(name) == named_type.end())
                throw std::invalid_argument("ERROR. UNKNOWN NAME FOR TYPE");

            return named_type[name]->clone();
        }
        default:
            throw std::invalid_argument("ERROR. UNPARSABLE TYPE");
    }
}

std::unique_ptr<AST::Function> Parser::parseFunction() {
    matchAndGoNext(tok_func);
    std::unique_ptr<AST::Function> lol(new AST::Function);

    return lol;

}

std::vector<std::unique_ptr<AST::ASTDeclaration>> Parser::parseConstDeclarationLine() {
    std::vector<std::unique_ptr<AST::ASTDeclaration>> res;

    auto names = parseIdentifierList();

    std::unique_ptr<AST::ASTType> type = nullptr;
    // if there is type before assign sign
    if (cur_tok != tok_assign) {
        type = parseType();
    }
    matchAndGoNext(tok_assign);

    auto values = parseExpressionList();

    // Checks
    if (names.size() != values.size())
        throw std::invalid_argument("ERROR. Diff size of declared var and expressions");

    if (type)
        for (const auto &i: values)
            if (!matchTypes(i->getType(), type))
                throw std::invalid_argument("Error. Diff type of declaration and given expr");


    for (unsigned long int i = 0; i < names.size(); ++i)
        if (type)
            res.push_back(std::make_unique<AST::ASTConstDeclaration>(names[i], values[i], type));
        else
            res.push_back(std::make_unique<AST::ASTConstDeclaration>(names[i], values[i], values[i]->getType()));


    return res;
}


std::vector<std::unique_ptr<AST::ASTDeclaration>> Parser::parseTypeDeclarationLine() {
    std::vector<std::unique_ptr<AST::ASTDeclaration>> res;
    match(tok_identifier);
    std::string name = lexer.identifierStr();
    cur_tok = lexer.gettok();
    if (cur_tok == tok_assign)
        cur_tok = lexer.gettok();

    auto type = parseType();
    res.push_back(std::make_unique<AST::ASTConstDeclaration>(name, type));

    return res;
}

std::vector<std::unique_ptr<AST::ASTDeclaration> > Parser::parseVarDeclarationLine() {
    std::vector<std::unique_ptr<AST::ASTDeclaration>> res;

    auto names = parseIdentifierList();


    // if there is type before assign sign
    if (cur_tok != tok_assign) {
        std::unique_ptr<AST::ASTType> type = nullptr;
        type = parseType();
        if (cur_tok == tok_assign) {
            //type and value
            matchAndGoNext(tok_assign);

            auto values = parseExpressionList();

            // Checks
            if (names.size() != values.size())
                throw std::invalid_argument("ERROR. Diff size of declared var and expressions");

            for (const auto &i: values)
                if (!matchTypes(i->getType(), type))
                    throw std::invalid_argument("Error. Diff type of declaration and given expr");


            for (unsigned long int i = 0; i < names.size(); ++i)
                res.push_back(std::make_unique<AST::ASTConstDeclaration>(names[i], values[i], type));


        } else
            // type without value
            for (auto &name: names)
                res.push_back(std::make_unique<AST::ASTConstDeclaration>(name, type));

    } else {
        //without type
        matchAndGoNext(tok_assign);

        auto values = parseExpressionList();

        // Checks
        if (names.size() != values.size())
            throw std::invalid_argument("ERROR. Diff size of declared var and expressions");

        for (unsigned long int i = 0; i < names.size(); ++i)
            res.push_back(std::make_unique<AST::ASTConstDeclaration>(names[i], values[i], values[i]->getType()));

    }

    return res;
}


std::vector<std::unique_ptr<AST::ASTDeclaration> >
Parser::parseDeclarationBlock(const std::function<std::vector<std::unique_ptr<AST::ASTDeclaration>>()> &type_of_line) {
    std::vector<std::unique_ptr<AST::ASTDeclaration>> result;

    if (cur_tok == tok_opbr) {
        //multiple
        matchAndGoNext(tok_opbr);
        while (cur_tok != tok_clbr) {
            auto decl_line = type_of_line();
            for (auto &i: decl_line)
                result.push_back(i->cloneDecl());
            if (!checkForSeparator()) {
                match(tok_clbr);
                break;
            }

        }
        matchAndGoNext(tok_clbr);
    } else {
        //single
        auto decl_line = type_of_line();

        for (auto &i: decl_line)
            result.push_back(i->cloneDecl());
    }

    return result;
}

std::vector<std::unique_ptr<AST::ASTDeclaration>> Parser::parseDeclaration() {
    std::vector<std::unique_ptr<AST::ASTDeclaration>> res;
    switch (cur_tok) {
        case tok_const:
            matchAndGoNext(tok_const);
            res = parseDeclarationBlock(std::bind(&Parser::parseConstDeclarationLine, this));
            break;
        case tok_var:
            matchAndGoNext(tok_var);
            res = parseDeclarationBlock(std::bind(&Parser::parseVarDeclarationLine, this));
            break;
        default:
            matchAndGoNext(tok_type);
            res = parseDeclarationBlock(std::bind(&Parser::parseTypeDeclarationLine, this));
            break;
    }
    if (!checkForSeparator())
        throw std::invalid_argument("ERROR. No separator after declaration");
    return res;
}