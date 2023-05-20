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
#include "common/gram/basic.h"
#include "lexer/lexer.h"

typedef pair<symbol_t, vector<token>> tok_product_t;

class EBNFParser
{
    Lexer ebnfLexer;
    vector<token> tokens;
    Grammar grammar;
    void tokenizeSyntax(string grammarPath);
    vector<tok_product_t> segmentProduct(tok_product_t product);
    vector<tok_product_t> geneStxProducts(token_iter_t start, token_iter_t end);
    vector<tok_product_t> geneMapProducts(token_iter_t start, token_iter_t end);
    void addRules(vector<tok_product_t> &products);
    void addMappings(vector<tok_product_t> &products);

public:
    EBNFParser(string ebnfLexPath);
    Lexer &getLexer() { return ebnfLexer; }
    Grammar parse(string grammarPath);
};