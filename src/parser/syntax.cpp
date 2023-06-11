/**
 * @file syntax.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Syntax Parser for EBNF
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

/**
 * 本文件实现了文法定义解析器
 * 该解析器可以解析EBNF定义的文法，包括多种“运算符”，如分组、选择、重复、可选等
 * 解析器会将文法定义文件递归下降地解析为一系列产生式，然后将其加入到文法中
 * 解析器还会解析MAPPING定义，将其中的映射关系加入到文法中
 * 解析器还会解析PREC定义，将其中的优先级和结合性加入到文法中
 * 解析器还会解析SEMANTIC定义，将其中的语义动作加入到文法中
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

ostream &operator<<(ostream &os, const tok_product_t &product)
{
    os << product.first << " -> ";
    for (auto &tok : product.second)
    {
        os << tok.value << " ";
    }
    return os;
}

stringstream &operator<<(stringstream &ss, const tok_product_t &product)
{
    ss << product.first << " -> ";
    for (auto &tok : product.second)
    {
        ss << tok.value << " ";
    }
    return ss;
}

/**
 * @brief 从tokens中查找第一个类型为type的token
 *
 * @param tokens 输入的token序列
 * @param type 要查找的token类型
 * @param start 查找的起始位置
 * @return token_const_iter_t 查找到的token的迭代器
 */
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

/**
 * @brief 从tokens中查找第一个值为deli的分隔符（运算符）
 *
 * @param tokens 输入的token序列
 * @param deli 要查找的分隔符（运算符）
 * @param start 查找的起始位置
 * @return token_const_iter_t 查找到的token的迭代器
 */
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

/**
 * @brief 删去给定字符串的首尾字符，如 `abc` => abc
 *
 * @param str 输入的字符串
 * @return string 删去首尾字符后的字符串
 */
inline string lrTrim(string str)
{
    return str.substr(1, str.length() - 2);
}

/**
 * @brief Syntax Parser的构造函数，主要完成用到的词法分析器的初始化
 *
 * @param ebnfLexPath EBNF词法分析器的元数据文件路径
 */
SyntaxParser::SyntaxParser(const string ebnfLexPath)
{
    MetaParser lexMeta = MetaParser::fromFile(ebnfLexPath);
    ebnfLexer = Lexer(lexMeta["EBNF"], lexMeta["IGNORED"]);
    mappingLexer = Lexer(lexMeta["MAPPING"], lexMeta["IGNORED"]);
    precLexer = Lexer(lexMeta["PREC"], lexMeta["IGNORED"]);
}

/**
 * @brief 获取给定分隔符（运算符）的结束分隔符（运算符），如 ( => )， { => }， [ => ]
 *
 * @param deli 给定的分隔符（运算符）
 * @return symbol_t 对应的结束分隔符（运算符）
 */
inline symbol_t getEndDeli(const symbol_t &deli)
{
    static const string startDelim = "([{", endDelim = ")]}";
    static size_t pos;
    assert((pos = startDelim.find(deli)) != string::npos, "getEndDeli: Expected (, [, or { .");
    return string(1, endDelim[pos]);
}

/**
 * @brief 将两个产生式列表中的每个产生式的右部进行全连接，用于实现运算符解析中间结果的拼接
 *
 * @param dst 待拼接的目的产生式列表
 * @param src 待拼接的源产生式列表
 */
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

/**
 * @brief 解析非平凡的产生式，即包含分组、选择、重复、可选等运算符的产生式
 *
 * @param tmp 解析的中间结果
 * @param left 产生式左部
 * @param beginIt 解析输入流的起始位置
 * @param endIt 解析输入流的结束位置
 */
void SyntaxParser::parseNonTrivialProducts(vector<tok_product_t> &tmp, const symbol_t &left, token_const_iter_t beginIt, token_const_iter_t endIt)
{
    // 当解析输入流遇到 (, [, { 时，需要调用此方法完成进一步的解析
    vector<tok_product_t> subProducts;
    tok_product_t tokProduct = make_pair(left, vector<token>(beginIt + 1, endIt));
    // 这里先将尝试 ()、[]、{} 中的内容拆分为多个子产生式
    // 这里的本质是递归下降法，因为 ()、[]、{} 中的内容可能包含 |、()、[]、{}，需要进一步拆分
    subProducts = segmentProduct(tokProduct);
    if (beginIt->value == "(")
    {
        // 解析 () 中的内容，即解析分组操作
        // S -> A(B|D)C => S -> ABC, S -> ADC
        fullConnProducts(tmp, subProducts);
    }
    else if (beginIt->value == "{")
    {
        // 解析 {} 中的内容，即解析重复操作
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
        // 解析 [] 中的内容，即解析可选操作
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

/**
 * @brief 将给定的产生式拆分为多个子产生式，主要处理选择符 | ，并递归下降处理 ()、[]、{} 中的内容
 *
 * @param product 给定的产生式
 * @return vector<tok_product_t> 拆分后的子产生式列表
 */
vector<tok_product_t> SyntaxParser::segmentProduct(tok_product_t &product)
{
    // 本函数区分两个概念，Separator 和 Delimiter
    // Separator 是指用于分隔产生式的符号，如 ;
    // Delimiter 是指用于分隔产生式中的子产生式的符号，如 |、 () 、 [] 、 {} 等
    // 一般Delimiter意味着某种运算符，而Separator意味着独立的产生式
    // 为了方便区分，可以称Delimiter为运算符，称Separator为分隔符
    info << format("SyntaxParser: Segmenting product: $.\n", product);
    vector<tok_product_t> products;
    symbol_t &left = product.first;
    vector<token> &right = product.second;
    // 存储上一个分隔符（|）的位置
    auto lastIt = right.cbegin();
    // 存储下一个分隔符（|）的位置
    auto nextIt = right.cbegin();
    // 查找接下来的运算符（()、[]、{}）的开始符号的位置
    auto deliBeginIt = findType(right, get_tok_type("DELIMITER"), right.begin());
    if (deliBeginIt == right.end())
    {
        // 如果没有运算符，说明不需要递归下降解析，直接将其加入到结果产生式中即可
        products.push_back(product);
        info << "SyntaxParser: No delimiter found, return directly." << endl;
        return products;
    }
    while (lastIt != right.end())
    {
        if (deliBeginIt == right.end()) // 无运算符
        {
            products.push_back(make_pair(left, vector<token>(lastIt, right.cend())));
            break;
        }
        else if (deliBeginIt->value == "|")
        {
            // 如果是选择符，则把左边的产生式直接加入到结果中，并继续向右查找下一个分隔符
            products.push_back(make_pair(left, vector<token>(lastIt, deliBeginIt)));
            nextIt = deliBeginIt;
        }
        else
        {
            // 如果是其他运算符，则需要递归拆分
            vector<tok_product_t> resProducts({make_pair(left, vector<token>())});
            vector<tok_product_t> leadProducts;
            // 查找运算符（()、[]、{}）的结束符号的位置并解析
            // 这里允许运算符嵌套，如 (A|B|(C|D))，因此需要递归下降解析
            // 这里允许运算符组合，如 (A|B)[C|D]，因此需要循环解析
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

                // 遇到运算符，直接调用parseNonTrivialProducts解析
                // 这里的本质是递归下降法，因为 ()、[]、{} 中的内容可能包含 |、()、[]、{}，需要进一步拆分
                parseNonTrivialProducts(resProducts, left, deliBeginIt, deliEndIt);
                lastIt = deliEndIt + 1;
                deliBeginIt = findType(right, get_tok_type("DELIMITER"), lastIt);
            }

            leadProducts = vector<tok_product_t>({make_pair(left, vector<token>(lastIt, deliBeginIt))});
            fullConnProducts(resProducts, leadProducts);

            nextIt = deliBeginIt;
            // 将解析的结果加入到最终的结果中
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
        std::cout << pro << endl;
    }
    return products;
}

/**
 * @brief 将EBNF定义的文法解析为一系列产生式，并将其加入到文法中
 *
 * @param tokens EBNF定义的文法的token序列
 */
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
    // 预处理，拆分分隔符 ; 表示的多个产生式
    vector<tok_product_t> rawProducts;
    for (auto it = tokens.cbegin(); it != tokens.cend(); it++)
    {
        // 解析出产生式的左部
        assert(
            it->type == nonTermType,
            format(
                "SyntaxParser: EBNF syntax error: Expected non-terminal, got $ at <$, $>.",
                it->value, it->line, it->col));
        symbol_t left = it->value;
        it++;
        // 产生式的定义符号 ::=
        assert(
            it != tokens.cend() && it->type == grammarDef,
            format(
                "SyntaxParser: EBNF syntax error: Expected ::=, got $ at <$, $>.",
                it->value, it->line, it->col));
        it++;
        assert(it != tokens.cend());
        // 将分隔符之前的所有产生式右部加入到rawProducts中
        token_const_iter_t sepIt = findType(tokens, sepType, it);
        vector<token> right;
        for (auto it2 = it; it2 != sepIt; it2++)
        {
            right.push_back(*it2);
        }
        rawProducts.push_back(make_pair(left, right));
        // 更新迭代器，继续解析下一个产生式
        it = sepIt;
    }
    // 进一步解析，处理产生式中的运算符 |、()、[]、{}
    for (auto &pro : rawProducts)
    {
        // 递归下降法解析拆分EBNF
        vector<tok_product_t> subProducts = segmentProduct(pro);
        products.insert(products.end(), subProducts.begin(), subProducts.end());
    }
    // 处理产生式中的空产生式标记 ε
    for (auto &pro : products)
    {
        if (pro.second.size() == 1 && pro.second[0].type == epsilonType)
        {
            pro.second.clear();
        }
    }
    // 打印输出解析完毕后的产生式
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
    // 语义动作序列，用于挂载到对应的产生式中，由元信息解析器提供
    const vector<semantic_t> &semSeq = syntaxMeta.getMeta("SEMANTIC", vector<semantic_t>());
    // 记录语义动作的行数，便于后续挂载到对应行的产生式中
    map<size_t, semantic_t> semLocMap;
    // 记录产生式右部的行数，便于后续挂载语义动作
    map<size_t, product_t> proLocMap;
    size_t semIdx = 0; // 语义动作序列的下标
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
            if (tok.type == termType)
            {
                // 如果是终结符，需要将其加入到终结符集合中
                // 终结符一般以 `` 包裹，如 `a`，需要去掉 ` 符号
                tok.value = lrTrim(tok.value);
                terminals.insert(tok.value);
            }
            else if (tok.type == mulTermType)
            {
                // 如果是映射终结符，需要将其加入到终结符集合和映射终结符集合中
                // 映射终结符一般以 $ 开头，如 $Ident，需要去掉 $ 符号
                tok.value = tok.value.substr(1);
                terminals.insert(tok.value);
                mulTerms.insert(tok.value);
            }
            else if (tok.type == nonTermType)
            {
                // 如果是非终结符，需要将其加入到非终结符集合中
                nonTerms.insert(tok.value);
            }
            else
            {
                error << format(
                    "SyntaxParser: EBNF token error: Expected terminal, non-terminal, or semantic action, got $ ($) at <$, $>.\n",
                    tok.value, *tok.type, tok.line, tok.col);
            }
            // 将词素产生式右部的每个符号加入到产生式右部中
            // 注意，这里其实是tok_product_t到product_t的转换
            // 前者包含文法定义中的token信息，比如所在行列数和词法分析类型，后者只包含符号
            // 在进行EBNF解析时，需要用到前者，但是在进行语法分析时，只需要后者
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

/**
 * @brief 将MAPPING定义的映射关系加入到文法中
 *
 * @param tokens MAPPING定义的token序列
 */
void SyntaxParser::addTokenMappings(const vector<token> &tokens)
{
    // 这里的处理方式类似于addSyntaxRules
    // 区别是MAPPING解析不需要递归下降，因为MAPPING中不允许出现 |、()、[]、{} 等运算符
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
        // 解析出产生式的左部
        assert(
            it->type == mulTermType,
            format(
                "SyntaxParser: MAPPING syntax error: Expected mul-terminal, got $ at <$, $>.",
                it->value, it->line, it->col));
        symbol_t left = it->value;
        it++;
        // 产生式的定义符号 ->
        assert(
            it->type == mappingDef,
            format(
                "SyntaxParser: MAPPING syntax error: Expected ->, got $ at <$, $>.",
                it->value, it->line, it->col));
        it++;
        // 将分隔符之前的所有产生式右部加入到rawProducts中
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

/**
 * @brief 将PREC定义的优先级和结合性加入到文法中
 *
 */
void SyntaxParser::addPrecAndAssoc()
{
    info << "SyntaxParser: Adding precedence and associativity ..." << endl;
    map<symbol_t, prec_assoc_t> &precMap = grammar.precMap;
    // 如果元信息中定义了左结合、右结合、非结合的优先级信息，则将其加入到文法中
    if (syntaxMeta.hasMeta("%LEFT"))
    {
        const vector<meta_content_t> &leftSeq = syntaxMeta["%LEFT"];
        for (size_t i = 0; i < leftSeq.size(); i++)
        {
            vector<token> tokens = precLexer.tokenize(leftSeq[i]);
            for (auto &tok : tokens)
            {
                precMap[lrTrim(tok.value)] = make_pair(i, ASSOC_LEFT);
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
                precMap[lrTrim(tok.value)] = make_pair(i, ASSOC_RIGHT);
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
                precMap[lrTrim(tok.value)] = make_pair(i, ASSOC_NONE);
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

/**
 * @brief 将EBNF定义的文法解析为一系列产生式，并将其加入到文法中
 *
 * @param grammarPath EBNF定义的文法的元数据文件路径
 * @return Grammar 解析得到的文法
 */
Grammar SyntaxParser::parse(const string grammarPath)
{
    vector<token> tokens;
    // 读取EBNF定义的文法的元数据
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
