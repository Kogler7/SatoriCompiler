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

#include "ebnf.h"
#include "utils/log.h"

typedef string term;

// vector<token>::iterator findType(vector<token> &vec, vector<token>::iterator begin, int type)
// {
//     for (auto it = begin; it != vec.end(); it++)
//     {
//         if (it->type == type)
//         {
//             return it;
//         }
//     }
//     return vec.end();
// }

string lrtri(string str)
{
    return str.substr(1, str.length() - 2);
}

EBNFParser::EBNFParser(string ebnfLexPath)
{
    ebnfLexer = Lexer(ebnfLexPath);
}

Grammar EBNFParser::parse(string grammarPath)
{
    ebnfLexer.readSrcFile(grammarPath);
    tie(tokens, tokTypes) = ebnfLexer.tokenize();
    term startTerm;
    set<term> terminals;
    set<term> nonTerms;
    map<term, set<vector<term>>> rules;
    map<term, set<term>> first;
    map<term, set<term>> follow;
    token_type termType = tokTypes["TERMINAL"];
    token_type startType = tokTypes["START"];
    token_type nonTermType = tokTypes["NON_TERM"];
    token_type epsilonType = tokTypes["EPSILON"];
    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].type == termType)
        {
            term t = lrtri(tokens[i].value);
            tokens[i].value = t;
            terminals.insert(t);
        }
        else if (tokens[i].type == nonTermType)
        {
            nonTerms.insert(tokens[i].value);
        }
        else if (tokens[i].type == startType)
        {
            startTerm = tokens[i - 1].value;
            // 删除tokens中的起始符号
            tokens.erase(tokens.begin() + i--);
        }
    }
    terminals.insert(SYM_END);
    // 解析产生式，构造规则集，根据[]和{}构造相应的规则，消除左递归
    for (auto it = tokens.begin(); it != tokens.end(); it++)
    {
        if (it->type == nonTermType)
        {
            term left = it->value;
            it++;
            if (it->value != "::=")
            {
                error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line;
                cout << "Expected ::= but got " << it->value << endl;
                throw runtime_error("");
            }
            it++;
            while (it != tokens.end())
            {
                vector<term> right;
                vector<term> newRight;
                if (it->type == epsilonType)
                {
                    rules[left].insert(right);
                    it++;
                    continue;
                }
                while (it != tokens.end() && it->value != "|" && it->value != ";")
                {
                    debug(0) << "EBNFParser: Parsing " << it->value << endl;
                    if (it->type == termType)
                    {
                        right.push_back(it->value);
                    }
                    else if (it->type == nonTermType)
                    {
                        right.push_back(it->value);
                    }
                    else if (it->value == "[")
                    {
                        newRight = right;
                        it++;
                        while (it->value != "]")
                        {
                            debug(1) << "EBNFParser: Parsing[] " << it->value << endl;
                            if (it->type == termType)
                            {
                                right.push_back(it->value);
                            }
                            else if (it->type == nonTermType)
                            {
                                right.push_back(it->value);
                            }
                            else if (it->value == "[")
                            {
                                error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line << endl;
                                throw runtime_error("");
                            }
                            else if (it->value == "{")
                            {
                                error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line << endl;
                                throw runtime_error("");
                            }
                            else
                            {
                                error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line << endl;
                                throw runtime_error("");
                            }
                            it++;
                        }
                    }
                    else if (it->value == "{")
                    {
                        term newLeft = left + "'";
                        nonTerms.insert(newLeft);
                        right.push_back(newLeft);
                        vector<term> tmpRight;
                        it++;
                        while (it->value != "}")
                        {
                            if (it->type == termType)
                            {
                                right.push_back(it->value);
                            }
                            else if (it->type == nonTermType)
                            {
                                right.push_back(it->value);
                            }
                            else if (it->value == "[")
                            {
                                error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line << endl;
                                throw runtime_error("");
                            }
                            else if (it->value == "{")
                            {
                                error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line << endl;
                                throw runtime_error("");
                            }
                            else
                            {
                                error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line << endl;
                                throw runtime_error("");
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
                        error << "EBNFParser: Unexpected token " << it->value << " at line " << it->line << endl;
                        throw runtime_error("");
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
    return Grammar(startTerm, terminals, nonTerms, rules);
}
