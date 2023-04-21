#include "lexer.h"
#include "nfa.h"
#include "regexp.h"
#include <fstream>
#include <sstream>
#include "../utils/log.h"

map<TokenType, string> tok2str = {
    {BLANK, "BLANK"},
    {IGNORE, "IGNORE"},
    {IDENTIFIER, "IDENTIFIER"},
    {SEPARATOR, "SEPARATOR"},
    {STRING, "STRING"},
    {CHARACTER, "CHARACTER"},
    {INTEGER, "INTEGER"},
    {REAL, "REAL"},
    {MACRO, "MACRO"},
    {INCLUDE, "INCLUDE"}};

map<string, TokenType> str2tok = {
    {"BLANK", BLANK},
    {"IGNORE", IGNORE},
    {"IDENTIFIER", IDENTIFIER},
    {"SEPARATOR", SEPARATOR},
    {"STRING", STRING},
    {"CHARACTER", CHARACTER},
    {"INTEGER", INTEGER},
    {"REAL", REAL},
    {"MACRO", MACRO},
    {"INCLUDE", INCLUDE}};

void Lexer::addKeywordId(string keyword, unsigned int id)
{
    rvIdMap[keyword] = id;
    debug(1) << "Add keyword: " << keyword << " with id: " << id << endl;
}

void Lexer::addTokenType(TokenType type, string regExp)
{
    Regexp2FA reg2FA(regExp);
    FiniteAutomaton nfa = reg2FA.convert();
    faMap[type].push_back(nfa);
    debug(1) << "Add token type: " << tok2str[type] << " with regExp: " << regExp << endl;
}

void Lexer::tokenize(string codeSeg)
{
    if (codeSeg != "")
        this->code = codeSeg;
    info << "Tokenizing... " << endl;
    code_viewer vCode(code);
    while (!vCode.ends())
    {
        bool matched = false;
        string matchedToken;
        TokenType matchedType;
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
            if (matchedType == IDENTIFIER || matchedType == SEPARATOR)
            {
                // 默认只认为标识符和分隔符是保留字，拥有种别码
                if (rvIdMap.find(matchedToken) != rvIdMap.end())
                {
                    // 如果是保留字，就把它的种别码作为词法单元的种别码
                    tokens.push_back(make_pair(matchedType, to_string(rvIdMap[matchedToken])));
                }
                else
                {
                    // 否则就把它的字符串作为词法单元的种别码
                    tokens.push_back(make_pair(matchedType, matchedToken));
                }
                literals.push_back(matchedToken); // 加入字面量序列，以变查看
            }
            else if (matchedType != IGNORE && matchedType != BLANK)
            {
                // 忽略空白和注释，其他的都作为词法单元
                tokens.push_back(make_pair(matchedType, matchedToken));
                literals.push_back(matchedToken); // 加入字面量序列，以变查看
            }
            vCode = matchedView;
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
}

void Lexer::printToken(int idx)
{
    pair<TokenType, string> token = tokens[idx];
    cout << "(" << setw(10) << right << tok2str[token.first] << ", ";
    cout << setw(16) << left << token.second << ")";
    cout << " : " << literals[idx];
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

void Lexer::readCodeFile(string fileName)
{
    ifstream ifs(fileName);
    string _code((istreambuf_iterator<char>(ifs)),
                 (istreambuf_iterator<char>()));
    code = _code;
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
            if (str2tok.find(*it) != str2tok.end())
            {
                addTokenType(str2tok[*it], *(it + 1));
            }
            else
            {
                error << "Unknown token type: " << *it << endl;
                throw runtime_error("Unknown token type");
            }
        }
    }
    // 解析RESERVED
    auto reservedIt = find(tokens.begin(), tokens.end(), "RESERVED");
    if (reservedIt != tokens.end())
    {
        auto rvStart = find(reservedIt, tokens.end(), "${");
        auto rvEnd = find(reservedIt, tokens.end(), "$}");
        int id = 0;
        for (auto it = rvStart + 1; it != rvEnd; it++)
        {
            addKeywordId(*it, id++);
        }
    }
}