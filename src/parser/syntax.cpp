/**
 * @file syntax.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Syntax Parser for EBNF
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <map>
#include <set>
#include <vector>
#include <string>

#include "common/token.h"
#include "syntax.h"
#include "utils/log.h"
#include "utils/table.h"
#include "utils/stl.h"

#define DEBUG_LEVEL 0

using namespace std;

token_const_iter_t findType(const vector<token> &tokens, token_type_t type, token_const_iter_t start)
{
    for (token_const_iter_t it = start; it != tokens.end(); it++)
    {
        if (it->type == type)
        {
            return it;
        }
    }
    return tokens.end();
}

token_const_iter_t findDeli(const vector<token> &tokens, string deli, token_const_iter_t start)
{
    token_type_t sepType = get_tok_type("DELIMITER");
    for (token_const_iter_t it = start; it != tokens.end(); it++)
    {
        if (it->type == sepType && it->value == deli)
        {
            return it;
        }
    }
    return tokens.end();
}

string lrtri(string str)
{
    return str.substr(1, str.length() - 2);
}

SyntaxParser::SyntaxParser(const string ebnfLexPath)
{
    MetaParser lexMeta = MetaParser::fromFile(ebnfLexPath);
    ebnfLexer = Lexer(lexMeta["EBNF"], lexMeta["IGNORED"]);
    mappingLexer = Lexer(lexMeta["MAPPING"], lexMeta["IGNORED"]);
    precLexer = Lexer(lexMeta["PREC"], lexMeta["IGNORED"]);
}

inline symbol_t getEndDeli(const symbol_t &deli)
{
    static const string startDelim = "([{", endDelim = ")]}";
    static size_t pos;
    assert((pos = startDelim.find(deli)) != string::npos, "getEndDeli: Expected (, [, or { .");
    return string(1, endDelim[pos]);
}

void fullConnProducts(vector<tok_product_t> &dst, const vector<tok_product_t> &src)
{
    vector<tok_product_t> res;
    for (const auto &srcPro : src)
    {
        for (const auto &dstPro : dst)
        {
            vector<token> newRight = dstPro.second;
            newRight.insert(newRight.end(), srcPro.second.begin(), srcPro.second.end());
            res.push_back(make_pair(dstPro.first, newRight));
        }
    }
    dst = res;
}

void SyntaxParser::parseDeliProducts(vector<tok_product_t> &tmp, const symbol_t &left, token_const_iter_t beginIt, token_const_iter_t endIt)
{
    vector<tok_product_t> subProducts;
    tok_product_t tokProduct = make_pair(left, vector<token>(beginIt + 1, endIt));
    subProducts = segmentProduct(tokProduct);
    if (beginIt->value == "(")
    {
        // S -> A(B|D)C => S -> ABC, S -> ADC
        fullConnProducts(tmp, subProducts);
    }
    else if (beginIt->value == "{")
    {
        // S -> A{B|D}C => S' -> B|D, S'' -> S'S'' | ε, S -> AS''C
        // 构造代表花括号的新非终结符
        symbol_t starTerm = left + "_star_";
        starTerm += to_string(++nonTermCount[left]);
        // 构造含新非终结符的新产生式
        token innerLeftTok = token(get_tok_type("NON_TERM"), starTerm);
        fullConnProducts(tmp, vector<tok_product_t>({make_pair(left, vector<token>({innerLeftTok}))}));
        for (auto &pro : subProducts)
        {
            vector<token> subRight = pro.second;
            subRight.push_back(innerLeftTok);
            tokProducts.push_back(make_pair(starTerm, subRight));
        }
        // 构造含空串的新产生式
        tokProducts.push_back(make_pair(starTerm, vector<token>()));
    }
    else if (beginIt->value == "[")
    {
        // S -> A[B|D]C => S -> ABC, S -> ADC, S -> AC
        symbol_t optiTerm = left + "_opti_";
        optiTerm += to_string(++nonTermCount[left]);
        // 构造含新非终结符的新产生式
        token optiLeftTok = token(get_tok_type("NON_TERM"), optiTerm);
        fullConnProducts(tmp, vector<tok_product_t>({make_pair(left, vector<token>({optiLeftTok}))}));
        for (auto &pro : subProducts)
        {
            tokProducts.push_back(make_pair(optiTerm, pro.second));
        }
        // 构造含空串的新产生式
        tokProducts.push_back(make_pair(optiTerm, vector<token>()));
    }
    else
    {
        error << "SyntaxParser: EBNF syntax error: Expected (, [, or { ." << endl;
        exit(1);
    }
}

vector<tok_product_t> SyntaxParser::segmentProduct(tok_product_t &product)
{
    info << "SyntaxParser: Segmenting product: " << product.first << " -> ";
    for (auto it = product.second.begin(); it != product.second.end(); it++)
    {
        cout << it->value << " ";
    }
    cout << endl;
    vector<tok_product_t> products;
    symbol_t &left = product.first;
    vector<token> &right = product.second;
    auto lastIt = right.cbegin();
    auto nextIt = right.cbegin();
    auto deliBeginIt = findType(right, get_tok_type("DELIMITER"), right.begin());
    if (deliBeginIt == right.end())
    {
        products.push_back(product);
        info << "SyntaxParser: No delimiter found, return directly." << endl;
        return products;
    }
    while (lastIt != right.end())
    {
        if (deliBeginIt == right.end()) // 无分隔符
        {
            products.push_back(make_pair(left, vector<token>(lastIt, right.cend())));
            break;
        }
        else if (deliBeginIt->value == "|")
        {
            // 如果是选择符，则把左边的产生式直接加入到结果中
            products.push_back(make_pair(left, vector<token>(lastIt, deliBeginIt)));
            nextIt = deliBeginIt;
        }
        else
        {
            // 如果是分组符，则需要递归拆分
            vector<tok_product_t> resProducts({make_pair(left, vector<token>())});
            vector<tok_product_t> leadProducts;

            while (deliBeginIt != right.end() && deliBeginIt->value != "|")
            {
                leadProducts = vector<tok_product_t>({make_pair(left, vector<token>(lastIt, deliBeginIt))});
                fullConnProducts(resProducts, leadProducts);

                symbol_t endDeli = getEndDeli(deliBeginIt->value);
                auto deliEndIt = findDeli(right, endDeli, deliBeginIt);
                assert(
                    deliEndIt != right.end(),
                    format(
                        "SyntaxParser: EBNF syntax error: Expected $, got EOF at <$, $>",
                        endDeli, right.back().line, right.back().col));

                parseDeliProducts(resProducts, left, deliBeginIt, deliEndIt);
                lastIt = deliEndIt + 1;
                deliBeginIt = findType(right, get_tok_type("DELIMITER"), lastIt);
            }

            leadProducts = vector<tok_product_t>({make_pair(left, vector<token>(lastIt, deliBeginIt))});
            fullConnProducts(resProducts, leadProducts);

            nextIt = deliBeginIt;
            products.insert(products.end(), resProducts.begin(), resProducts.end());
        }
        if (nextIt == right.end())
        {
            break;
        }
        lastIt = nextIt + 1;
        info << "SyntaxParser: lastIt: " << lastIt->value << endl;
        if (nextIt != right.end())
        {
            deliBeginIt = findType(right, get_tok_type("DELIMITER"), lastIt);
        }
    }
    info << "SyntaxParser: Segmented product: " << endl;
    for (auto &pro : products)
    {
        cout << pro.first << " -> ";
        for (auto &tok : pro.second)
        {
            cout << tok.value << " ";
        }
        cout << endl;
    }
    return products;
}

void SyntaxParser::addSyntaxRules(const vector<token> &tokens)
{
    vector<tok_product_t> &products = tokProducts;
    token_type_t grammarDef = get_tok_type("GRAMMAR_DEF");
    token_type_t sepType = get_tok_type("SEPARATOR");
    token_type_t termType = get_tok_type("TERMINAL");
    token_type_t nonTermType = get_tok_type("NON_TERM");
    token_type_t mulTermType = get_tok_type("MUL_TERM");
    token_type_t epsilonType = get_tok_type("EPSILON");
    token_type_t semanticType = get_tok_type("SEMANTIC");
    // 预处理，拆分 ; 表示的多个产生式
    vector<tok_product_t> rawProducts;
    for (auto it = tokens.cbegin(); it != tokens.cend(); it++)
    {
        assert(
            it->type == nonTermType,
            format(
                "SyntaxParser: EBNF syntax error: Expected non-terminal, got $ at <$, $>.",
                it->value, it->line, it->col));
        symbol_t left = it->value;
        it++;
        assert(
            it != tokens.cend() && it->type == grammarDef,
            format(
                "SyntaxParser: EBNF syntax error: Expected ::=, got $ at <$, $>.",
                it->value, it->line, it->col));
        it++;
        assert(it != tokens.cend());
        token_const_iter_t sepIt = findType(tokens, sepType, it);
        vector<token> right;
        for (auto it2 = it; it2 != sepIt; it2++)
        {
            right.push_back(*it2);
        }
        rawProducts.push_back(make_pair(left, right));
        it = sepIt;
    }
    // 进一步拆分
    for (auto &pro : rawProducts)
    {
        // 递归下降法解析拆分EBNF
        vector<tok_product_t> subProducts = segmentProduct(pro);
        products.insert(products.end(), subProducts.begin(), subProducts.end());
    }
    // 处理产生式中的 ε
    for (auto &pro : products)
    {
        if (pro.second.size() == 1 && pro.second[0].type == epsilonType)
        {
            pro.second.clear();
        }
    }
    using namespace table;
    info << "SyntaxParser::geneMapProducts: " << products.size() << " productions generated." << endl;
    tb_head | TB_TAB | MD_TAB | "Generated Productions";
    set_col | AL_RGT | AL_LFT | AL_LFT;
    for (auto &pro : products)
    {
        stringstream ss;
        for (auto &tok : pro.second)
        {
            ss << tok.value << " ";
        }
        new_row | pro.first | "->" | ss.str();
    }
    std::cout << tb_view(BDR_RUD);
    // 将终结符、映射终结符、非终结符、产生式、规则和语义动作加入文法中
    symset_t &mulTerms = grammar.mulTerms;
    symset_t &nonTerms = grammar.nonTerms;
    symset_t &terminals = grammar.terminals;
    vector<product_t> &gPros = grammar.products;
    map<symbol_t, set<symstr_t>> &rules = grammar.rules;
    const vector<semantic_t> &semSeq = syntaxMeta.getMeta("SEMANTIC", vector<semantic_t>());
    info << "semSeq size: " << semSeq.size() << endl;
    map<size_t, semantic_t> semLocMap;
    map<size_t, product_t> proLocMap;
    size_t semIdx = 0;
    for (auto &pro : products)
    {
        nonTerms.insert(pro.first);
        symstr_t right;
        for (auto &tok : pro.second)
        {
            if (tok.type == semanticType)
            {
                // 记录下语义动作及其对应的行数，便于后续挂载到对应的产生式中
                assert(semIdx < semSeq.size(), "SyntaxParser: Semantic action index out of range.");
                if (semLocMap.find(tok.line) == semLocMap.end())
                {
                    // 语义动作只匹配第一次出现的行数
                    // 因为前面的处理会导致同一行的语义动作出现多次
                    // 如果不加以考虑，这里会导致下标越界
                    semLocMap[tok.line] = semSeq[semIdx++];
                }
                // 语义动作不加入到最后的产生式中
                continue;
            }
            if (tok.type == nonTermType)
            {
                nonTerms.insert(tok.value);
            }
            else if (tok.type == mulTermType)
            {
                tok.value = tok.value.substr(1);
                terminals.insert(tok.value);
                mulTerms.insert(tok.value);
            }
            else if (tok.type == termType)
            {
                tok.value = lrtri(tok.value);
                terminals.insert(tok.value);
            }
            else
            {
                error << format(
                    "SyntaxParser: EBNF token error: Expected terminal, non-terminal, or semantic action, got $ ($) at <$, $>.\n",
                    tok.value, *tok.type, tok.line, tok.col);
            }
            right.push_back(tok.value);
        }
        // 获取产生式右部所在的行数
        size_t proLine = right.size() ? pro.second.back().line : 0;
        rules[pro.first].insert(right); // 添加规则
        product_t newPro = make_pair(pro.first, right);
        gPros.push_back(newPro); // 添加产生式
        proLocMap[proLine] = newPro;
    }
    // 重新遍历产生式，挂载语义动作
    map<product_t, semantic_t> &semMap = grammar.semMap;
    for (auto &pair : proLocMap)
    {
        size_t proLine = pair.first;
        product_t &pro = pair.second;
        if (semLocMap.find(proLine) != semLocMap.end())
        {
            semMap[pro] = semLocMap[proLine];
        }
    }
}

void SyntaxParser::addTokenMappings(const vector<token> &tokens)
{
    vector<tok_product_t> products;
    token_type_t mappingDef = get_tok_type("MAPPING_DEF");
    token_type_t sepType = get_tok_type("SEPARATOR");
    token_type_t mulTermType = get_tok_type("MUL_TERM");
    token_type_t epsilonType = get_tok_type("EPSILON");
    token_type_t tokType = get_tok_type("TOK_TYPE");
    // 预处理，拆分 ; 表示的多个产生式
    vector<tok_product_t> rawProducts;
    for (auto it = tokens.cbegin(); it != tokens.cend(); it++)
    {
        assert(
            it->type == mulTermType,
            format(
                "SyntaxParser: MAPPING syntax error: Expected mul-terminal, got $ at <$, $>.",
                it->value, it->line, it->col));
        symbol_t left = it->value;
        it++;
        assert(
            it->type == mappingDef,
            format(
                "SyntaxParser: MAPPING syntax error: Expected ->, got $ at <$, $>.",
                it->value, it->line, it->col));
        it++;
        token_const_iter_t sepIt = findType(tokens, sepType, it);
        vector<token> right;
        for (auto it2 = it; it2 != sepIt; it2++)
        {
            right.push_back(*it2);
        }
        rawProducts.push_back(make_pair(left, right));
        it = sepIt;
    }
    // 进一步拆分
    for (auto &pro : rawProducts)
    {
        vector<tok_product_t> subProducts = segmentProduct(pro);
        products.insert(products.end(), subProducts.begin(), subProducts.end());
    }
    // 将映射加入文法中
    map<token_type_t, symbol_t> &tok2sym = grammar.tok2sym;
    for (auto &pro : products)
    {
        symbol_t left = pro.first.substr(1);
        vector<token> &right = pro.second;
        assert(
            right.size() == 1,
            format(
                "SyntaxParser: Mapping syntax error: Expected only one token-type term, got $ at <$, $>.",
                right.size(), right[0].line, right[0].col));
        assert(
            right[0].type == tokType,
            format(
                "SyntaxParser: Mapping syntax error: Expected token type, got $ at <$, $>.",
                right[0].value, right[0].line, right[0].col));
        symbol_t tokTerm = right[0].value;
        tokTerm = tokTerm.substr(1);
        grammar.terminals.insert(left);
        if (!find_tok_type(tokTerm))
        {
            warn << "SyntaxParser: Mapping syntax warn: Undefined token type " << tokTerm << endl;
            set_tok_type(tokTerm, make_tok_type(tokTerm));
        }
        tok2sym[get_tok_type(tokTerm)] = left;
    }
}

void SyntaxParser::addPrecAndAssoc()
{
    info << "SyntaxParser: Adding precedence and associativity ..." << endl;
    map<symbol_t, prec_assoc_t> &precMap = grammar.precMap;
    if (syntaxMeta.hasMeta("%LEFT"))
    {
        const vector<meta_content_t> &leftSeq = syntaxMeta["%LEFT"];
        for (size_t i = 0; i < leftSeq.size(); i++)
        {
            vector<token> tokens = precLexer.tokenize(leftSeq[i]);
            for (auto &tok : tokens)
            {
                precMap[lrtri(tok.value)] = make_pair(i, ASSOC_LEFT);
            }
        }
    }
    if (syntaxMeta.hasMeta("%RIGHT"))
    {
        const vector<meta_content_t> &rightSeq = syntaxMeta["%RIGHT"];
        for (size_t i = 0; i < rightSeq.size(); i++)
        {
            vector<token> tokens = precLexer.tokenize(rightSeq[i]);
            for (auto &tok : tokens)
            {
                precMap[lrtri(tok.value)] = make_pair(i, ASSOC_RIGHT);
            }
        }
    }
    if (syntaxMeta.hasMeta("%NONASSOC"))
    {
        const vector<meta_content_t> &nonSeq = syntaxMeta["%NONASSOC"];
        for (size_t i = 0; i < nonSeq.size(); i++)
        {
            vector<token> tokens = precLexer.tokenize(nonSeq[i]);
            for (auto &tok : tokens)
            {
                precMap[lrtri(tok.value)] = make_pair(i, ASSOC_NONE);
            }
        }
    }
    // 打印优先级和结合性
    vector<string> precStrs = {"LEFT", "RIGHT", "NONASSOC"};
    for (auto &pair : precMap)
    {
        std::cout << format(
            "SyntaxParser: Prec and assoc of $ is $, $\n",
            pair.first, pair.second.first, precStrs[pair.second.second]);
    }
}

Grammar SyntaxParser::parse(const string grammarPath)
{
    vector<token> tokens;
    syntaxMeta = MetaParser::fromFile(grammarPath);

    // 解析EBNF定义的文法
    tokens = ebnfLexer.tokenize(*syntaxMeta["GRAMMAR"].begin());
    info << "SyntaxParser: EBNF tokens: " << endl;
    ebnfLexer.printTokens(tokens);

    // 解析开始符号
    auto startIt = findType(tokens, get_tok_type("START_MRK"), tokens.begin());
    assert(startIt != tokens.end(), "SyntaxParser: No start symbol defined.");
    grammar.symStart = (startIt - 1)->value;
    info << "SyntaxParser: Start symbol is " << grammar.symStart << endl;

    tokens.erase(startIt);
    assert(
        findType(tokens, get_tok_type("START_MRK"), tokens.begin()) == tokens.end(),
        "SyntaxParser: Multiple start symbols defined.");

    // 解析并添加产生式
    addSyntaxRules(tokens);
    // 解析MAPPING映射
    tokens = mappingLexer.tokenize(*syntaxMeta["MAPPING"].begin());
    info << "SyntaxParser: MAPPING tokens: " << endl;
    mappingLexer.printTokens(tokens);

    // 解析并添加映射
    addTokenMappings(tokens);

    // 解析并添加优先级和结合性
    addPrecAndAssoc();

    return grammar;
}
