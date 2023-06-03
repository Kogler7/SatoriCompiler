/**
 * @file slr1/parser.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Simple LR(1) Parser
 * @date 2023-05-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "parser.h"
#include <stack>
#include "utils/table.h"
#include "utils/tok_view.h"

using namespace table;

template <typename T>
string descStack(stack<T> s, int limit = 8)
{
    stringstream ss;
    symstr_t v;
    while (!s.empty() && limit--)
    {
        stringstream t;
        t << s.top();
        v.push_back(t.str());
        s.pop();
    }
    if (limit <= 0)
        v.push_back("...");
    reverse(v.begin(), v.end());
    ss << container2str(v, " ", "");
    return ss.str();
}

pair<string, string> SimpleLR1Parser::descAction(const action_t &act)
{
    string a, b;
    stringstream ss;
    if (holds_alternative<shift_t>(act))
    {
        a = "Shift";
        ss << "S" << get<shift_t>(act);
    }
    else if (holds_alternative<reduce_t>(act))
    {
        product_t &reduce = get<reduce_t>(act).get();
        symbol_t left = reduce.first;
        symstr_t right = reduce.second;
        a = "Reduce";
        ss << left << " -> " << compact(right);
    }
    else if (holds_alternative<accept_t>(act) && get<accept_t>(act))
    {
        a = "Reduce";
        ss << grammar.symStart << " -> " << compact(*(grammar.rules[grammar.symStart].begin()));
    }
    else
    {
        a = "Error";
    }
    return make_pair(a, ss.str());
}

inline void printRemainingTreeNodes(stack<cst_node_ptr_t> &cstStk)
{
    while (!cstStk.empty())
    {
        cst_node_ptr_t node = cstStk.top();
        cstStk.pop();
        node->print();
        if (!cstStk.empty())
            std::cout << endl;
    }
}

bool SimpleLR1Parser::parse(vector<token> &input)
{
    info << "SimpleLR1Parser: Parsing..." << endl;
    // 初始化状态
    input.push_back(token(make_shared<symbol_t>(SYM_END), SYM_END, 0, 0));
    TokenViewer viewer(input);
    // 初始化栈
    stack<symbol_t> symStk;       // 符号栈
    stack<state_id_t> stateStk;   // 状态栈
    stack<cst_node_ptr_t> cstStk; // 语法树栈
    symStk.push(SYM_END);
    stateStk.push(0);
    // 初始化表格
    tb_head | "Symbol/State" | "Input" | "Action";
    set_row | AL_CTR;
    while (!stateStk.empty() && !symStk.empty() && !viewer.ends())
    {
        token &tok = viewer.current();
        state_id_t s = stateStk.top();
        symbol_t a = *(tok.type);
        action_t &act = grammar.slr1Table[mkcrd(s, a)];
        string act1, act2;
        tie(act1, act2) = descAction(act);
        new_row | Cell(descStack(symStk)) & AL_LFT | Cell(a) & AL_LFT | Cell(act1) & AL_LFT;
        new_row | descStack(stateStk) | Cell(descTokVecFrom(input, viewer.pos())) & AL_RGT | Cell(act2) & AL_RGT;
        tb_line();
        if (holds_alternative<shift_t>(act))
        {
            // 移进动作
            shift_t shift = get<shift_t>(act);
            symStk.push(a);
            stateStk.push(shift);
            cst_node_ptr_t node = cst_tree_t::createNode(TERMINAL, tok.value, tok.line, tok.col);
            cstStk.push(node);
            viewer.advance();
        }
        else if (holds_alternative<reduce_t>(act))
        {
            // 规约动作
            product_t &reduce = get<reduce_t>(act).get();
            symbol_t left = reduce.first;
            symstr_t right = reduce.second;
            size_t len = right.size();
            cst_node_ptr_t node = cst_tree_t::createNode(NON_TERM, left, 0, 0);
            vector<cst_node_ptr_t> children;
            for (size_t i = 0; i < len; i++)
            {
                symStk.pop();
                stateStk.pop();
                children.push_back(cstStk.top());
                cstStk.pop();
            }
            for (auto it = children.rbegin(); it != children.rend(); it++)
                *node << *it;
            symStk.push(left);
            cstStk.push(node);
            action_t &nAct = grammar.slr1Table[mkcrd(stateStk.top(), left)];
            if (holds_alternative<shift_t>(nAct))
                stateStk.push(get<shift_t>(nAct));
            else if (holds_alternative<accept_t>(nAct) && get<accept_t>(nAct))
                goto accept;
            else
                goto reject;
        }
        else if (holds_alternative<accept_t>(act) && get<accept_t>(act))
            goto accept;
        else
            goto reject;
    }
reject:
    error << "SimpleLR1Parser: Parsing failed!" << endl;
    new_row | TB_TAB | MD_TAB | Cell("Rejected") & FORE_RED;
    std::cout << tb_view();
    printRemainingTreeNodes(cstStk);
    return false;
accept:
    info << "SimpleLR1Parser: Parsing succeed!" << endl;
    symstr_t right = *(grammar.rules[grammar.symStart].begin());
    cst_node_ptr_t startNode = cst_tree_t::createNode(NON_TERM, grammar.symStart, 0, 0);
    vector<cst_node_ptr_t> children;
    for (size_t i = 0; i < right.size(); i++)
    {
        symStk.pop();
        stateStk.pop();
        children.push_back(cstStk.top());
        cstStk.pop();
    }
    for (auto it = children.rbegin(); it != children.rend(); it++)
        *startNode << *it;
    tree = startNode;
    new_row | Cell(descStack(symStk)) & AL_LFT | MD_TAB | Cell("Accepted") & FORE_GRE;
    std::cout << tb_view();
    return true;
}