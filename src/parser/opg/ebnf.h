/**
 * @file ebnf.h
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

#include "../../utils/log.h"
#include "../../common/token.h"

typedef string term;

class EBNFParser
{
    vector<token> tokens;
    vector<string> tokTypes;
    term startTerm;
    int termType;
    int startType;
    int nonTermType;
    int epsilonType;
    set<term> terminals;
    set<term> non_terms;
    map<term, set<vector<term>>> rules;
    map<term, set<term>> first;
    map<term, set<term>> follow;

public:
    EBNFParser(vector<token> ebnf_tokens, vector<string> tok_types);
    term getStart() { return startTerm; }
    void parseRules();
    void printRules();
    set<term> calcFirstOf(term t);
    set<term> calcFollowOf(term t);
    void calcFirst();
    void calcFollow();
    void printFirst();
    void printFollow();
    void printTerminals();
    void printNonTerms();
};