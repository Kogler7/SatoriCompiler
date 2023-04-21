#pragma once
#include <map>
#include <string>
#include <vector>
#include "nfa.h"

extern enum TokenType {
    BLANK,
    IGNORE,
    MACRO,
    INCLUDE,
    IDENTIFIER,
    STRING,
    CHARACTER,
    REAL,
    INTEGER,
    SEPARATOR,
};

extern map<TokenType, string> tok2str;

extern map<string, TokenType> str2tok;

class Lexer
{
    map<string, unsigned int> rvIdMap;             // 保留字种别码对照表
    map<TokenType, vector<FiniteAutomaton>> faMap; // 状态自动机对照表
    vector<pair<TokenType, string>> tokens;        // 词法单元序列
    vector<string> literals;                       // 字符串字面量序列
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