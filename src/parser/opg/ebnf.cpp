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
#include "../../common/token.h"

typedef unsigned int term;

class EBNFParser
{
    vector<token> tokens;
    set<term> terminals;
    set<term> non_terms;
    map<term, set<vector<term>>> rules;
    map<term, set<term>> first;
    map<term, set<term>> follow;

public:
    EBNFParser(vector<token> ebnf_tokens) : tokens(ebnf_tokens) {}
    void parse();
};

void EBNFParser::parse()
{
    
}