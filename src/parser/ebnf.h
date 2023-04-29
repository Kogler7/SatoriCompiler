/**
 * @file ebnf.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief EBNF Parser
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "../common/token.h"
#include "gram.h"
#include "../lexer/lexer.h"

class EBNFParser
{
    Lexer ebnfLexer;
    vector<token> tokens;
    vector<string> tokTypes;

public:
    EBNFParser(string ebnfLexPath);
    Lexer &getLexer() { return ebnfLexer; }
    Grammar parse(string grammarPath);
};