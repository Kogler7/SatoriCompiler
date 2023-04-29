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
#include "../common/token.h"

using namespace std;
typedef unsigned int TokenType;

class Lexer
{
    vector<string> types;
    map<string, TokenType> str2typ;
    set<TokenType> ignoredTypes;
    map<TokenType, vector<FiniteAutomaton>> faMap; // 状态自动机对照表
    vector<token> tokens;                          // 词法单元序列
    string code;                                   // 代码文本
    void printToken(int idx);

public:
    Lexer() {}
    Lexer(string lexPath)
    {
        readLexerDef(lexPath);
    }
    void addTokenType(string typeName, string regExp);
    void addIgnoredType(string typeName);
    void readSrcFile(string filename);
    void readLexerDef(string filename);
    void tokenize(string codeSeg = "");
    vector<token> getTokens() { return tokens; }
    vector<string> getTypes() { return types; }
    void printTokens();
    void clear();
};