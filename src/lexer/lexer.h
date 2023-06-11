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

#include "nfa.h"
#include "common/token.h"
#include "utils/view/viewer.h"
#include "utils/meta.h"

#include <map>
#include <string>
#include <vector>

class Lexer
{
    std::set<token_type_t, type_less> ignoredTypes;
    std::vector<token_type_t> typeOrder;                                   // 词法单元类型顺序
    std::map<token_type_t, std::vector<FiniteAutomaton>, type_less> faMap; // 状态自动机对照表

public:
    Lexer() {}
    Lexer(const meta_t &pattern, const meta_t &ignored = meta_null)
    {
        configLexer(pattern, ignored);
    }
    Lexer(const std::string &metaFile)
    {
        MetaParser parser = MetaParser::fromFile(metaFile);
        configLexer(parser["PATTERN"], parser["IGNORED"]);
    }
    void configLexer(const meta_t &pattern, const meta_t &ignored = meta_null);
    void addTokenType(std::string typeName, std::string regExp);
    void addIgnoredType(std::string typeName);
    std::vector<token> tokenize(const Viewer &viewer) const;
    std::vector<token> tokenizeFile(const std::string &fileName) const
    {
        return tokenize(Viewer::fromFile(fileName));
    }
    static void printTokens(const std::vector<token> &tokens);
};