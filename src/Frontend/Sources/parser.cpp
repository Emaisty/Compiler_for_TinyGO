#include "parser.h"


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
                program.addFunction(std::move(tmp));
                break;
            }
                // declaration

            case tok_type: {
                matchAndGoNext(tok_type);
                for (auto &i: parseDeclarationBlock(std::bind(&Parser::parseTypeDeclarationLine, this)))
                    program.addTypeDecl(std::move(i));
                if (!checkForSeparator())
                    throw std::invalid_argument("ERROR. No separator after declaration");
                break;
            }
            case tok_const: {
                matchAndGoNext(tok_type);
                for (auto &i: parseDeclarationBlock(std::bind(&Parser::parseConstDeclarationLine, this)))
                    program.addVarDecl(std::move(i));
                if (!checkForSeparator())
                    throw std::invalid_argument("ERROR. No separator after declaration");
                break;
            }
            case tok_var: {
                matchAndGoNext(tok_type);
                for (auto &i: parseDeclarationBlock(std::bind(&Parser::parseVarDeclarationLine, this)))
                    program.addVarDecl(std::move(i));
                if (!checkForSeparator())
                    throw std::invalid_argument("ERROR. No separator after declaration");
                break;
            }
            default:
                return false;
        }

    }
//    program.checker();
    return true;

}

void Parser::printIR(std::ostream &os) {
//    os << *program.generateIR();
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
    return type == reference_type;
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
    auto res = std::make_unique<AST::ASTTypeStruct>();

    while (cur_tok != tok_clfigbr) {
        auto names = parseIdentifierList();

        auto type = parseType();

        for (auto &name: names)
            res->addField(name, std::move(type));

        if (!checkForSeparator()) {
            match(tok_clfigbr);
            break;
        }
    }

    matchAndGoNext(tok_clfigbr);

    return res;
}

std::unique_ptr<AST::ASTExpression> Parser::E11() {
    return E11_PRIME(E10());
}

std::unique_ptr<AST::ASTExpression> Parser::E11_PRIME(std::unique_ptr<AST::ASTExpression> &&left) {
    if (cur_tok == tok_or) {
        matchAndGoNext(tok_or);

        auto right = E10();
        return E11_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                  AST::ASTBinaryOperator::OR));
    }

    return left;
}

std::unique_ptr<AST::ASTExpression> Parser::E10() {
    return E10_PRIME(E9());
}

std::unique_ptr<AST::ASTExpression> Parser::E10_PRIME(std::unique_ptr<AST::ASTExpression> &&left) {
    if (cur_tok == tok_and) {
        cur_tok = lexer.gettok();

        auto right = E9();
        return E10_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                  AST::ASTBinaryOperator::AND));
    }

    return left;
}

std::unique_ptr<AST::ASTExpression> Parser::E9() {
    return E9_PRIME(E8());
}

std::unique_ptr<AST::ASTExpression> Parser::E9_PRIME(std::unique_ptr<AST::ASTExpression> &&left) {
    if (cur_tok == tok_binor) {
        cur_tok = lexer.gettok();

        auto right = E8();
        return E9_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                 AST::ASTBinaryOperator::BINOR));
    }

    return left;
}

std::unique_ptr<AST::ASTExpression> Parser::E8() {
    return E8_PRIME(E7());
}

std::unique_ptr<AST::ASTExpression> Parser::E8_PRIME(std::unique_ptr<AST::ASTExpression> &&left) {
    if (cur_tok == tok_binand) {
        cur_tok = lexer.gettok();

        auto right = E7();
        return E8_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                 AST::ASTBinaryOperator::BINAND));
    }

    return left;
}

std::unique_ptr<AST::ASTExpression> Parser::E7() {
    return E7_PRIME(E6());
}

std::unique_ptr<AST::ASTExpression> Parser::E7_PRIME(std::unique_ptr<AST::ASTExpression> &&left) {
    switch (cur_tok) {
        case tok_eq: {
            cur_tok = lexer.gettok();

            auto right = E6();
            return E7_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                     AST::ASTBinaryOperator::EQ));
        }
        case tok_ne: {
            cur_tok = lexer.gettok();

            auto right = E6();
            return E7_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                     AST::ASTBinaryOperator::NE));
        }
        default:
            return left;
    }
}

std::unique_ptr<AST::ASTExpression> Parser::E6() {
    return E6_PRIME(E5());
}

std::unique_ptr<AST::ASTExpression> Parser::E6_PRIME(std::unique_ptr<AST::ASTExpression> &&left) {
    switch (cur_tok) {
        case tok_gt: {
            cur_tok = lexer.gettok();

            auto right = E5();
            return E6_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                     AST::ASTBinaryOperator::GT));
        }
        case tok_ge: {
            cur_tok = lexer.gettok();

            auto right = E5();
            return E6_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                     AST::ASTBinaryOperator::GE));
        }
        case tok_lt: {
            cur_tok = lexer.gettok();

            auto right = E5();
            return E6_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                     AST::ASTBinaryOperator::LT));
        }
        case tok_le: {
            cur_tok = lexer.gettok();

            auto right = E5();
            return E6_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                     AST::ASTBinaryOperator::LE));
        }
        default:
            return left;
    }
}

std::unique_ptr<AST::ASTExpression> Parser::E5() {
    return E5_PRIME(E4());
}

std::unique_ptr<AST::ASTExpression> Parser::E5_PRIME(std::unique_ptr<AST::ASTExpression> &&left) {
    switch (cur_tok) {
        case tok_plus: {
            cur_tok = lexer.gettok();

            auto right = E4();
            return E5_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                     AST::ASTBinaryOperator::PLUS));
        }
        case tok_minus: {
            cur_tok = lexer.gettok();

            auto right = E4();
            return E5_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                     AST::ASTBinaryOperator::MINUS));
        }
        default:
            return left;
    }
}

std::unique_ptr<AST::ASTExpression> Parser::E4() {
    return E4_PRIME(E2());
}

std::unique_ptr<AST::ASTExpression> Parser::E4_PRIME(std::unique_ptr<AST::ASTExpression> &&left) {
    switch (cur_tok) {
        case tok_asterisk: {
            cur_tok = lexer.gettok();

            auto right = E2();
            return E4_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                     AST::ASTBinaryOperator::MUL));
        }
        case tok_div: {
            cur_tok = lexer.gettok();

            auto right = E2();
            return E4_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                     AST::ASTBinaryOperator::DIV));
        }
        case tok_mod: {
            cur_tok = lexer.gettok();

            auto right = E2();
            return E4_PRIME(std::make_unique<AST::ASTBinaryOperator>(std::move(left), std::move(right),
                                                                     AST::ASTBinaryOperator::MOD));
        }
        default:
            return left;
    }
}

std::unique_ptr<AST::ASTExpression> Parser::E2() {
    switch (cur_tok) {
        case tok_minus: {
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTUnaryOperator>(E2(), AST::ASTUnaryOperator::MINUS);
        }
        case tok_plus: {
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTUnaryOperator>(E2(), AST::ASTUnaryOperator::PLUS);
        }
        case tok_inc: {
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTUnaryOperator>(E2(), AST::ASTUnaryOperator::PREINC);
        }
        case tok_dec: {
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTUnaryOperator>(E2(), AST::ASTUnaryOperator::PREDEC);
        }
        case tok_excl: {
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTUnaryOperator>(E2(), AST::ASTUnaryOperator::NOT);
        }
        case tok_asterisk: {
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTUnaryOperator>(E2(), AST::ASTUnaryOperator::DEREFER);
        }
        case tok_binand: {
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTUnaryOperator>(E2(), AST::ASTUnaryOperator::REFER);
        }
        default: {
            return E1();
        }

    }
}

std::unique_ptr<AST::ASTExpression> Parser::E1() {
    return E1_PRIME(E0());
}

std::unique_ptr<AST::ASTExpression> Parser::E1_PRIME(std::unique_ptr<AST::ASTExpression> &&left) {
    switch (cur_tok) {
        case tok_opbr: {
            cur_tok = lexer.gettok();
            auto arg = parseExpressionList();
            matchAndGoNext(tok_clbr);
            return E1_PRIME(std::make_unique<AST::ASTFunctionCall>(std::move(left), arg));
        }
        case tok_dot: {
            cur_tok = lexer.gettok();
            match(tok_identifier);
            auto name_of_member = lexer.identifierStr();
            matchAndGoNext(tok_identifier);
            return E1_PRIME(std::make_unique<AST::ASTMemberAccess>(std::move(left), name_of_member));
        }
        case tok_inc: {
            cur_tok = lexer.gettok();
            return E1_PRIME(
                    std::make_unique<AST::ASTUnaryOperator>(std::move(left), AST::ASTUnaryOperator::Operator::POSTINC));
        }
        case tok_dec: {
            cur_tok = lexer.gettok();
            return E1_PRIME(
                    std::make_unique<AST::ASTUnaryOperator>(std::move(left), AST::ASTUnaryOperator::Operator::POSTDEC));
        }
        default:
            return std::move(left);
    }
}

std::unique_ptr<AST::ASTExpression> Parser::E0() {
    switch (cur_tok) {
        case tok_opbr: {
            matchAndGoNext(tok_opbr);
            auto res = parseExpression();
            matchAndGoNext(tok_clbr);
            return std::move(res);
        }
        case tok_num_int: {
            auto res = std::make_unique<AST::ASTIntNumber>(lexer.numVal());
            cur_tok = lexer.gettok();
            return std::move(res);
        }
        case tok_false:
        case tok_true: {
            auto res = std::make_unique<AST::ASTBoolNumber>(cur_tok == tok_true);
            cur_tok = lexer.gettok();
            return res;
        }
        case tok_num_float: {
            auto res = std::make_unique<AST::ASTFloatNumber>(lexer.douVal());
            cur_tok = lexer.gettok();
            return std::move(res);
        }

        case tok_identifier: {
            match(tok_identifier);
            auto res = std::make_unique<AST::ASTVar>(lexer.identifierStr());
            cur_tok = lexer.gettok();
            return std::move(res);
        }
        case tok_struct : {
            auto type = parseStruct();
            std::vector<std::pair<std::string, std::unique_ptr<AST::ASTExpression>>> values;
            matchAndGoNext(tok_opfigbr);
            if (cur_tok == tok_clfigbr) {
                matchAndGoNext(tok_clfigbr);
                return std::make_unique<AST::ASTStruct>(std::move(type), values);
            }
            do {
                match(tok_identifier);
                auto name = lexer.identifierStr();
                matchAndGoNext(tok_identifier);
                matchAndGoNext(tok_colon);
                auto type_of_field = parseExpression();
                values.emplace_back(std::make_pair(name, std::move(type_of_field)));
            } while (cur_tok == tok_comma && (cur_tok = lexer.gettok()));

            matchAndGoNext(tok_clfigbr);
            return std::make_unique<AST::ASTStruct>(std::move(type), values);
        }
        default:
            throw std::invalid_argument("ERROR. Cannot parse expression.");
    }
}


std::unique_ptr<AST::ASTExpression> Parser::parseExpressionOrNone() {
    switch (cur_tok) {
        case tok_opbr:
        case tok_num_int:
        case tok_num_float:
        case tok_false:
        case tok_true:
        case tok_identifier:
        case tok_plus:
        case tok_minus:
        case tok_inc:
        case tok_dec:
        case tok_excl:
            return E11();
        default:
            return nullptr;
    }
}

std::vector<std::unique_ptr<AST::ASTExpression>> Parser::parseExpressionListOrNone() {
    std::vector<std::unique_ptr<AST::ASTExpression>> res;
    auto expr = parseExpressionOrNone();
    if (!expr)
        return res;
    res.push_back(std::move(expr));

    while (cur_tok == tok_comma && (cur_tok = lexer.gettok())) {
        res.push_back(parseExpression());
    }
    return res;
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

std::vector<std::unique_ptr<AST::Statement>> Parser::parseSimpleStat() {
    std::vector<std::unique_ptr<AST::Statement>> res;

    std::vector<std::unique_ptr<AST::ASTExpression>> exprs;
    std::vector<std::string> names;

    int line_number = 0;

    bool can_be_decl = false;


    if (cur_tok == tok_identifier) {
        names.push_back(lexer.identifierStr());
        can_be_decl = true;
    }

    auto expr = parseExpressionOrNone();

    // empty statement
    if (!expr)
        return res;

    exprs.push_back(std::move(expr));

    while (cur_tok == tok_comma && (cur_tok = lexer.gettok())) {
        if (cur_tok == tok_identifier)
            names.push_back(lexer.identifierStr());

        expr = parseExpression();
        if (!dynamic_cast<AST::ASTVar *>(expr.get())) {
            can_be_decl = false;
        }
        exprs.push_back(std::move(expr));
    }

    if (cur_tok == tok_plusassign || cur_tok == tok_minassign || cur_tok == tok_mulassign ||
        cur_tok == tok_divassign || cur_tok == tok_modassign || cur_tok == tok_assign) {
        AST::ASTAssign::TypeOfAssign type;
        line_number = lexer.getLineNumber();
        switch (cur_tok) {
            case tok_plusassign:
                type = AST::ASTAssign::TypeOfAssign::PLUSASSIGN;
                break;
            case tok_minassign:
                type = AST::ASTAssign::TypeOfAssign::MINUSASSIGN;
                break;
            case tok_mulassign:
                type = AST::ASTAssign::TypeOfAssign::MULTASSIGN;
                break;
            case tok_divassign:
                type = AST::ASTAssign::TypeOfAssign::DIVASSIGN;
                break;
            case tok_modassign:
                type = AST::ASTAssign::TypeOfAssign::MODASSIGN;
                break;
            case tok_assign:
                type = AST::ASTAssign::TypeOfAssign::ASSIGN;
                break;
        }
        cur_tok = lexer.gettok();

        auto values = parseExpressionList();
        res.emplace_back(std::make_unique<AST::ASTAssign>(std::move(exprs), std::move(values), type));
        return res;
    }

    if (cur_tok == tok_fastassign && can_be_decl) {
        matchAndGoNext(tok_fastassign);
        line_number = lexer.getLineNumber();
        auto values = parseExpressionList();
        res.emplace_back(std::make_unique<AST::ASTVarDeclaration>(std::move(names), std::move(values)));
        return res;
    }

    // if line is a seq of expresions

    for (auto &i: exprs)
        res.emplace_back(std::move(i));

    for (auto &i: res)

        i->addLineNumber(line_number);

    return res;
}

std::unique_ptr<AST::Statement> Parser::parseReturn() {
    auto res = std::make_unique<AST::ASTReturn>();
    matchAndGoNext(tok_return);

    res->addLineNumber(lexer.getLineNumber());

    auto return_values = parseExpressionListOrNone();
    if (return_values.empty())
        return res;
    if (return_values.size() == 1)
        res->addReturnValue(std::move(return_values[0]));
    else
        for (auto &i: parseExpressionListOrNone())
            res->addReturnValue(std::move(i));

    return res;
}

std::unique_ptr<AST::Statement> Parser::parseIfStat() {
    auto res = std::make_unique<AST::ASTIf>();
    matchAndGoNext(tok_if);

    res->addLineNumber(lexer.getLineNumber());

    auto expr = parseExpression();
    res->addExpr(std::move(expr));

    auto if_cl = parseBlock();
    res->addIfClause(std::move(if_cl));

    if (cur_tok == tok_else) {
        matchAndGoNext(tok_else);

        auto else_cl = parseBlock();
        res->addElseClause(std::move(else_cl));
    }
    return res;
}

std::unique_ptr<AST::Statement> Parser::parseForLoop() {

    auto res = std::make_unique<AST::ASTFor>();
    matchAndGoNext(tok_for);

    res->addLineNumber(lexer.getLineNumber());

    if (cur_tok != tok_semicolon)
        res->addInitClause(parseSimpleStat());

    matchAndGoNext(tok_semicolon);
    if (cur_tok != tok_semicolon)
        res->addCondClause(parseExpression());
    else
        res->addCondClause(std::make_unique<AST::ASTBoolNumber>(true));
    matchAndGoNext(tok_semicolon);
    if (cur_tok != tok_opfigbr)
        res->addIterClause(parseSimpleStat());


    res->addBody(parseBlock());

    return res;
}

std::unique_ptr<AST::Statement> Parser::parseSwitch() {
    auto res = std::make_unique<AST::ASTSwitch>();
    matchAndGoNext(tok_switch);

    res->addLineNumber(lexer.getLineNumber());

    auto expr = parseExpression();
    res->addExpr(std::move(expr));

    matchAndGoNext(tok_opfigbr);

    bool was_default = false;
    while (cur_tok == tok_case || cur_tok == tok_default) {
        std::unique_ptr<AST::ASTExpression> case_expr = nullptr;
        if (cur_tok == tok_case) {
            matchAndGoNext(tok_case);
            case_expr = parseExpression();
        } else {
            if (was_default)
                throw std::invalid_argument("ERROR. Double default cases");
            was_default = true;
        }

        matchAndGoNext(tok_colon);

        auto block = std::make_unique<AST::ASTBlock>();
        for (auto &i: parseStatementList())
            block->addStatement(std::move(i));

        res->addCase(std::move(case_expr), std::move(block));

    }

    matchAndGoNext(tok_clfigbr);

    return res;
}

std::vector<std::unique_ptr<AST::Statement>> Parser::parseStatement() {
    std::vector<std::unique_ptr<AST::Statement>> res;
    switch (cur_tok) {
        case tok_var:
        case tok_const:
        case tok_type:
            for (auto &i: parseDeclaration())
                res.emplace_back(std::move(i));
            break;
        case tok_if:
            res.emplace_back(parseIfStat());
            break;
        case tok_switch:
            res.emplace_back(parseSwitch());
            break;
        case tok_for:
            res.emplace_back(parseForLoop());
            break;
        case tok_break:
            res.emplace_back(std::make_unique<AST::ASTBreak>());
            break;
        case tok_continue:
            res.emplace_back(std::make_unique<AST::ASTContinue>());
            break;
        case tok_return:
            res.emplace_back(parseReturn());
            break;

        case tok_opfigbr:
            res.emplace_back(parseBlock());
            break;
        default:
            for (auto &i: parseSimpleStat())
                res.emplace_back(std::move(i));
            break;

    }
    return res;
}

std::vector<std::unique_ptr<AST::Statement>> Parser::parseStatementList() {
    std::vector<std::unique_ptr<AST::Statement>> res, stat;

    do {
        stat = parseStatement();
        for (auto &i: stat)
            res.emplace_back(std::move(i));
    } while (!stat.empty() && checkForSeparator());

    return res;
}

std::unique_ptr<AST::ASTType> Parser::parseType() {
    switch (cur_tok) {
        case tok_int:
        case tok_int32:
        case tok_int8:
        case tok_int64:
        case tok_float:
        case tok_bool:
        case tok_identifier: {
            auto name = lexer.identifierStr();
            cur_tok = lexer.gettok();
            return std::make_unique<AST::ASTTypeNamed>(name);
        }
        case tok_asterisk:
            matchAndGoNext(tok_asterisk);
            return std::make_unique<AST::ASTTypePointer>(parseType());
        case tok_struct:
            return parseStruct();

        case tok_opbr: {
            cur_tok = lexer.gettok();
            auto tmp = parseType();
            matchAndGoNext(tok_clbr);
            return tmp;
        }
        default:
            throw std::invalid_argument("ERROR. UNPARSABLE TYPE");
    }
}

std::unique_ptr<AST::ASTBlock> Parser::parseBlock() {
    matchAndGoNext(tok_opfigbr);

    auto res = std::make_unique<AST::ASTBlock>();
    auto stat_list = parseStatementList();
    for (auto &i: stat_list)
        res->addStatement(std::move(i));

    matchAndGoNext(tok_clfigbr);

    return res;
}

void Parser::parseFuncSignature(std::unique_ptr<AST::Function> &function) {

    //parameters
    matchAndGoNext(tok_opbr);

    //if param not empty list
    if (cur_tok != tok_clbr) {

        do {
            auto names = parseIdentifierList();
            auto type = parseType();
            function->addParam(std::move(names), std::move(type));
        } while (cur_tok == tok_comma && (cur_tok = lexer.gettok()));

    }

    matchAndGoNext(tok_clbr);

    // return type


    //if func return something
    if (cur_tok != tok_opfigbr) {


        std::vector<std::unique_ptr<AST::ASTType>> return_type;

        // TODO in future maybe return multiple return types
//        if (cur_tok == tok_opbr) {
//            //maybe multiple of them
//            matchAndGoNext(tok_opbr);
//            do {
//                return_type.emplace_back(std::move(parseType()));
//            } while (cur_tok == tok_comma && (cur_tok = lexer.gettok()));
//            matchAndGoNext(tok_clbr);
//
//        } else
        // just a single type
        return_type.emplace_back(std::move(parseType()));


        for (auto &i: return_type)
            function->addReturn(std::move(i));

    }

}

std::unique_ptr<AST::Function> Parser::parseFunction() {
    matchAndGoNext(tok_func);

    auto res = std::make_unique<AST::Function>();
    res->addLineNumber(lexer.getLineNumber());

    //parse if method
    if (cur_tok == tok_opbr) {
        matchAndGoNext(tok_opbr);
        match(tok_identifier);
        auto name_of_struct = lexer.identifierStr();
        matchAndGoNext(tok_identifier);
        res->setMethod(name_of_struct, parseType());
        matchAndGoNext(tok_clbr);
    }

    // name of func
    match(tok_identifier);
    auto name = lexer.identifierStr();
    res->setName(name);
    cur_tok = lexer.gettok();

    //parameters and return types
    parseFuncSignature(res);

    //body of a function
    auto body = parseBlock();
    res->setBody(std::move(body));

    return res;

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
    int line_number = lexer.getLineNumber();

    auto values = parseExpressionList();

    if (type)
        res.push_back(std::make_unique<AST::ASTConstDeclaration>(std::move(names), std::move(values), std::move(type)));
    else
        res.push_back(std::make_unique<AST::ASTConstDeclaration>(std::move(names), std::move(values)));

    for (auto &i: res)
        i->addLineNumber(line_number);

    return res;
}


std::vector<std::unique_ptr<AST::ASTDeclaration>> Parser::parseTypeDeclarationLine() {
    std::vector<std::unique_ptr<AST::ASTDeclaration>> res;

    int line_number = lexer.getLineNumber();

    match(tok_identifier);
    std::vector<std::string> name = {lexer.identifierStr()};
    cur_tok = lexer.gettok();
    if (cur_tok == tok_assign)
        cur_tok = lexer.gettok();

    res.push_back(std::make_unique<AST::ASTTypeDeclaration>(std::move(name), parseType()));

    for (auto &i: res)
        i->addLineNumber(line_number);

    return res;
}

std::vector<std::unique_ptr<AST::ASTDeclaration>> Parser::parseVarDeclarationLine() {
    std::vector<std::unique_ptr<AST::ASTDeclaration>> res;

    int line_number;

    auto names = parseIdentifierList();


    if (cur_tok != tok_assign) {
        // if there is type before assign sign
        std::unique_ptr<AST::ASTType> type = nullptr;
        type = parseType();
        if (cur_tok == tok_assign) {
            //type and value
            matchAndGoNext(tok_assign);
            line_number = lexer.getLineNumber();

            auto values = parseExpressionList();

            res.push_back(
                    std::make_unique<AST::ASTVarDeclaration>(std::move(names), std::move(values), std::move(type)));

        } else
            // type without value
            res.push_back(std::make_unique<AST::ASTVarDeclaration>(std::move(names), std::move(type)));

    } else {
        //without type
        matchAndGoNext(tok_assign);
        line_number = lexer.getLineNumber();

        auto values = parseExpressionList();

        res.push_back(std::make_unique<AST::ASTVarDeclaration>(std::move(names), std::move(values)));

    }

    for (auto &i: res)
        i->addLineNumber(line_number);

    return res;
}


std::vector<std::unique_ptr<AST::ASTDeclaration>>
Parser::parseDeclarationBlock(const std::function<std::vector<std::unique_ptr<AST::ASTDeclaration>>()> &type_of_line) {
    std::vector<std::unique_ptr<AST::ASTDeclaration>> result;

    if (cur_tok == tok_opbr) {
        //multiple
        matchAndGoNext(tok_opbr);
        while (cur_tok != tok_clbr) {
            auto decl_line = type_of_line();
            for (auto &i: decl_line)
                result.push_back(std::move(i));
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
            result.push_back(std::move(i));
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