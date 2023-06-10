/**
 * @file elr/parser.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Extended SLR(1) Parser
 * @date 2023-06-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stack>
#include <functional>

#include "parser.h"
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
            for (size_t i = 0; i < len; i++)
            {
                symStk.pop();
                stateStk.pop();
                *node << cstStk.top();
                cstStk.pop();
            }
            node->reverseChildren();
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
    for (size_t i = 0; i < right.size(); i++)
    {
        symStk.pop();
        stateStk.pop();
        *startNode << cstStk.top();
        cstStk.pop();
    }
    startNode->reverseChildren();
    cst = startNode;
    new_row | Cell(descStack(symStk)) & AL_LFT | MD_TAB | Cell("Accepted") & FORE_GRE;
    std::cout << tb_view();
    return true;
}

pst_tree_ptr_t ExtendedSimpleLR1Parser::reduceCST()
{
    info << "ExtendedSimpleLR1Parser: Reducing CST... (CST->RST)" << endl;
    const Grammar &g = this->grammar;
    symset_t &mulTerms = grammar.mulTerms;
    symset_t &nonTerms = grammar.nonTerms;
    symset_t &terminals = grammar.terminals;
    stack<pst_node_ptr_t> rstStk;
    cst->postorder(
        [&](pst_node_t node)
        {
            // 创建新的RST节点
            pst_node_ptr_t rstNode = pst_tree_t::createNode(node.data);
            // 如果当前节点代表非终结符，按照其产生式构造简化的RST节点
            // 因为是后序遍历，所以子节点已经被压入栈中了
            // 如果当前节点代表终结符，直接压入栈中并返回即可
            if (node.data.type == NON_TERM)
            {
                assert(node.data.product_opt.has_value());
                product_t &product = node.data.product_opt.value();
                symstr_t &right = product.second;
                reduced_product_t reducedProduct = g.reduceProduct(product);
                rstNode->attachProduct(reducedProduct);
                // 如果产生式右部只有一个终结符，那么将其作为RST节点的数据保留
                if (right.size() == 1 && _find(terminals, right[0]))
                {
                    pst_node_ptr_t child = rstStk.top();
                    rstStk.pop();
                    *rstNode << child;
                }
                else
                {
                    // 逆序遍历子节点，将其弹出栈并添加到新的RST节点中
                    for (auto it = right.rbegin(); it != right.rend(); it++)
                    {
                        pst_node_ptr_t child = rstStk.top();
                        rstStk.pop();
                        // RST节点仅保留非终结符（non-term）和带有映射信息的字面量终结符（mul-term）
                        if (_find(nonTerms, *it) || _find(mulTerms, *it))
                        {
                            *rstNode << child;
                        }
                    }
                    // 逆转子节点顺序
                    rstNode->reverseChildren();
                }
            }
            // 将新的RST节点压入栈中
            rstStk.push(rstNode);
        });
    // 最后栈中只剩下一个RST节点，即为最终的RST
    rst = rstStk.top();
    return rst;
}

void refactorStarListNode(pst_node_ptr_t astNode, vector<size_t> indexes)
{
    for (auto idx : indexes)
    {
        pst_node_ptr_t target = astNode->getChildAt(idx);
        // 创建一个新的AST节点，将原来的子节点遍历处理后添加到新的节点中
        pst_node_ptr_t listNode = pst_tree_t::createNode(target->data);
        listNode->data.symbol = "StarList";
        // 递归遍历子节点
        function<void(pst_node_ptr_t)> traverse = [&](pst_node_ptr_t node)
        {
            const vector<pst_node_ptr_t> &children = node->getChildren();
            if (children.size() > 0)
            {
                for (int i = 0; i < children.size() - 1; i++)
                {
                    *listNode << children[i];
                }
                traverse(children.back());
            }
        };
        traverse(target);
        // 用新节点替换原来的节点
        astNode->replace(idx, listNode);
        // 下面进行AST简化整合
        if (astNode->getChildren().size() == 1)
        {
            // 如果当前节点只有一个子节点，说明其只有一个ListNode节点
            // 此时删掉ListNode节点，将其子节点提升到当前节点
            const vector<pst_node_ptr_t> &children = listNode->getChildren();
            astNode->pop_back();
            for (auto &child : children)
            {
                *astNode << child;
            }
        }
        else if (astNode->getChildren().size() == 2)
        {
            // 如果当前节点有两个子节点，说明其有一个ListNode节点和一个非ListNode节点
            // 此时判断非ListNode节点是否与ListNode节点的子节点是同类型
            // 如果是，则将ListNode子节点提升到当前节点，删掉ListNode节点
            const vector<pst_node_ptr_t> &children = listNode->getChildren();
            pst_node_ptr_t nonListNode = astNode->getChildAt(0);
            if (children.size() == 0)
            {
                astNode->pop_back();
                continue;
            }
            if (nonListNode->data.symbol == children[0]->data.symbol)
            {
                astNode->pop_back();
                for (auto &child : children)
                {
                    *astNode << child;
                }
            }
        }
    }
}

void refactorOptionalNode(pst_node_ptr_t astNode, vector<size_t> indexes)
{
    vector<pst_node_ptr_t> &children = astNode->getChildren();
    for (auto idx : indexes)
    {
        children[idx]->data.symbol = "Optional";
    }
}

pst_tree_ptr_t ExtendedSimpleLR1Parser::refactorRST()
{
    info << "ExtendedSimpleLR1Parser: Refactoring RST... (RST->AST)" << endl;
    stack<pst_node_ptr_t> astStk;
    rst->postorder(
        [&](pst_node_t node)
        {
            // 创建新的AST节点
            pst_node_ptr_t astNode = pst_tree_t::createNode(node.data);
            // 如果当前节点代表非终结符，先将栈中的子节点弹出并添加到新的AST节点中
            // 而后检查其产生式是否包含特殊非终结符（隐含有运算符信息）
            // 如果包含，则调用相应的函数进行处理
            // 如果当前节点代表终结符，直接压入栈中并返回即可
            if (node.data.type == NON_TERM)
            {
                assert(node.data.product_opt.has_value());
                product_t &product = node.data.product_opt.value();
                symbol_t &left = product.first;
                symstr_t &right = product.second;
                static vector<size_t> starIndexes, optiIndexes;
                starIndexes.clear();
                optiIndexes.clear();
                // 逆序遍历子节点，将其弹出栈并添加到新的AST节点中
                for (size_t i = right.size() - 1; i != -1; i--)
                {
                    symbol_t &s = right[i];
                    pst_node_ptr_t child = astStk.top();
                    astStk.pop();
                    *astNode << child;
                    // 如果右部有特殊非终结符，记录其位置
                    if (s.find("_star_") != string::npos)
                    {
                        starIndexes.push_back(i);
                    }
                    else if (s.find("_opti_") != string::npos)
                    {
                        optiIndexes.push_back(i);
                    }
                }
                // 逆转子节点顺序
                astNode->reverseChildren();
                // 处理特殊非终结符
                // 如果左部有特殊非终结符，说明不需要做额外处理
                // 因为我们需要向上找到第一个不含特殊非终结符的左部产生式，再统一处理
                if (optiIndexes.size() > 0)
                    refactorOptionalNode(astNode, optiIndexes);
                if (left.find("_star_") == string::npos && starIndexes.size() > 0)
                    refactorStarListNode(astNode, starIndexes);
            }
            // 将新的RST节点压入栈中
            astStk.push(astNode);
        });
    // 最后栈中只剩下一个AST节点，即为最终的AST
    ast = astStk.top();
    return ast;
}