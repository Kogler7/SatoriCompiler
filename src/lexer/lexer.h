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

extern map<TokenType, string> tok2str;

extern map<string, TokenType> str2tok;

class Lexer
{
    map<string, unsigned int> rvIdMap;             // 保留字种别码对照表
    map<TokenType, vector<FiniteAutomaton>> faMap; // 状态自动机对照表
    vector<token> tokens;                          // 词法单元序列
    string code;                                   // 代码文本
    void printToken(int idx);

public:
    Lexer() {}
    void addKeywordId(string keyword, unsigned int id);
    void addTokenType(TokenType type, string regExp);
    void readCodeFile(string filename);
    void readLexerDef(string filename);
    void tokenize(string codeSeg = "");
    void printTokens();
    void clear();
};