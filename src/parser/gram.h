/**
 * @file gram.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Grammar
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <map>
#include <set>
#include <vector>
#include <string>

#include "common/token.h"

#define EPSILON "@" // 用于表示空串
#define SYM_END "#" // 用于表示输入串结束

using namespace std;

typedef string term;

class Grammar
{
public:
    term startTerm;
    set<term> terminals;
    set<term> nonTerms;
    map<token_type, term> tok2term;
    map<term, set<vector<term>>> rules;
    map<term, set<term>> first;
    map<term, set<term>> follow;
    map<term, map<vector<term>, set<term>>> select;
    map<term, map<term, vector<term>>> predict;

    Grammar(term start, set<term> terms, set<term> non_terms, map<term, set<vector<term>>> rules, map<token_type, term> tok2term);
    Grammar() {}
    void operator=(const Grammar &g);
    void eliminateLeftRecursion();
    void extractLeftCommonFactor();
    bool isLL1Grammar();
    set<term> calcFirstOf(term t);
    set<term> calcFollowOf(term t);
    set<term> calcSelectOf(term t, vector<term> rule);
    void calcFirst();
    void calcFollow();
    void calcSelect();
    void printRules();
    void printTerminals();
    void printNonTerms();
    void printFirst();
    void printFollow();
    void printSelect();
};