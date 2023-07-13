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
#include "utils/stl.h"
#include "utils/log.h"
#include "utils/table.h"
#include "utils/view/ctx_view.h"
#include "utils/view/wrd_view.h"

#include <sstream>

using namespace std;

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

string &visualize(string &s)
{
    // 将字符串中的不可见字符转换为可见字符
    // 比如将换行符转换为\n，将制表符转换为\t
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] == '\n')
        {
            s.replace(i, 1, "\\n");
        }
        else if (s[i] == '\t')
        {
            s.replace(i, 1, "\\t");
        }
        else if (s[i] == '\r')
        {
            s.replace(i, 1, "\\r");
        }
        else if (s[i] == '\v')
        {
            s.replace(i, 1, "\\v");
        }
        else if (s[i] == '\f')
        {
            s.replace(i, 1, "\\f");
        }
    }
    return s;
}

vector<token> Lexer::tokenize(const Viewer &viewer) const
{
    info << "Tokenizing... " << endl;
    vector<token> tokens;
    ContextViewer vCode(viewer);
    while (!vCode.ends())
    {
        bool matched = false;
        string matchedToken;
        token_type_t matchedType;
        Viewer matchedView = vCode;
        for (auto typ : typeOrder) // 按序遍历所有的状态自动机
        {
            const auto &faVec = faMap.at(typ);
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
            matchedToken = zTrim(matchedToken);     // 补丁：去掉末尾的空字符，产生原因未知
            matchedToken = visualize(matchedToken); // 补丁：可视化字符串
            vCode = matchedView;
            if (!_find(ignoredTypes, matchedType))
            {
                // 忽略空白和注释，其他的都作为词法单元
                size_t line, col;
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

void Lexer::printTokens(const vector<token> &tokens)
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

void Lexer::configLexer(const meta_t &pattern, const meta_t &ignored)
{
    // 解析PATTERN
    assert(pattern.size() > 0, "Lexer: PATTERN is empty!");
    for (auto &content : pattern)
    {
        WordViewer viewer(content);
        while (!viewer.terminate())
        {
            word_loc_t typeLoc = viewer.current();
            string typeName = viewer[typeLoc];
            word_loc_t regLoc = viewer.advance();
            assert(regLoc != word_npos, "Lexer: Regexp not found!");
            string regExp = viewer[regLoc];
            addTokenType(typeName, regExp);
            viewer.advance();
        }
    }
    // 解析IGNORE
    if (ignored != meta_null)
    {
        for (auto &content : ignored)
        {
            WordViewer viewer(content);
            while (!viewer.terminate())
            {
                word_loc_t typeLoc = viewer.current();
                string typeName = viewer[typeLoc];
                addIgnoredType(typeName);
                viewer.advance();
            }
        }
    }
    else
    {
        warn << "Lexer: IGNORED not found!" << endl;
    }
}