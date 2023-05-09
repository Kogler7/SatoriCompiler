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

typedef string term;

token_iter findType(vector<token> &tokens, token_type type, token_iter start)
{
    for (token_iter it = start; it != tokens.end(); it++)
    {
        if (it->type == type)
        {
            return it;
        }
    }
    return tokens.end();
}

token_iter findSep(vector<token> &tokens, string sep, token_iter start)
{
    token_type sepType = get_tok_type("DELIMITER");
    for (token_iter it = start; it != tokens.end(); it++)
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
    ebnfLexer.readSrcFile(grammarPath);
    tokens = ebnfLexer.tokenize();
    ebnfLexer.printTokens();
}

term getEndSep(term sep)
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
        error << "getEndSep: Expected (, [, or { ." << endl;
        exit(1);
    }
}

vector<tok_production> EBNFParser::segmentProduct(tok_production product)
{
    info << "EBNFParser: Segmenting product: " << product.first << " -> ";
    for (auto it = product.second.begin(); it != product.second.end(); it++)
    {
        cout << it->value << " ";
    }
    cout << endl;
    vector<tok_production> products;
    term &left = product.first;
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
            term endDeli = getEndSep(deliBeginIt->value);
            auto deliEndIt = findSep(right, endDeli, deliBeginIt);
            assert(
                deliEndIt != right.end(),
                format(
                    "EBNFParser: EBNF syntax error: Expected $, got EOF at <$, $>",
                    endDeli, right.back().line, right.back().col));
            nextIt = findType(right, get_tok_type("DELIMITER"), deliEndIt + 1);
            assert( // 如果分组后面有非选择符的分隔符，则报错（不支持多种分隔符排列组合）
                nextIt == right.end() || nextIt->value == "|",
                format(
                    "EBNFParser: EBNF syntax error: Expected |, got $ at <$, $>",
                    nextIt->value, nextIt->line, nextIt->col));
            vector<tok_production> subProducts;
            subProducts = segmentProduct(make_pair(left, vector<token>(deliBeginIt + 1, deliEndIt)));
            vector<token> head = vector<token>(lastIt, deliBeginIt);
            vector<token> tail = vector<token>(deliEndIt + 1, nextIt);
            if (deliBeginIt->value == "{")
            {
                // S -> A{B|D}C => S -> AB'C, B' -> B | ε, S -> AD'C, D' -> D | ε
                int nonTermCount = 0;
                for (auto &pro : subProducts)
                {
                    // 构造新的非终结符
                    term newLeft = left + "_";
                    newLeft += to_string(++nonTermCount);
                    token newLeftTok = token(get_tok_type("NON_TERM"), newLeft);
                    // 构造含新非终结符的新产生式
                    vector<token> newRight;
                    newRight.insert(newRight.end(), head.begin(), head.end());
                    newRight.push_back(newLeftTok);
                    newRight.insert(newRight.end(), tail.begin(), tail.end());
                    products.push_back(make_pair(left, newRight));
                    // 构造含右递归的新产生式
                    vector<token> subRight = pro.second;
                    subRight.push_back(newLeftTok);
                    products.push_back(make_pair(newLeft, subRight));
                    // 构造含空串的新产生式
                    products.push_back(make_pair(newLeft, vector<token>()));
                }
            }
            else if (deliBeginIt->value == "(" || deliBeginIt->value == "[")
            {
                // S -> A[B|D]C => S -> ABC, S -> ADC, S -> AC
                // S -> A(B|D)C => S -> ABC, S -> ADC
                for (auto &pro : subProducts)
                {
                    // S -> ABC, S -> ADC
                    vector<token> newRight;
                    newRight.insert(newRight.end(), head.begin(), head.end());
                    newRight.insert(newRight.end(), pro.second.begin(), pro.second.end());
                    newRight.insert(newRight.end(), tail.begin(), tail.end());
                    products.push_back(make_pair(left, newRight));
                }
                if (deliBeginIt->value == "[")
                {
                    // S -> AC
                    vector<token> newRight;
                    newRight.insert(newRight.end(), head.begin(), head.end());
                    newRight.insert(newRight.end(), tail.begin(), tail.end());
                    products.push_back(make_pair(left, newRight));
                }
            }
            else
            {
                error << "EBNFParser: EBNF syntax error: Expected (, [, or { ." << endl;
                exit(1);
            }
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

vector<tok_production> EBNFParser::geneStxProducts(token_iter start, token_iter end)
{
    vector<tok_production> products;
    token_type grammarDef = get_tok_type("GRAMMAR_DEF");
    token_type sepType = get_tok_type("SEPARATOR");
    token_type nonTermType = get_tok_type("NON_TERM");
    token_type epsilonType = get_tok_type("EPSILON");
    // 预处理，拆分 ; 表示的多个产生式
    vector<tok_production> rawProducts;
    for (auto it = start; it != end; it++)
    {
        assert(
            it->type == nonTermType,
            format(
                "EBNFParser: EBNF syntax error: Expected non-terminal, got $ at <$, $>.",
                it->value, it->line, it->col));
        term left = it->value;
        it++;
        assert(
            it != end && it->type == grammarDef,
            format(
                "EBNFParser: EBNF syntax error: Expected ::=, got $ at <$, $>.",
                it->value, it->line, it->col));
        it++;
        assert(it != end);
        token_iter sepIt = findType(tokens, sepType, it);
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
        vector<tok_production> subProducts = segmentProduct(pro);
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
    vector<production> &gProducts = grammar.products;
    token_type termType = get_tok_type("TERMINAL");
    for (auto &pro : products)
    {
        production newPro;
        newPro.first = pro.first;
        for (auto &tok : pro.second)
        {
            if (tok.type == termType)
                newPro.second.push_back(lrtri(tok.value));
            else
                newPro.second.push_back(tok.value);
        }
        gProducts.push_back(newPro);
    }
    return products;
}

vector<tok_production> EBNFParser::geneMapProducts(token_iter start, token_iter end)
{
    vector<tok_production> products;
    token_type mappingDef = get_tok_type("MAPPING_DEF");
    token_type sepType = get_tok_type("SEPARATOR");
    token_type mulTermType = get_tok_type("MUL_TERM");
    token_type epsilonType = get_tok_type("EPSILON");
    // 预处理，拆分 ; 表示的多个产生式
    vector<tok_production> rawProducts;
    for (auto it = start; it != end; it++)
    {
        assert(
            it->type == mulTermType,
            format(
                "EBNFParser: MAPPING syntax error: Expected mul-terminal, got $ at <$, $>.",
                it->value, it->line, it->col));
        term left = it->value;
        it++;
        assert(
            it->type == mappingDef,
            format(
                "EBNFParser: MAPPING syntax error: Expected ->, got $ at <$, $>.",
                it->value, it->line, it->col));
        it++;
        token_iter sepIt = findType(tokens, sepType, it);
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
        vector<tok_production> subProducts = segmentProduct(pro);
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

void EBNFParser::addRules(vector<tok_production> &products)
{
    token_type nonTermType = get_tok_type("NON_TERM");
    token_type termType = get_tok_type("TERMINAL");
    token_type mulTermType = get_tok_type("MUL_TERM");
    set<term> &nonTerms = grammar.nonTerms;
    set<term> &terminals = grammar.terminals;
    map<term, set<vector<term>>> &rules = grammar.rules;
    for (auto &pro : products)
    {
        nonTerms.insert(pro.first);
        vector<term> right;
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
                tok.value = lrtri(tok.value);
                terminals.insert(tok.value);
            }
            right.push_back(tok.value);
        }
        rules[pro.first].insert(right);
    }
}

void EBNFParser::addMappings(vector<tok_production> &products)
{
    token_type tokType = get_tok_type("TOK_TYPE");
    map<token_type, term> &tok2term = grammar.tok2term;
    for (auto &pro : products)
    {
        term left = pro.first;
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
        term tokTerm = right[0].value;
        tokTerm = tokTerm.substr(1);
        if (!find_tok_type(tokTerm))
        {
            warn << "EBNFParser: Mapping syntax warn: Undefined token type " << tokTerm << endl;
            set_tok_type(tokTerm, make_tok_type(tokTerm));
        }
        tok2term[get_tok_type(tokTerm)] = left;
    }
}

Grammar EBNFParser::parse(string grammarPath)
{
    ebnfLexer.readSrcFile(grammarPath);
    tokens = ebnfLexer.tokenize();
    ebnfLexer.printTokens();
    // 解析开始符号
    auto startIt = findType(tokens, get_tok_type("START_MRK"), tokens.begin());
    assert(startIt != tokens.end(), "EBNFParser: No start symbol defined.");
    grammar.startTerm = (startIt - 1)->value;
    startIt = tokens.erase(startIt);
    startIt = findType(tokens, get_tok_type("START_MRK"), tokens.begin());
    assert(startIt == tokens.end(), "EBNFParser: Multiple start symbols defined.");
    info << "EBNFParser: Start symbol is " << grammar.startTerm << endl;
    // 解析EBNF定义的文法
    auto grammarIt = findType(tokens, get_tok_type("GRAMMAR"), tokens.begin());
    assert(grammarIt != tokens.end(), "EBNFParser: No grammar defined.");
    auto blkStart = findType(tokens, get_tok_type("BLOCK_SRT"), grammarIt);
    assert(blkStart != tokens.end(), "EBNFParser: Grammar block not found.");
    auto blkEnd = findType(tokens, get_tok_type("BLOCK_END"), blkStart);
    assert(blkEnd != tokens.end(), "EBNFParser: Grammar block not ended.");
    vector<tok_production> products = geneStxProducts(blkStart + 1, blkEnd);
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
