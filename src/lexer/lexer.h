/**
 * @file lexer.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Lexical Analysis
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <map>
#include <string>
#include <vector>
#include "nfa.h"
#include "common/token.h"

using namespace std;

class Lexer
{
    set<token_type, type_less> ignoredTypes;
    vector<token_type> typeOrder;                              // 词法单元类型顺序
    map<token_type, vector<FiniteAutomaton>, type_less> faMap; // 状态自动机对照表
    void readLexerDef(string fileName);

public:
    Lexer() {}
    Lexer(string lexPath)
    {
        readLexerDef(lexPath);
    }
    void addTokenType(string typeName, string regExp);
    void addIgnoredType(string typeName);
    vector<token> tokenize(string fileName);
    void printTokens(vector<token> tokens);
    void clear();
};