/**
 * @file elr/parser.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Extended SLR(1) Parser
 * @date 2023-06-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "parser.h"
#include <stack>
#include "utils/table.h"
#include "utils/view/tok_view.h"

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

pair<string, string> ExtendedSimpleLR1Parser::descAction(const action_t &act) const
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
        ss << grammar.symStart << " -> " << compact(*(grammar.rules.at(grammar.symStart).begin()));
    }
    else
    {
        a = "Error";
    }
    return make_pair(a, ss.str());
}

inline void printRemainingTreeNodes(stack<pst_node_ptr_t> &cstStk)
{
    while (!cstStk.empty())
    {
        pst_node_ptr_t node = cstStk.top();
        cstStk.pop();
        node->print();
        if (!cstStk.empty())
            std::cout << endl;
    }
}

bool ExtendedSimpleLR1Parser::parse(vector<token> &input, const ContextViewer &code)
{
    info << "ExtendedSimpleLR1Parser: Parsing..." << endl;
    // 初始化状态
    input.push_back(token(make_shared<symbol_t>(SYM_END), SYM_END, 0, 0));
    TokenViewer viewer(input);
    // 初始化栈
    stack<symbol_t> symStk;       // 符号栈
    stack<state_id_t> stateStk;   // 状态栈
    stack<pst_node_ptr_t> cstStk; // 语法树栈
    symStk.push(SYM_END);
    stateStk.push(0);
    // 初始化表格
    tb_head | "Symbol/State" | "Input" | "Action";
    set_row | AL_CTR;
    token &tok = viewer.current();
    while (!stateStk.empty() && !symStk.empty() && !viewer.ends())
    {
        tok = viewer.current();
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
            pst_node_ptr_t node = pst_tree_t::createNode(TERMINAL, tok.value, tok.line, tok.col);
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
            pst_node_ptr_t node = pst_tree_t::createNode(NON_TERM, left, 0, 0);
            node->attachProduct(reduce);
            vector<pst_node_ptr_t> children;
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
    error << "ExtendedSimpleLR1Parser: Parsing failed!" << endl;
    new_row | TB_TAB | MD_TAB | Cell("Rejected") & FORE_RED;
    std::cout << tb_view();
    info << "ExtendedSimpleLR1Parser: Related context:" << endl;
    tok = viewer.current();
    code.printContext(tok.line, tok.col);
    info << "ExtendedSimpleLR1Parser: Remaining cst nodes:" << endl;
    printRemainingTreeNodes(cstStk);
    return false;
accept:
    info << "ExtendedSimpleLR1Parser: Parsing succeed!" << endl;
    grammar.updateStartProduct();
    product_t &startProduct = grammar.startProduct;
    symstr_t &right = startProduct.second;
    pst_node_ptr_t startNode = pst_tree_t::createNode(NON_TERM, grammar.symStart, 0, 0);
    startNode->attachProduct(startProduct);
    vector<pst_node_ptr_t> children;
    for (size_t i = 0; i < right.size(); i++)
    {
        symStk.pop();
        stateStk.pop();
        children.push_back(cstStk.top());
        cstStk.pop();
    }
    for (auto it = children.rbegin(); it != children.rend(); it++)
        *startNode << *it;
    cst = startNode;
    new_row | Cell(descStack(symStk)) & AL_LFT | MD_TAB | Cell("Accepted") & FORE_GRE;
    std::cout << tb_view();
    return true;
}