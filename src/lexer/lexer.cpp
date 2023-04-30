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
#include "utils/log.h"

#define _find(x, y) (x.find(y) != x.end())

void Lexer::addTokenType(string typeName, string regExp)
{
    token_type type;
    if (_find(types, typeName))
    {
        type = types[typeName];
    }
    else
    {
        type = make_type(typeName);
        types[typeName] = type;
    }
    RegexpParser regParser(regExp);
    FiniteAutomaton nfa = regParser.parse();
    faMap[type].push_back(nfa);
    debug(1) << "Add token type: " << type << " with regExp: " << regExp << endl;
}

void Lexer::addIgnoredType(string typeName)
{
    if (_find(types, typeName))
    {
        token_type type = types[typeName];
        ignoredTypes.insert(type);
        debug(1) << "Add ignored type: " << type << endl;
        return;
    }
    error << "Type " << typeName << " not found!" << endl;
}

pair<vector<token>, map<string, token_type>> Lexer::tokenize(string codeSeg)
{
    if (codeSeg != "")
        this->code = codeSeg;
    info << "Tokenizing... " << endl;
    code_viewer vCode(code);
    while (!vCode.ends())
    {
        bool matched = false;
        string matchedToken;
        token_type matchedType;
        viewer matchedView = vCode;
        for (auto tokFa : faMap) // 按序遍历所有的状态自动机
        {
            for (auto nfa : tokFa.second)
            {
                viewer vTmp = vCode;
                string matchResult;
                if (nfa.accepts(vTmp, matchResult))
                {
                    if (matchResult.length() > matchedToken.length()) // 同类型的自动机取匹配的最长词法单元
                    {
                        matchedToken = matchResult;
                        matchedType = tokFa.first;
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
            vCode = matchedView;
            if (!_find(ignoredTypes, matchedType))
            {
                // 忽略空白和注释，其他的都作为词法单元
                int line, col;
                tie(line, col) = vCode.getCurLineCol();
                tokens.push_back(
                    token(matchedType, matchedToken, line, col));
            }
            debug(0) << "Tokenize accepted: " << matchedToken << endl;
        }
        else
        {
            auto lc = vCode.getCurLineCol();
            error << "Tokenize failed at <" << lc.first << ", " << lc.second << ">" << endl;
            // 打印出错位置的上下文
            vCode.printCodeCtx();
            vCode.skipToNextLine();
        }
    }
    return make_pair(tokens, types);
}

void Lexer::printToken(int idx)
{
    token tok = tokens[idx];
    cout << "(" << setw(10) << right << tok.type << ", ";
    cout << setw(12) << left << tok.value << ")";
    cout << endl;
}

void Lexer::printTokens()
{
    info << "Tokens: " << endl;
    for (int i = 0; i < tokens.size(); i++)
    {
        printToken(i);
    }
}

void Lexer::clear()
{
    tokens.clear();
}

void Lexer::readSrcFile(string fileName)
{
    ifstream ifs(fileName);
    string _code((istreambuf_iterator<char>(ifs)),
                 (istreambuf_iterator<char>()));
    code = _code;
    ifs.close();
    info << "Code: " << endl;
    cout << code << endl;
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