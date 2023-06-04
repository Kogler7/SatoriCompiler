/**
 * @file lexer.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Lexical Analysis
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "lexer.h"
#include "nfa.h"
#include "regexp/parser.h"
#include <fstream>
#include <sstream>
#include "utils/stl.h"
#include "utils/log.h"
#include "utils/table.h"
#include "utils/view/ctx_view.h"

#define DEBUG_LEVEL -1

void Lexer::addTokenType(string typeName, string regExp)
{
    token_type_t type;
    if (find_tok_type(typeName))
    {
        type = get_tok_type(typeName);
    }
    else
    {
        type = make_tok_type(typeName);
        set_tok_type(typeName, type);
    }
    RegexpParser regParser(regExp);
    FiniteAutomaton nfa = regParser.parse();
    typeOrder.push_back(type);
    faMap[type].push_back(nfa);
    debug(1) << "Add token type: " << type << " with regExp: " << regExp << endl;
}

void Lexer::addIgnoredType(string typeName)
{
    if (find_tok_type(typeName))
    {
        token_type_t type = get_tok_type(typeName);
        ignoredTypes.insert(type);
        debug(1) << "Add ignored type: " << type << endl;
        return;
    }
    error << "Type " << typeName << " not found!" << endl;
}

string &zTrim(string &s)
{
    while (!s.empty() && s.back() == 0)
    {
        s.pop_back();
    }
    return s;
}

vector<token> Lexer::tokenize(string fileName)
{
    ifstream ifs(fileName);
    assert(ifs.is_open(), "File not found: " + fileName);
    string code(
        (istreambuf_iterator<char>(ifs)),
        (istreambuf_iterator<char>()));
    ifs.close();
    info << "Code: " << endl;
    cout << code << endl;
    info << "Tokenizing... " << endl;
    vector<token> tokens;
    ContextViewer vCode(code);
    while (!vCode.ends())
    {
        bool matched = false;
        string matchedToken;
        token_type_t matchedType;
        Viewer matchedView = vCode;
        for (auto typ : typeOrder) // 按序遍历所有的状态自动机
        {
            const auto &faVec = faMap[typ];
            for (auto nfa : faVec)
            {
                Viewer vTmp = vCode;
                string matchResult;
                if (nfa.accepts(vTmp, matchResult))
                {
                    if (matchResult.length() > matchedToken.length()) // 同类型的自动机取匹配的最长词法单元
                    {
                        matchedToken = matchResult;
                        matchedType = typ;
                        matchedView = vTmp;
                        matched = true;
                    }
                }
            }
            if (matched) // 按照顺序，一旦有某种类型的自动机匹配成功，就不再匹配其他类型的自动机
                break;
        }
        if (matched)
        {
            matchedToken = zTrim(matchedToken); // 补丁：去掉末尾的空字符，产生原因未知
            vCode = matchedView;
            if (!_find(ignoredTypes, matchedType))
            {
                // 忽略空白和注释，其他的都作为词法单元
                int line, col;
                tie(line, col) = vCode.getCurLineCol();
                tokens.push_back(
                    token(matchedType, matchedToken, line, col));
            }
            debug(0) << format("Matched: $ <$>", matchedToken, matchedToken.size()) << endl;
        }
        else
        {
            auto lc = vCode.getCurLineCol();
            error << "Tokenize failed at <" << lc.first << ", " << lc.second << ">" << endl;
            // 打印出错位置的上下文
            vCode.printContext();
            vCode.skipToNextLine();
        }
    }
    return tokens;
}

void Lexer::printTokens(vector<token> tokens)
{
    info << "Tokens: " << endl;
    tb_head | "Type" | "Value";
    set_col | table::AL_RGT | table::AL_LFT;
    for (int i = 0; i < tokens.size(); i++)
    {
        new_row | *tokens[i].type | tokens[i].value;
    }
    cout << tb_view(table::BDR_VRT | table::BDR_TOP | table::BDR_BTM);
}

bool startWith(const string &s, const string &prefix)
{
    return (s.length() >= prefix.length()) && (s.substr(0, prefix.length()) == prefix);
}

vector<string> split(const string &s, char delimiter)
{
    vector<string> tokens;
    if (s.empty())
    {
        return tokens;
    }
    stringstream ss(s);
    string token;
    while (getline(ss, token, delimiter))
    {
        if (!token.empty())
        {
            tokens.push_back(move(token));
        }
    }
    if (s.back() == delimiter)
    {
        tokens.pop_back();
    }
    return move(tokens);
}

void Lexer::readLexerDef(string fileName)
{
    ifstream ifs(fileName);
    assert(ifs.is_open(), "File not found: " + fileName);
    vector<string> tokens;
    // 读取关键字，使用cin循环读入，并存到vector中
    string token;
    while (ifs >> token)
    {
        tokens.push_back(token);
    }
    for (auto tok : tokens)
    {
        debug(1) << tok << endl;
    }
    // 解析PATTERN
    auto patternIt = find(tokens.begin(), tokens.end(), "PATTERN");
    if (patternIt != tokens.end())
    {
        auto ptnStart = find(patternIt, tokens.end(), "${");
        auto ptnEnd = find(patternIt, tokens.end(), "$}");
        for (auto it = ptnStart + 1; it != ptnEnd; it += 2)
        {
            addTokenType(*it, *(it + 1));
        }
    }
    // 解析IGNORE
    auto ignoreIt = find(tokens.begin(), tokens.end(), "IGNORE");
    if (ignoreIt != tokens.end())
    {
        auto ignStart = find(ignoreIt, tokens.end(), "${");
        auto ignEnd = find(ignoreIt, tokens.end(), "$}");
        for (auto it = ignStart + 1; it != ignEnd; it++)
        {
            addIgnoredType(*it);
        }
    }
}