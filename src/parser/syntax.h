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

// 词素产生式，用于存储解析出的词素产生式，具有产生式的结构，但是右部的符号是词素
using tok_product_t = pair<symbol_t, vector<token>>;

class SyntaxParser
{
    MetaParser syntaxMeta;           // 元信息解析器
    Lexer ebnfLexer;                 // EBNF词法解析器
    Lexer mappingLexer;              // 映射词法解析器
    Lexer precLexer;                 // 优先级词法解析器
    Grammar grammar;                 // 语法解析结果
    map<symbol_t, int> nonTermCount; // 非终结符计数器，用于程序自动生成唯一的新非终结符

    vector<tok_product_t> tokProducts; // 用于存储解析出的词素产生式
    vector<tok_product_t> segmentProduct(tok_product_t &product);
    void parseNonTrivialProducts(vector<tok_product_t> &tmp, const symbol_t &left, token_const_iter_t beginIt, token_const_iter_t endIt);

    void addSyntaxRules(const vector<token> &tokens);
    void addTokenMappings(const vector<token> &tokens);
    void addPrecAndAssoc();

public:
    SyntaxParser(const string syntaxLexPath);
    Grammar parse(const string grammarPath);
};