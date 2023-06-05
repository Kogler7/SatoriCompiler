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
#include "utils/view/viewer.h"
#include "utils/meta.h"

using namespace std;

class Lexer
{
    set<token_type_t, type_less> ignoredTypes;
    vector<token_type_t> typeOrder;                              // 词法单元类型顺序
    map<token_type_t, vector<FiniteAutomaton>, type_less> faMap; // 状态自动机对照表

public:
    Lexer() {}
    Lexer(const meta_t &pattern, const meta_t &ignored = meta_null)
    {
        configLexer(pattern, ignored);
    }
    Lexer(const string &metaFile)
    {
        MetaParser parser = MetaParser::fromFile(metaFile);
        configLexer(parser["PATTERN"], parser["IGNORED"]);
    }
    void configLexer(const meta_t &pattern, const meta_t &ignored = meta_null);
    void addTokenType(string typeName, string regExp);
    void addIgnoredType(string typeName);
    vector<token> tokenize(const Viewer &viewer);
    vector<token> tokenizeFile(const string &fileName)
    {
        return tokenize(Viewer::fromFile(fileName));
    }
    static void printTokens(const vector<token> &tokens);
};