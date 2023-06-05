/**
 * @file syntax.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Syntax Parser for EBNF
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/token.h"
#include "common/gram/basic.h"
#include "lexer/lexer.h"
#include "utils/meta.h"
#include "utils/view/viewer.h"

using tok_product_t = pair<symbol_t, vector<token>>;

class SyntaxParser
{
    Lexer ebnfLexer;
    Lexer mappingLexer;
    Grammar grammar;
    map<symbol_t, int> nonTermCount;

    vector<tok_product_t> tokProducts;
    vector<tok_product_t> segmentProduct(tok_product_t &product);
    void parseDeliProducts(vector<tok_product_t> &tmp, const symbol_t &left, token_const_iter_t beginIt, token_const_iter_t endIt);

    void addSyntaxRules(const vector<token> &tokens);
    void addTokenMappings(const vector<token> &tokens);
    void addSemanticActions();
    void addPrecRelations();
    void addAssociativity();

public:
    SyntaxParser(const string syntaxLexPath);
    Grammar parse(const string grammarPath);
};