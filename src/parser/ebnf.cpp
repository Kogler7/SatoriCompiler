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

#define _is_term_type(t) (t == termType || t == mulTermType || t == nonTermType)

typedef string term;

using namespace std;

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
    token_type sepType = get_tok_type("SEPARATOR");
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
        error << "EBNFParser: EBNF syntax error: Expected (, [, or { ." << endl;
        exit(1);
    }
}

vector<production> EBNFParser::segmentProduct(production product)
{
    vector<production> products;
    term &left = product.first;
    vector<token> &right = product.second;
    auto lastIt = right.begin();
    auto nextIt = right.begin();
    auto sepBeginIt = findType(right, get_tok_type("SEPARATOR"), right.begin());
    if (sepBeginIt == right.end()) // 无分隔符
    {
        products.push_back(product);
        return products;
    }
    // 有分隔符
    while (lastIt != right.end())
    {
        if (sepBeginIt->value == "|")
        {
            // 如果是选择符，则把左边的产生式直接加入到结果中
            products.push_back(make_pair(left, vector<token>(lastIt, sepBeginIt)));
            nextIt = sepBeginIt + 1;
        }
        else
        {
            // 如果是分组符，则需要递归拆分
            term endSep = getEndSep(sepBeginIt->value);
            auto sepEndIt = findSep(right, endSep, sepBeginIt);
            if (sepEndIt == right.end())
            {
                error << "EBNFParser: EBNF syntax error: Expected " << endSep << ", got EOF";
                cout << " at <" << right.back().line << ", " << right.back().col << ">" << endl;
                exit(1);
            }
            nextIt = findType(right, get_tok_type("SEPARATOR"), sepEndIt + 1);
            if (nextIt != right.end() && nextIt->value != "|")
            {
                // 如果分组后面有非选择符的分隔符，则报错（不支持多种分隔符排列组合）
                error << "EBNFParser: EBNF syntax error: Expected |, got " << nextIt->value;
                cout << " at <" << nextIt->line << ", " << nextIt->col << ">" << endl;
                exit(1);
            }
            vector<production> subProducts;
            subProducts = segmentProduct(make_pair(left, vector<token>(sepBeginIt + 1, sepEndIt)));
            vector<token> head = vector<token>(lastIt, sepBeginIt);
            vector<token> tail = vector<token>(sepEndIt + 1, nextIt);
            if (sepBeginIt->value == "{")
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
                }
                // 构造含空串的新产生式
                products.push_back(make_pair(left, vector<token>()));
            }
            else if (sepBeginIt->value == "(" || sepBeginIt->value == "[")
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
                if (sepBeginIt->value == "[")
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
        lastIt = nextIt;
        if (nextIt != right.end())
        {
            sepBeginIt = findType(right, get_tok_type("SEPARATOR"), nextIt);
        }
    }
}

vector<production> EBNFParser::generateProducts(token_iter start, token_iter end)
{
    vector<production> products;
    token_type grammarDef = get_tok_type("GRAMMAR_DEF");
    token_type mappingDef = get_tok_type("MAPPING_DEF");
    token_type sepType = get_tok_type("SEPARATOR");
    token_type nonTermType = get_tok_type("NON_TERM");
    token_type epsilonType = get_tok_type("EPSILON");
    // 预处理，拆分 ; 表示的多个产生式
    vector<production> rawProducts;
    for (auto it = start; it != end; it++)
    {
        if (it->type != nonTermType)
        {
            error << "EBNFParser: EBNF syntax error: Expected non-terminal, got " << it->value;
            cout << " at <" << it->line << ", " << it->col << ">" << endl;
            exit(1);
        }
        term left = it->value;
        it++;
        if (it->type != grammarDef && it->type != mappingDef)
        {
            error << "EBNFParser: EBNF syntax error: Expected ::= or ->, got " << it->value;
            cout << " at <" << it->line << ", " << it->col << ">" << endl;
            exit(1);
        }
        it++;
        token_iter sepIt = findType(tokens, sepType, it);
        vector<token> right;
        for (auto it2 = it; it2 != sepIt; it2++)
        {
            right.push_back(*it2);
        }
        rawProducts.push_back(make_pair(left, right));
    }
    // 进一步拆分
    for (auto &pro : rawProducts)
    {
        vector<production> subProducts = segmentProduct(pro);
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
    return products;
}

void EBNFParser::generateRules(vector<production> &products)
{
    token_type nonTermType = get_tok_type("NON_TERM");
    token_type termType = get_tok_type("TERMINAL");
    token_type mulTermType = get_tok_type("MUL_TERM");
    set<term> &nonTerms = grammar.nonTerms;
    set<term> &terminals = grammar.terminals;
    map<term, set<vector<term>>> &rules = grammar.rules;
    for (auto &pro : products)
    {
        vector<term> right;
        for (auto &tok : pro.second)
        {
            if (tok.type == nonTermType)
            {
                nonTerms.insert(tok.value);
            }
            else if (tok.type == termType || tok.type == mulTermType)
            {
                terminals.insert(tok.value);
            }
            right.push_back(tok.value);
        }
        rules[pro.first].insert(right);
    }
}

void EBNFParser::generateMappings(vector<production> &products)
{
    token_type tokType = get_tok_type("TOK_TYPE");
    map<token_type, term> &tok2term = grammar.tok2term;
    for (auto &pro : products)
    {
        term left = pro.first;
        vector<token> &right = pro.second;
        if (right.size() == 1 && right[0].type == tokType)
        {
            term tokTerm = right[0].value;
            tokTerm = tokTerm.substr(1);
            if (!find_tok_type(tokTerm))
            {
                error << "EBNFParser: Mapping syntax error: Undefined token type " << tokTerm << endl;
                exit(1);
            }
            tok2term[get_tok_type(tokTerm)] = left;
        }
        else
        {
            error << "EBNFParser: Mapping syntax error: Expected token type, got " << right[0].value;
            cout << " at <" << right[0].line << ", " << right[0].col << ">" << endl;
            exit(1);
        }
    }
}

Grammar EBNFParser::parse(string grammarPath)
{
    ebnfLexer.readSrcFile(grammarPath);
    tokens = ebnfLexer.tokenize();
    ebnfLexer.printTokens();
    set<term> terminals;
    set<term> nonTerms;
    map<token_type, term> tok2term;
    map<term, set<vector<term>>> rules;
    map<term, set<term>> first;
    map<term, set<term>> follow;
    // 解析Grammar
    // 找到开始符号
    auto startIt = findType(tokens, get_tok_type("START_MRK"), tokens.begin());
    term startTerm = (startIt - 1)->value;
    info << "EBNFParser: Start symbol is " << startTerm << endl;
    tokens.erase(startIt);
    // 找到语法块
    auto gIt = findType(tokens, get_tok_type("GRAMMAR"), tokens.begin());
    auto blkStart = findType(tokens, get_tok_type("BLOCK_SRT"), gIt);
    auto blkEnd = findType(tokens, get_tok_type("BLOCK_END"), blkStart);
    token_type grammarDef = get_tok_type("GRAMMAR_DEF");
    token_type termType = get_tok_type("TERMINAL");
    token_type mulTermType = get_tok_type("MUL_TERM");
    token_type nonTermType = get_tok_type("NON_TERM");
    token_type epsilonType = get_tok_type("EPSILON");
    // 解析终结符和非终结符
    for (auto it = blkStart + 1; it != blkEnd; it++)
    {
        if (it->type == termType)
        {
            term t = lrtri(it->value);
            it->value = t;
            terminals.insert(t);
            tok2term[it->type] = t;
        }
        else if (it->type == mulTermType)
        {
            term t = it->value.substr(1); // 去掉前导的$
            it->value = t;
            terminals.insert(t);
        }
        else if (it->type == nonTermType)
        {
            nonTerms.insert(it->value);
        }
    }
    terminals.insert(SYM_END);
    // 解析产生式，构造规则集，根据(), []和{}构造相应的规则，消除左递归
    for (auto it = blkStart + 1; it != blkEnd; it++)
    {
        // 对于每一个非终结符定义，构造规则集
        if (it->type == nonTermType)
        {
            term left = it->value;
            it++;
            if (it->type != grammarDef)
            {
                error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line;
                cout << ", expected ::= but got " << it->value << endl;
                exit(1);
            }
            it++;
            while (it != blkEnd)
            {
                vector<term> right;    // 产生式右部
                vector<term> newRight; // 新产生式右部
                if (it->type == epsilonType)
                {
                    rules[left].insert(right);
                    it++;
                    continue;
                }
                // 解析右部，直到遇到|或者;
                while (it != blkEnd && it->value != "|" && it->value != ";")
                {
                    debug(0) << "EBNFParser: Parsing " << it->value << endl;
                    if (_is_term_type(it->type))
                    {
                        right.push_back(it->value);
                    }
                    // 解析 [] （出现零次或一次）
                    else if (it->value == "[")
                    {
                        newRight = right;
                        it++;
                        while (it->value != "]")
                        {
                            debug(1) << "EBNFParser: Parsing[] " << it->value << endl;
                            if (it->type == termType || it->type == nonTermType)
                            {
                                right.push_back(it->value);
                            }
                            else
                            {
                                error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line << endl;
                                exit(1);
                            }
                            it++;
                        }
                    }
                    // 解析 {} （出现零次或多次）
                    else if (it->value == "{")
                    {
                        term newLeft = left + "'";
                        nonTerms.insert(newLeft);
                        right.push_back(newLeft);
                        vector<term> tmpRight;
                        it++;
                        while (it->value != "}")
                        {
                            if (_is_term_type(it->type))
                            {
                                right.push_back(it->value);
                            }
                            else
                            {
                                error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line << endl;
                                exit(1);
                            }
                            it++;
                        }
                        tmpRight.push_back(newLeft);
                        rules[newLeft].insert(tmpRight);
                        debug(0) << "EBNFParser: inserted " << left << "::=";
                        for (auto it2 = tmpRight.begin(); it2 != tmpRight.end(); it2++)
                        {
                            cout << *it2 << " ";
                        }
                        cout << endl;
                        rules[newLeft].insert(vector<term>());
                    }
                    else
                    {
                        error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line;
                        cout << ", expected a term or non-term but got " << it->value << endl;
                        exit(1);
                    }
                    it++;
                }
                rules[left].insert(right);
                debug(0) << "EBNFParser: inserted " << left << "::=";
                for (auto it2 = right.begin(); it2 != right.end(); it2++)
                {
                    cout << *it2 << " ";
                }
                cout << endl;
                if (newRight.size() > 0)
                {
                    rules[left].insert(newRight);
                    debug(0) << "EBNFParser: inserted " << left << "::=";
                    for (auto it2 = newRight.begin(); it2 != newRight.end(); it2++)
                    {
                        cout << *it2 << " ";
                    }
                    cout << endl;
                }
                if (it->value == ";")
                {
                    break;
                }
                it++;
            }
        }
    }
    // 解析MAPPING映射
    auto mIt = findType(tokens, get_tok_type("MAPPING"), tokens.begin());
    blkStart = findType(tokens, get_tok_type("BLOCK_SRT"), mIt);
    blkEnd = findType(tokens, get_tok_type("BLOCK_END"), blkStart);
    token_type mappingDef = get_tok_type("MAPPING_DEF");
    token_type tokMarkType = get_tok_type("TOK_TYPE");
    // for (auto it = blkStart + 1; it != blkEnd; it++)
    // {
    //     if (it->type == mappingDef)
    //     {
    //         it++;
    //         if (it->type != tokMarkType)
    //         {
    //             error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line;
    //             cout << ", expected a token type but got " << it->value << endl;
    //             exit(1);
    //         }
    //         token_type tokType = get_tok_type(it->value);
    //         it++;
    //         if (it->type != termType)
    //         {
    //             error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line;
    //             cout << ", expected a term but got " << it->value << endl;
    //             exit(1);
    //         }
    //         term t = it->value;
    //         tok2term[tokType] = t;
    //     }
    // }
    return Grammar(startTerm, terminals, nonTerms, rules, tok2term);
}
