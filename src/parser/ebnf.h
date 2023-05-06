/**
 * @file ebnf.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief EBNF Parser
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/token.h"
#include "common/gram.h"
#include "lexer/lexer.h"

typedef pair<term, vector<token>> production;

class EBNFParser
{
    Lexer ebnfLexer;
    vector<token> tokens;
    Grammar grammar;
    void tokenizeSyntax(string grammarPath);
    vector<production> segmentProduct(production product);
    vector<production> geneStxProducts(token_iter start, token_iter end);
    vector<production> geneMapProducts(token_iter start, token_iter end);
    void addRules(vector<production> &products);
    void addMappings(vector<production> &products);

public:
    EBNFParser(string ebnfLexPath);
    Lexer &getLexer() { return ebnfLexer; }
    Grammar parse(string grammarPath);
};