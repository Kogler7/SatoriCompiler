/**
 * @file rdp.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Recursive Descent Parsing
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "rdp.h"
#include "utils/log.h"
#include <fstream>

RDPParser::RDPParser(Grammar &grammar)
{
    this->grammar = grammar;
}

bool RDPParser::parseSymbol(TokenViewer &tokens, term t)
{
    // debug(0) << "Parsing symbol " << t << endl;
    // if (tokens.empty())
    // {
    //     error << "RDPParser: Unexpected end of input" << endl;
    //     return false;
    // }
    // token tok = tokens.peek();
    // if (tok.type == TokenType::SYMBOL && tok.value == t)
    // {
    //     tokens.next();
    //     return true;
    // }
    // else
    // {
    //     error << "RDPParser: Unexpected token " << tok.value << " at line " << tok.line << " column " << tok.column << endl;
    //     return false;
    // }
    return true;
}

bool RDPParser::parse(vector<token> &tokens, vector<TokenType> &tok_types)
{
    info << "RDPParser: Parsing..." << endl;
    // TokenViewer tokens(tokens);
    return true;
}