/**
 * @file ebnf.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief EBNF Parser
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <map>
#include <set>
#include <vector>
#include <string>

#include "common/token.h"
#include "ebnf.h"
#include "utils/log.h"

#define DEBUG_LEVEL 0

using namespace std;

typedef string symbol_t;

token_iter_t findType(vector<token> &tokens, token_type_t type, token_iter_t start)
{
    for (token_iter_t it = start; it != tokens.end(); it++)
    {
        if (it->type == type)
        {
            return it;
        }
    }
    return tokens.end();
}

token_iter_t findDeli(vector<token> &tokens, string sep, token_iter_t start)
{
    token_type_t sepType = get_tok_type("DELIMITER");
    for (token_iter_t it = start; it != tokens.end(); it++)
    {
        if (it->type == sepType && it->value == sep)
        {
            return it;
        }
    }
    return tokens.end();
}

string lrtri(string str)
{
    return str.substr(1, str.length() - 2);
}

EBNFParser::EBNFParser(string ebnfLexPath)
{
    ebnfLexer = Lexer(ebnfLexPath);
}

void EBNFParser::tokenizeSyntax(string grammarPath)
{
    tokens = ebnfLexer.tokenize(grammarPath);
    ebnfLexer.printTokens(tokens);
}

symbol_t getEndDeli(symbol_t sep)
{
    if (sep == "(")
    {
        return ")";
    }
    else if (sep == "[")
    {
        return "]";
    }
    else if (sep == "{")
    {
        return "}";
    }
    else
    {
        error << "getEndDeli: Expected (, [, or { ." << endl;
        exit(1);
    }
}

void fullConnProducts(vector<tok_product_t> &dst, const vector<tok_product_t> &src, bool hasEpsilon = false)
{
    vector<tok_product_t> res;
    for (const auto &srcPro : src)
    {
        for (const auto &dstPro : dst)
        {
            vector<token> newRight = dstPro.second;
            newRight.insert(newRight.end(), srcPro.second.begin(), srcPro.second.end());
            res.push_back(make_pair(dstPro.first, newRight));
        }
    }
    if (hasEpsilon)
    {
        dst.insert(dst.end(), res.begin(), res.end());
    }
    else
    {
        dst = res;
    }
}

void EBNFParser::parseDeliProducts(vector<tok_product_t> &dst, vector<tok_product_t> &tmp, const symbol_t &left, token_iter_t beginIt, token_iter_t endIt)
{
    vector<tok_product_t> subProducts;
    tok_product_t tokProduct = make_pair(left, vector<token>(beginIt + 1, endIt));
    subProducts = segmentProduct(tokProduct);
    if (beginIt->value == "{")
    {
        // S -> A{B|D}C => S -> AB'C, B' -> B | ε, S -> AD'C, D' -> D | ε
        for (auto &pro : subProducts)
        {
            // 构造新的非终结符
            symbol_t newLeft = left + "_";
            newLeft += to_string(++nonTermCount[left]);
            token newLeftTok = token(get_tok_type("NON_TERM"), newLeft);
            // 构造含新非终结符的新产生式
            fullConnProducts(tmp, vector<tok_product_t>({make_pair(left, vector<token>({newLeftTok}))}));
            // 构造含右递归的新产生式
            vector<token> subRight = pro.second;
            subRight.push_back(newLeftTok);
            dst.push_back(make_pair(newLeft, subRight));
            // 构造含空串的新产生式
            dst.push_back(make_pair(newLeft, vector<token>()));
        }
    }
    else if (beginIt->value == "(" || beginIt->value == "[")
    {
        // S -> A[B|D]C => S -> ABC, S -> ADC, S -> AC
        // S -> A(B|D)C => S -> ABC, S -> ADC
        fullConnProducts(tmp, subProducts, beginIt->value == "[");
    }
    else
    {
        error << "EBNFParser: EBNF syntax error: Expected (, [, or { ." << endl;
        exit(1);
    }
}

vector<tok_product_t> EBNFParser::segmentProduct(tok_product_t &product)
{
    info << "EBNFParser: Segmenting product: " << product.first << " -> ";
    for (auto it = product.second.begin(); it != product.second.end(); it++)
    {
        cout << it->value << " ";
    }
    cout << endl;
    vector<tok_product_t> products;
    symbol_t &left = product.first;
    vector<token> &right = product.second;
    auto lastIt = right.begin();
    auto nextIt = right.begin();
    auto deliBeginIt = findType(right, get_tok_type("DELIMITER"), right.begin());
    if (deliBeginIt == right.end())
    {
        products.push_back(product);
        info << "EBNFParser: No delimiter found, return directly." << endl;
        return products;
    }
    while (lastIt != right.end())
    {
        if (deliBeginIt == right.end()) // 无分隔符
        {
            products.push_back(make_pair(left, vector<token>(lastIt, right.end())));
            break;
        }
        else if (deliBeginIt->value == "|")
        {
            // 如果是选择符，则把左边的产生式直接加入到结果中
            products.push_back(make_pair(left, vector<token>(lastIt, deliBeginIt)));
            nextIt = deliBeginIt;
        }
        else
        {
            // 如果是分组符，则需要递归拆分
            vector<tok_product_t> resProducts({make_pair(left, vector<token>())});
            vector<tok_product_t> leadProducts;

            while (deliBeginIt != right.end() && deliBeginIt->value != "|")
            {
                leadProducts = vector<tok_product_t>({make_pair(left, vector<token>(lastIt, deliBeginIt))});
                fullConnProducts(resProducts, leadProducts);

                symbol_t endDeli = getEndDeli(deliBeginIt->value);
                auto deliEndIt = findDeli(right, endDeli, deliBeginIt);
                assert(
                    deliEndIt != right.end(),
                    format(
                        "EBNFParser: EBNF syntax error: Expected $, got EOF at <$, $>",
                        endDeli, right.back().line, right.back().col));

                parseDeliProducts(products, resProducts, left, deliBeginIt, deliEndIt);
                lastIt = deliEndIt + 1;
                deliBeginIt = findType(right, get_tok_type("DELIMITER"), lastIt);
            }

            leadProducts = vector<tok_product_t>({make_pair(left, vector<token>(lastIt, deliBeginIt))});
            fullConnProducts(resProducts, leadProducts);

            nextIt = deliBeginIt;
            products.insert(products.end(), resProducts.begin(), resProducts.end());
        }
        if (nextIt == right.end())
        {
            break;
        }
        lastIt = nextIt + 1;
        info << "EBNFParser: lastIt: " << lastIt->value << endl;
        if (nextIt != right.end())
        {
            deliBeginIt = findType(right, get_tok_type("DELIMITER"), lastIt);
        }
    }
    info << "EBNFParser: Segmented product: " << endl;
    for (auto &pro : products)
    {
        cout << pro.first << " -> ";
        for (auto &tok : pro.second)
        {
            cout << tok.value << " ";
        }
        cout << endl;
    }
    return products;
}

vector<tok_product_t> EBNFParser::geneStxProducts(token_iter_t start, token_iter_t end)
{
    vector<tok_product_t> products;
    token_type_t grammarDef = get_tok_type("GRAMMAR_DEF");
    token_type_t sepType = get_tok_type("SEPARATOR");
    token_type_t nonTermType = get_tok_type("NON_TERM");
    token_type_t epsilonType = get_tok_type("EPSILON");
    // 预处理，拆分 ; 表示的多个产生式
    vector<tok_product_t> rawProducts;
    for (auto it = start; it != end; it++)
    {
        assert(
            it->type == nonTermType,
            format(
                "EBNFParser: EBNF syntax error: Expected non-terminal, got $ at <$, $>.",
                it->value, it->line, it->col));
        symbol_t left = it->value;
        it++;
        assert(
            it != end && it->type == grammarDef,
            format(
                "EBNFParser: EBNF syntax error: Expected ::=, got $ at <$, $>.",
                it->value, it->line, it->col));
        it++;
        assert(it != end);
        token_iter_t sepIt = findType(tokens, sepType, it);
        vector<token> right;
        for (auto it2 = it; it2 != sepIt; it2++)
        {
            right.push_back(*it2);
        }
        rawProducts.push_back(make_pair(left, right));
        it = sepIt;
    }
    // 进一步拆分
    for (auto &pro : rawProducts)
    {
        // 递归下降法解析拆分EBNF
        vector<tok_product_t> subProducts = segmentProduct(pro);
        products.insert(products.end(), subProducts.begin(), subProducts.end());
    }
    // 处理产生式中的 ε
    for (auto &pro : products)
    {
        if (pro.second.size() == 1 && pro.second[0].type == epsilonType)
        {
            pro.second.clear();
        }
    }
    info << "EBNFParser::geneStxProducts: " << products.size() << " productions generated." << endl;
    for (auto &pro : products)
    {
        debug(0) << pro.first << " -> ";
        for (auto &tok : pro.second)
        {
            debug_u(0) << tok.value << " ";
        }
        debug_u(0) << endl;
    }
    // 删去tok_production中的token额外信息，转换为production
    vector<product_t> &gProducts = grammar.products;
    token_type_t termType = get_tok_type("TERMINAL");
    token_type_t mulTermType = get_tok_type("MUL_TERM");
    for (auto &pro : products)
    {
        product_t newPro;
        newPro.first = pro.first;
        for (auto &tok : pro.second)
        {
            if (tok.type == termType)
            {
                tok.value = lrtri(tok.value);
                newPro.second.push_back(tok.value);
            }
            else if (tok.type == mulTermType)
            {
                tok.value = tok.value.substr(1);
                newPro.second.push_back(tok.value);
            }
            else
                newPro.second.push_back(tok.value);
        }
        gProducts.push_back(newPro);
    }
    return products;
}

vector<tok_product_t> EBNFParser::geneMapProducts(token_iter_t start, token_iter_t end)
{
    vector<tok_product_t> products;
    token_type_t mappingDef = get_tok_type("MAPPING_DEF");
    token_type_t sepType = get_tok_type("SEPARATOR");
    token_type_t mulTermType = get_tok_type("MUL_TERM");
    token_type_t epsilonType = get_tok_type("EPSILON");
    // 预处理，拆分 ; 表示的多个产生式
    vector<tok_product_t> rawProducts;
    for (auto it = start; it != end; it++)
    {
        assert(
            it->type == mulTermType,
            format(
                "EBNFParser: MAPPING syntax error: Expected mul-terminal, got $ at <$, $>.",
                it->value, it->line, it->col));
        symbol_t left = it->value;
        it++;
        assert(
            it->type == mappingDef,
            format(
                "EBNFParser: MAPPING syntax error: Expected ->, got $ at <$, $>.",
                it->value, it->line, it->col));
        it++;
        token_iter_t sepIt = findType(tokens, sepType, it);
        vector<token> right;
        for (auto it2 = it; it2 != sepIt; it2++)
        {
            right.push_back(*it2);
        }
        rawProducts.push_back(make_pair(left, right));
        it = sepIt;
    }
    // 进一步拆分
    for (auto &pro : rawProducts)
    {
        vector<tok_product_t> subProducts = segmentProduct(pro);
        products.insert(products.end(), subProducts.begin(), subProducts.end());
    }
    info << "EBNFParser::geneMapProducts: " << products.size() << " productions generated." << endl;
    for (auto &pro : products)
    {
        debug(0) << pro.first << " -> ";
        for (auto &tok : pro.second)
        {
            debug_u(0) << tok.value << " ";
        }
        debug_u(0) << endl;
    }
    return products;
}

void EBNFParser::addRules(vector<tok_product_t> &products)
{
    token_type_t nonTermType = get_tok_type("NON_TERM");
    token_type_t termType = get_tok_type("TERMINAL");
    token_type_t mulTermType = get_tok_type("MUL_TERM");
    symset_t &nonTerms = grammar.nonTerms;
    symset_t &terminals = grammar.terminals;
    map<symbol_t, set<symstr_t>> &rules = grammar.rules;
    for (auto &pro : products)
    {
        nonTerms.insert(pro.first);
        symstr_t right;
        for (auto &tok : pro.second)
        {
            if (tok.type == nonTermType)
            {
                nonTerms.insert(tok.value);
            }
            else if (tok.type == mulTermType)
            {
                terminals.insert(tok.value);
            }
            else if (tok.type == termType)
            {
                terminals.insert(tok.value);
            }
            right.push_back(tok.value);
        }
        rules[pro.first].insert(right);
    }
}

void EBNFParser::addMappings(vector<tok_product_t> &products)
{
    token_type_t tokType = get_tok_type("TOK_TYPE");
    map<token_type_t, symbol_t> &tok2sym = grammar.tok2sym;
    for (auto &pro : products)
    {
        symbol_t left = pro.first.substr(1);
        vector<token> &right = pro.second;
        assert(
            right.size() == 1,
            format(
                "EBNFParser: Mapping syntax error: Expected only one token-type term, got $ at <$, $>.",
                right.size(), right[0].line, right[0].col));
        assert(
            right[0].type == tokType,
            format(
                "EBNFParser: Mapping syntax error: Expected token type, got $ at <$, $>.",
                right[0].value, right[0].line, right[0].col));
        symbol_t tokTerm = right[0].value;
        tokTerm = tokTerm.substr(1);
        grammar.terminals.insert(left);
        if (!find_tok_type(tokTerm))
        {
            warn << "EBNFParser: Mapping syntax warn: Undefined token type " << tokTerm << endl;
            set_tok_type(tokTerm, make_tok_type(tokTerm));
        }
        tok2sym[get_tok_type(tokTerm)] = left;
    }
}

Grammar EBNFParser::parse(string grammarPath)
{
    tokens = ebnfLexer.tokenize(grammarPath);
    ebnfLexer.printTokens(tokens);
    // 解析开始符号
    auto startIt = findType(tokens, get_tok_type("START_MRK"), tokens.begin());
    assert(startIt != tokens.end(), "EBNFParser: No start symbol defined.");
    grammar.symStart = (startIt - 1)->value;
    startIt = tokens.erase(startIt);
    startIt = findType(tokens, get_tok_type("START_MRK"), tokens.begin());
    assert(startIt == tokens.end(), "EBNFParser: Multiple start symbols defined.");
    info << "EBNFParser: Start symbol is " << grammar.symStart << endl;
    // 解析EBNF定义的文法
    auto grammarIt = findType(tokens, get_tok_type("GRAMMAR"), tokens.begin());
    assert(grammarIt != tokens.end(), "EBNFParser: No grammar defined.");
    auto blkStart = findType(tokens, get_tok_type("BLOCK_SRT"), grammarIt);
    assert(blkStart != tokens.end(), "EBNFParser: Grammar block not found.");
    auto blkEnd = findType(tokens, get_tok_type("BLOCK_END"), blkStart);
    assert(blkEnd != tokens.end(), "EBNFParser: Grammar block not ended.");
    vector<tok_product_t> products = geneStxProducts(blkStart + 1, blkEnd);
    addRules(products);
    // 解析MAPPING映射
    auto mappingIt = findType(tokens, get_tok_type("MAPPING"), tokens.begin());
    if (mappingIt == tokens.end())
    {
        warn << "EBNFParser: No mapping defined." << endl;
        return grammar;
    }
    blkStart = findType(tokens, get_tok_type("BLOCK_SRT"), mappingIt);
    assert(blkStart != tokens.end(), "EBNFParser: Mappings block not found.");
    blkEnd = findType(tokens, get_tok_type("BLOCK_END"), blkStart);
    assert(blkEnd != tokens.end(), "EBNFParser: Mappings block not ended.");
    products = geneMapProducts(blkStart + 1, blkEnd);
    addMappings(products);
    return grammar;
}
