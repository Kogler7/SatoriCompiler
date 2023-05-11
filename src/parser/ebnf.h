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

typedef pair<symbol, vector<token>> tok_production;

class EBNFParser
{
    Lexer ebnfLexer;
    vector<token> tokens;
    Grammar grammar;
    void tokenizeSyntax(string grammarPath);
    vector<tok_production> segmentProduct(tok_production product);
    vector<tok_production> geneStxProducts(token_iter start, token_iter end);
    vector<tok_production> geneMapProducts(token_iter start, token_iter end);
    void addRules(vector<tok_production> &products);
    void addMappings(vector<tok_production> &products);

public:
    EBNFParser(string ebnfLexPath);
    Lexer &getLexer() { return ebnfLexer; }
    Grammar parse(string grammarPath);
};