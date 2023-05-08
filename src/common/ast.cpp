/**
 * @file ast.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Abstract Syntax Tree
 * @date 2023-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ast.h"

AbstractSyntaxTreeNode::AbstractSyntaxTreeNode(
    node_type typ, string sym, size_t ln, size_t co) : type(typ), symbol(sym), line(ln), col(co)
{
    parent = nullptr;
}

ast_node_ptr AbstractSyntaxTreeNode::createNode(node_type type, const string &symbol, size_t line, size_t col)
{
    return make_shared<ast_node>(type, symbol, line, col);
}

vector<ast_node>::reference AbstractSyntaxTreeNode::operator[](size_t index)
{
    return *(ast_childs::operator[](index));
}

ast_node &AbstractSyntaxTreeNode::operator<<(const ast_node_ptr node)
{
    node->parent = this;
    push_back(node);
    return *this;
}

size_t AbstractSyntaxTreeNode::size() const
{
    return ast_childs::size();
}

template <typename func_t>
void AbstractSyntaxTreeNode::foreach (func_t f) const
{
    auto nodeF = [=](ast_childs::const_reference ref)
    {
        f(*ref);
    };
    for_each(ast_childs::begin(), ast_childs::end(), nodeF);
}

template <typename func_t>
void AbstractSyntaxTreeNode::traverse(func_t f) const
{
    f(*this);
    foreach (
        [=](ast_node &ref)
        { ref.traverse(f); })
        ;
}

template <typename func_t>
void AbstractSyntaxTreeNode::traverse(func_t f, int &level, int &index)
{
    ast_node &self = *this;
    f(self);
    level++;
    int tmpIdx = index++;
    index = 0;
    foreach (
        [&](ast_node &ref)
        {
            ref.traverse(f, level, index);
            index++;
        })
        ;
    level--;
    index = tmpIdx;
}

void AbstractSyntaxTreeNode::dump()
{
    vector<bool> visible;
    int level = 0;
    int index = 0;
    traverse(
        [&](ast_node &node)
        {
            if (visible.size() <= level)
                visible.push_back(true);
            if (level > 0 && index == node.parent->size() - 1)
                visible[level - 1] = false;
            auto getHead = [=](int level) -> string
            {
                int i = 0;
                string ret;
                while (i < level - 1)
                {
                    if (visible[i])
                        ret += "|  ";
                    else
                        ret += "   ";
                    i++;
                }
                if (level > 0)
                    ret += "|--";
                return ret;
            };
            cout << getHead(level);
            if (node.type == NON_TERM)
                cout << node.symbol;
            else
                cout << '`' << node.symbol << '`';
            cout << " <" << node.line << ", " << node.col << ">";
            cout << endl;
            if (level > 0)
                for (int i = level; i < visible.size(); i++)
                    visible[i] = true;
        },
        level, index);
}

void AbstractSyntaxTreeNode::dumpSave(const string &file)
{
    ofstream ofstream;
    ofstream.open(file);

    vector<bool> visible;
    int level = 0;
    int index = 0;
    traverse(
        [&](ast_node &node)
        {
            if (visible.size() <= level)
                visible.push_back(true);
            if (level > 0 && index == node.parent->size() - 1)
                visible[level - 1] = false;
            auto getHead = [=](int level) -> string
            {
                int i = 0;
                string ret;
                while (i < level - 1)
                {
                    if (visible[i])
                        ret += "|  ";
                    else
                        ret += "   ";
                    i++;
                }
                if (level > 0)
                    ret += "|--";
                return ret;
            };
            ofstream << getHead(level);
            if (node.type == NON_TERM)
                ofstream << node.symbol;
            else
                ofstream << '`' << node.symbol << '`';
            ofstream << " <" << node.line << ", " << node.col << ">";
            ofstream << endl;
            if (level > 0)
                for (int i = level; i < visible.size(); i++)
                    visible[i] = true;
        },
        level, index);
}