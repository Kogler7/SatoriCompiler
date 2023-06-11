/**
 * @file elr/parser.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Extended SLR(1) Parser
 * @date 2023-06-05
 *
 * @copyright Copyright (c) 2023
 *
 */

/**
 * 本文件实现扩展的SLR1文法分析程序
 * 该程序的主要功能是根据文法定义文件的声明将输入的token序列转换为抽象语法树
 * 相比于SLR1文法分析程序，该程序的主要改动在于：
 * 1、支持扩展的文法定义，即除了EBNF文法和映射之外，还支持语义动作和优先级与结合性的解析
 * 2、支持扩展的文法分析，即除了SLR1文法分析之外，还支持基于优先级解决移进-规约冲突的LR1文法分析
 * 3、支持扩展的语法树构建，即除了构建CST之外，还支持构建RST和AST，便于后续翻译步骤
 */

#include "parser.h"
#include "utils/table.h"
#include "utils/view/tok_view.h"

#include <stack>
#include <functional>

using namespace std;
using namespace table;

/**
 * @brief 将符号栈转换为字符串，用于打印输出
 *
 * @tparam T 栈内元素类型
 * @param s 符号栈
 * @param limit 限制打印的元素数量，用于控制输出长度
 * @return string 符号栈的字符串表示
 */
template <typename T>
string descStack(stack<T> s, int limit = 6)
{
    stringstream ss;
    symstr_t v;
    // 逆序遍历栈中元素，将其转换为字符串并添加到vector中
    // 最多添加limit个元素，超出部分用省略号代替
    while (!s.empty() && limit--)
    {
        stringstream t;
        t << s.top();
        v.push_back(t.str());
        s.pop();
    }
    // 如果栈中还有元素，说明超出了限制，添加省略号
    if (limit <= 0)
        v.push_back("...");
    // 逆序遍历vector，将其转换为字符串
    // 这里主要是为了将栈顶元素放在最后（右）面
    reverse(v.begin(), v.end());
    // 将vector转换为字符串
    ss << container2str(v, " ", "");
    return ss.str();
}

/**
 * @brief 将SLR1文法分析表中的动作(action)对转换为字符串，用于打印输出
 *
 * @param act  SLR1文法分析表中的动作(action)
 * @return pair<string, string> 动作(action)对的字符串表示，第一个元素为动作类型，第二个元素为动作内容
 */
pair<string, string> ExtendedSimpleLR1Parser::descAction(const action_t &act) const
{
    string a, b;
    stringstream ss;
    if (holds_alternative<shift_t>(act))
    {
        // 移进动作，此时动作内容为状态编号
        a = "Shift";
        ss << "S" << get<shift_t>(act);
    }
    else if (holds_alternative<reduce_t>(act))
    {
        // 规约动作，此时动作内容为产生式
        product_t &reduce = get<reduce_t>(act).get();
        symbol_t left = reduce.first;
        symstr_t right = reduce.second;
        a = "Reduce";
        ss << left << " -> " << compact(right);
    }
    else if (holds_alternative<accept_t>(act) && get<accept_t>(act))
    {
        // 接受动作，此时动作内容为接受标志
        a = "Reduce";
        ss << grammar.symStart << " -> " << compact(*(grammar.rules.at(grammar.symStart).begin()));
    }
    else
    {
        // 错误动作，此时动作内容为错误标志
        a = "Error";
    }
    return make_pair(a, ss.str());
}

/**
 * @brief 将树结点栈中剩余的树结点打印输出，用于在分析失败时输出分析站中剩余的语法树结点
 *
 * @param pstStk 解析树结点栈
 */
inline void printRemainingTreeNodes(stack<pst_node_ptr_t> &pstStk)
{
    while (!pstStk.empty())
    {
        // 对于栈中的每一个树结点，打印输出
        pst_node_ptr_t node = pstStk.top();
        pstStk.pop();
        node->print();
        if (!pstStk.empty())
            std::cout << endl;
    }
}

/**
 * @brief ESLR核心方分析过程，用于解析输入的token序列并构建CST
 *
 * @param input 输入的token序列
 * @param code 上下文浏览器，这里仅用于在出错时打印相关上下文信息
 * @return true 解析成功
 * @return false 解析失败
 */
bool ExtendedSimpleLR1Parser::parse(vector<token> &input, const ContextViewer &code)
{
    info << "ExtendedSimpleLR1Parser: Parsing..." << endl;
    // 初始化，将输入的token序列添加一个结束符号
    // 将输入的token序列转换为TokenViewer，方便后续遍历
    input.push_back(token(make_shared<symbol_t>(SYM_END), SYM_END, 0, 0));
    TokenViewer viewer(input);
    // 初始化栈
    stack<symbol_t> symStk;       // 符号栈
    stack<state_id_t> stateStk;   // 状态栈
    stack<pst_node_ptr_t> cstStk; // 解析树栈（CST）
    symStk.push(SYM_END);         // 符号栈初始加入结束符号
    stateStk.push(0);             // 状态栈初始加入状态 0
    // 初始化表头（用于打印输出分析过程）
    tb_head | "Symbol/State" | "Input" | "Action";
    set_row | AL_CTR;
    token &tok = viewer.current();
    while (!stateStk.empty() && !symStk.empty() && !viewer.ends())
    {
        // 逐步遍历输入流，直到输入流结束
        tok = viewer.current();                         // 获取输入流的当前token
        state_id_t s = stateStk.top();                  // 获取状态栈的栈顶状态
        symbol_t a = *(tok.type);                       // 获取输入流的当前token代表的终结符
        action_t &act = grammar.slr1Table[mkcrd(s, a)]; // 获取当前状态和当前终结符在SLR1分析表中对应的动作
        // 打印输出分析过程
        string act1, act2;
        tie(act1, act2) = descAction(act); // 将动作转换为字符串，用于打印输出
        // 将当前符号栈、状态栈、输入流和动作添加到打印表格中
        new_row | Cell(descStack(symStk)) & AL_LFT | Cell(a) & AL_LFT | Cell(act1) & AL_LFT;
        new_row | descStack(stateStk) | Cell(descTokVecFrom(input, viewer.pos())) & AL_RGT | Cell(act2) & AL_RGT;
        tb_line();
        // 根据动作类型进行相应的处理
        if (holds_alternative<shift_t>(act))
        {
            // 移进动作
            shift_t shift = get<shift_t>(act); // 获取移进动作对应的状态
            symStk.push(a);                    // 将当前终结符压入符号栈
            stateStk.push(shift);              // 将移进动作对应的状态压入状态栈
            // 创建一个新的CST叶子节点，将当前终结符作为其数据
            pst_node_ptr_t node = pst_tree_t::createNode(TERMINAL, tok.value, tok.line, tok.col);
            cstStk.push(node); // 将新的CST节点压入解析树栈
            viewer.advance();  // 将输入流向前移动一个token
        }
        else if (holds_alternative<reduce_t>(act))
        {
            // 规约动作
            product_t &reduce = get<reduce_t>(act).get(); // 获取规约动作对应的产生式
            symbol_t left = reduce.first;                 // 获取产生式左部
            symstr_t right = reduce.second;               // 获取产生式右部
            size_t len = right.size();                    // 获取产生式右部长度（即规约长度）
            // 创建一个新的CST非叶子节点，将产生式左部作为其数据
            // 该节点的子节点为规约长度个数的CST节点，这些CST节点是从解析树栈中弹出的
            pst_node_ptr_t node = pst_tree_t::createNode(NON_TERM, left, 0, 0);
            node->attachProduct(reduce); // 将产生式信息附加到新的CST节点上，便于后续构建RST和AST
            for (size_t i = 0; i < len; i++)
            {
                // 从解析树栈中弹出规约长度个数的CST节点，并添加到新的CST节点中
                // 这里是逆序添加，因为解析树栈中的CST节点是按照规约顺序压入的
                // 符号栈和状态栈中的元素也相应地弹出
                symStk.pop();
                stateStk.pop();
                *node << cstStk.top();
                cstStk.pop();
            }
            // 将新的CST节点的子节点顺序逆转，保证其顺序与产生式右部一致
            node->reverseChildren();
            symStk.push(left); // 将产生式左部压入符号栈
            cstStk.push(node); // 将新的CST节点压入解析树栈
            // 根据产生式左部和当前状态在goto表中查找，得到下一个状态
            action_t &nAct = grammar.slr1Table[mkcrd(stateStk.top(), left)];
            if (holds_alternative<shift_t>(nAct))
                stateStk.push(get<shift_t>(nAct)); // 如果下一个状态是移进状态，将新的状态号压入状态栈
            else if (holds_alternative<accept_t>(nAct) && get<accept_t>(nAct))
                goto accept; // 如果下一个状态是接受状态，说明分析成功，跳转到接受处理部分
            else
                goto reject; // 如果下一个状态是错误状态，说明分析失败，跳转到拒绝处理部分
        }
        // 接受动作
        else if (holds_alternative<accept_t>(act) && get<accept_t>(act))
            goto accept;
        else // 错误动作
            goto reject;
    }
reject: // 拒绝处理部分
    error << "ExtendedSimpleLR1Parser: Parsing failed!" << endl;
    new_row | TB_TAB | MD_TAB | Cell("Rejected") & FORE_RED;
    std::cout << tb_view(); // 打印输出分析表格
    info << "ExtendedSimpleLR1Parser: Related context:" << endl;
    tok = viewer.current();               // 获取当前token
    code.printContext(tok.line, tok.col); // 打印当前Token的相关上下文信息
    info << "ExtendedSimpleLR1Parser: Remaining cst nodes:" << endl;
    // 打印输出分析栈中剩余的CST节点，便于找出问题
    printRemainingTreeNodes(cstStk);
    return false;
accept: // 接受处理部分
    info << "ExtendedSimpleLR1Parser: Parsing succeed!" << endl;
    // 获取文法开始符号对应的产生式
    grammar.updateStartProduct();
    product_t &startProduct = grammar.startProduct;
    symstr_t &right = startProduct.second;
    // 创建一个新的CST根节点（整个CST的根节点），将文法开始符号作为其数据
    pst_node_ptr_t startNode = pst_tree_t::createNode(NON_TERM, grammar.symStart, 0, 0);
    startNode->attachProduct(startProduct); // 将文法开始符号对应的产生式信息附加到新的CST节点上
    for (size_t i = 0; i < right.size(); i++)
    {
        // 从解析树栈中弹出规约长度个数的CST节点，并添加到CST根节点中
        symStk.pop();
        stateStk.pop();
        *startNode << cstStk.top();
        cstStk.pop();
    }
    // 将CST根节点的子节点顺序逆转，保证其顺序与产生式右部一致
    startNode->reverseChildren();
    // 最终CST树的根节点即为文法开始符号对应的CST节点
    cst = startNode;
    // 打印输出分析表格
    new_row | Cell(descStack(symStk)) & AL_LFT | MD_TAB | Cell("Accepted") & FORE_GRE;
    std::cout << tb_view();
    return true;
}

/**
 * @brief 精简CST，将其转换为RST
 *
 * @return pst_tree_ptr_t 精简后的RST根节点
 */
pst_tree_ptr_t ExtendedSimpleLR1Parser::reduceCST()
{
    info << "ExtendedSimpleLR1Parser: Reducing CST... (CST->RST)" << endl;
    const Grammar &g = this->grammar;
    symset_t &mulTerms = grammar.mulTerms;
    symset_t &nonTerms = grammar.nonTerms;
    symset_t &terminals = grammar.terminals;
    // 用于构建RST的节点栈
    // 这里并不是在原来的CST上进行修改，而是在遍历CST的过程中挑选有用的信息构建新的RST
    stack<pst_node_ptr_t> rstStk;
    // 后序遍历CST，同时利用栈保存遍历过程中的节点，自底向上构建RST
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
                // 将产生式简化为仅包含有用信息的产生式
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

/**
 * @brief 重构AST节点，将该节点的右递归子节点平铺
 *
 * @param astNode 需要处理的AST节点
 * @param indexes 需要处理的子节点的索引
 */
void refactorStarListNode(pst_node_ptr_t astNode, vector<size_t> indexes)
{
    for (auto idx : indexes)
    {
        pst_node_ptr_t target = astNode->getChildAt(idx);
        // 创建一个新的AST节点，将原来的子节点遍历处理后添加到新的节点中
        pst_node_ptr_t listNode = pst_tree_t::createNode(target->data);
        // 新节点的命名为StarList，便于后续处理
        listNode->data.symbol = "StarList";
        // 递归遍历子节点
        function<void(pst_node_ptr_t)> traverse = [&](pst_node_ptr_t node)
        {
            const vector<pst_node_ptr_t> &children = node->getChildren();
            if (children.size() > 0)
            {
                // 将各层的非右递归子节点直接平铺添加的要处理的根节点中
                for (int i = 0; i < children.size() - 1; i++)
                {
                    *listNode << children[i];
                }
                // 在右递归的子结点中，最后一个子结点还是右递归的子结点
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

/**
 * @brief 重构AST节点，将该节点的可选子节点标记为Optional
 *
 * @param astNode 需要处理的AST节点
 * @param indexes 需要处理的子节点的索引
 */
void refactorOptionalNode(pst_node_ptr_t astNode, vector<size_t> indexes)
{
    vector<pst_node_ptr_t> &children = astNode->getChildren();
    for (auto idx : indexes)
    {
        children[idx]->data.symbol = "Optional";
    }
}

/**
 * @brief 重构RST，将其转换为AST
 *
 * @return pst_tree_ptr_t 重构后的AST根节点
 */
pst_tree_ptr_t ExtendedSimpleLR1Parser::refactorRST()
{
    info << "ExtendedSimpleLR1Parser: Refactoring RST... (RST->AST)" << endl;
    // 用于构建AST的节点栈
    // 重构的思路与精简CST类似，依然是在遍历RST的过程构建新的AST
    // 只是这里是并不是挑选有用的信息，而是构建的同时调用相应的函数进行重构处理
    stack<pst_node_ptr_t> astStk;
    // 后序遍历RST，同时利用栈保存遍历过程中的节点，自底向上构建AST
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
                // 记录产生式中特殊非终结符的位置
                // 特殊非终结符包括：_star_（表示0或多次）、_opti_（表示0或1次）
                // 这些特殊的非终结符是在EBNF解析的过程中添加的
                // 在语法分析时，这些特殊非终结符会被记录在CST中
                // 在完成语法分析后，这些程序自动添加的特殊非终结符就可以被删除了
                // 最后只留下用户自定义的非终结符构成的AST，便于后续语义分析的过程
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
                    refactorOptionalNode(astNode, optiIndexes); // 处理Optional节点
                if (left.find("_star_") == string::npos && starIndexes.size() > 0)
                    refactorStarListNode(astNode, starIndexes); // 处理StarList节点
            }
            // 将新的RST节点压入栈中
            astStk.push(astNode);
        });
    // 最后栈中只剩下一个AST节点，即为最终的AST
    ast = astStk.top();
    return ast;
}