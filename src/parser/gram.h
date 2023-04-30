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
    map<term, set<vector<term>>> rules;
    map<term, set<term>> first;
    map<term, set<term>> follow;

    Grammar(term start, set<term> terms, set<term> non_terms, map<term, set<vector<term>>> rules);
    Grammar() {}
    void operator=(const Grammar &g);
    void printRules();
    void printTerminals();
    void printNonTerms();
    set<term> calcFirstOf(term t);
    set<term> calcFollowOf(term t);
    void calcFirst();
    void calcFollow();
    void printFirst();
    void printFollow();
};