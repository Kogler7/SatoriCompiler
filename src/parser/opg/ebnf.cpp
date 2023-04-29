/**
 * @file ebnf.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief EBNF Parser
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#define _find(x, y) (x.find(y) != x.end())

#define EPSILON "@"
#define END "#" // 用于表示输入串结束
#include "ebnf.h"

vector<token>::iterator findType(vector<token> &vec, vector<token>::iterator begin, int type)
{
    for (auto it = begin; it != vec.end(); it++)
    {
        if (it->type == type)
        {
            return it;
        }
    }
    return vec.end();
}

string lrtri(string str)
{
    return str.substr(1, str.length() - 2);
}

EBNFParser::EBNFParser(vector<token> ebnf_tokens, vector<string> tok_types) : tokens(ebnf_tokens), tokTypes(tok_types)
{
    for (int i = 0; i < tokTypes.size(); i++)
    {
        if (tokTypes[i] == "TERMINAL")
        {
            termType = i;
        }
        else if (tokTypes[i] == "NON_TERM")
        {
            nonTermType = i;
        }
        else if (tokTypes[i] == "EPSILON")
        {
            epsilonType = i;
        }
        else if (tokTypes[i] == "START")
        {
            startType = i;
        }
    }
    if (termType == -1 || nonTermType == -1)
    {
        error << "EBNFParser: No TERMINAL or NON_TERM type found!" << endl;
        throw runtime_error("EBNFParser: No TERMINAL or NON_TERM type found!");
    }
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
            non_terms.insert(tokens[i].value);
        }
        else if (tokens[i].type == startType)
        {
            startTerm = tokens[i - 1].value;
            // 删除tokens中的起始符号
            tokens.erase(tokens.begin() + i--);
        }
    }
    terminals.insert(END);
}

void EBNFParser::printRules()
{
    for (auto it = rules.begin(); it != rules.end(); it++)
    {
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            cout << it->first << " ::= ";
            for (auto it3 = it2->begin(); it3 != it2->end(); it3++)
            {
                cout << *it3 << " ";
            }
            cout << endl;
        }
    }
}

void EBNFParser::parseRules()
{
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
                    cout << it->value << endl;
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
                            cout << "[] " << it->value << endl;
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
                        non_terms.insert(newLeft);
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
                        cout << "inserted " << left << "::=";
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
                cout << "inserted " << left << "::=";
                for (auto it2 = right.begin(); it2 != right.end(); it2++)
                {
                    cout << *it2 << " ";
                }
                cout << endl;
                if (newRight.size() > 0)
                {
                    rules[left].insert(newRight);
                    cout << "inserted " << left << "::=";
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
}

set<term> EBNFParser::calcFirstOf(term t)
{
    info << "Calculating First(" << t << ")" << endl;
    if (first[t].size() > 0)
    {
        info << "First(" << t << ") has been calculated before" << endl;
        return first[t]; // 已经计算过
    }
    // 对于每个产生式，计算first集
    for (auto &right : rules[t])
    {
        // 空产生式，加入epsilon
        if (right.size() == 0)
        {
            first[t].insert(EPSILON);
            info << "First(" << t << ") = {epsilon}" << endl;
        }
        else
        {
            // 对于产生式右部的每个符号
            bool allHaveEpsilon = true;
            for (auto &symbol : right)
            {
                if (_find(terminals, symbol))
                {
                    // 终结符，直接加入first集
                    first[t].insert(symbol);
                    info << "First(" << t << ") = {" << symbol << "}" << endl;
                    allHaveEpsilon = false;
                    break; // 终结符后面的符号不再计算
                }
                else
                {
                    set<term> resFirst = calcFirstOf(symbol);
                    set<term> tmpFirst = resFirst;
                    tmpFirst.erase(EPSILON);
                    first[t].insert(tmpFirst.begin(), tmpFirst.end());
                    for (auto &f : tmpFirst)
                    {
                        info << "First(" << t << ") = {" << f << "}" << endl;
                    }
                    if (!_find(resFirst, EPSILON))
                    {
                        // 该符号的first集不含epsilon，后面的符号不再计算
                        allHaveEpsilon = false;
                        break;
                    }
                }
            }
            if (allHaveEpsilon)
            {
                first[t].insert(EPSILON);
                info << "First(" << t << ") = {epsilon}" << endl;
            }
        }
    }
    return first[t];
}
set<term> EBNFParser::calcFollowOf(term t)
{
    info << "Calculating Follow(" << t << ")" << endl;
    if (follow[t].size() > 0)
    {
        info << "Follow(" << t << ") has been calculated before" << endl;
        return follow[t]; // 已经计算过
    }
    if (t == startTerm)
    {
        follow[t].insert(END);
        info << "Follow(" << t << ") = {#}" << endl;
    }
    for (auto &rule : rules)
    {
        for (auto &right : rule.second)
        {
            auto symIt = right.begin();
            symIt = find(symIt, right.end(), t);
            while (symIt != right.end())
            {
                if (symIt == right.end() - 1)
                {
                    // A -> aB
                    if (rule.first != t)
                    {
                        set<term> resFollow = calcFollowOf(rule.first);
                        follow[t].insert(resFollow.begin(), resFollow.end());
                        for (auto &f : resFollow)
                        {
                            info << "Follow(" << t << ") = {" << f << "} <A -> aB>" << endl;
                        }
                    }
                }
                else if (_find(terminals, *(symIt + 1)))
                {
                    // A -> aBb
                    follow[t].insert(*(symIt + 1));
                    info << "Follow(" << t << ") = {" << *(symIt + 1) << "} <A -> aBb>" << endl;
                }
                else
                {
                    // A -> aBC
                    set<term> resFirst;
                    for (auto tmpIt = symIt + 1; tmpIt != right.end(); tmpIt++)
                    {
                        resFirst = calcFirstOf(*tmpIt);
                        set<term> tmpFirst = resFirst;
                        tmpFirst.erase(EPSILON);
                        follow[t].insert(tmpFirst.begin(), tmpFirst.end());
                        for (auto &f : tmpFirst)
                        {
                            info << "Follow(" << t << ") = {" << f << "} <A -> aBC>" << endl;
                        }
                        if (!_find(resFirst, EPSILON))
                        {
                            break;
                        }
                    }
                    if (resFirst.size() == 0 || _find(resFirst, EPSILON))
                    {
                        // A -> aBC, First(C) contains epsilon
                        if (rule.first != t)
                        {
                            set<term> resFollow = calcFollowOf(rule.first);
                            follow[t].insert(resFollow.begin(), resFollow.end());
                            for (auto &f : resFollow)
                            {
                                info << "Follow(" << t << ") = {" << f << "} <A -> aBC2>" << endl;
                            }
                        }
                    }
                }
                symIt = find(symIt + 1, right.end(), t);
            }
        }
    }
    return follow[t];
}

void EBNFParser::calcFirst()
{
    for (auto &it : non_terms)
    {
        calcFirstOf(it);
    }
}

void EBNFParser::calcFollow()
{
    for (auto &it : non_terms)
    {
        calcFollowOf(it);
    }
}

void EBNFParser::printFirst()
{
    info << "First:" << endl;
    for (auto it = non_terms.begin(); it != non_terms.end(); it++)
    {
        cout << *it << " : ";
        for (auto it2 = first[*it].begin(); it2 != first[*it].end(); it2++)
        {
            cout << *it2 << " ";
        }
        cout << endl;
    }
}

void EBNFParser::printFollow()
{
    info << "Follow:" << endl;
    for (auto it = non_terms.begin(); it != non_terms.end(); it++)
    {
        cout << *it << " : ";
        for (auto it2 = follow[*it].begin(); it2 != follow[*it].end(); it2++)
        {
            cout << *it2 << " ";
        }
        cout << endl;
    }
}

void EBNFParser::printTerminals()
{
    info << "Terminals:" << endl;
    for (auto it = terminals.begin(); it != terminals.end(); it++)
    {
        cout << *it << " ";
    }
    cout << endl;
}
void EBNFParser::printNonTerms()
{
    info << "Non-terminals:" << endl;
    for (auto it = non_terms.begin(); it != non_terms.end(); it++)
    {
        cout << *it << " ";
    }
    cout << endl;
}