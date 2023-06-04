/**
 * @file utils/word_viewer.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Lightweight Word Viewer
 * @date 2023-06-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "viewer.h"

using word_loc_t = pair<size_t, size_t>;
static constexpr inline const word_loc_t word_npos = make_pair(-1, -1);

class WordViewer : public Viewer
{
    bool isWord(word_loc_t loc)
    {
        if (loc.first > 0 && !isspace(str[loc.first - 1]))
            return false;
        if (loc.second < str.length() && !isspace(str[loc.second]))
            return false;
        return true;
    }

public:
    void operator=(Viewer &v)
    {
        Viewer::operator=(v);
    }
    WordViewer(string str) : Viewer(str) {}
    WordViewer(Viewer &v) : Viewer(v) {}

    string operator[](word_loc_t loc)
    {
        return str.substr(loc.first, loc.second - loc.first);
    }

    word_loc_t find(const string &word, word_loc_t start = word_npos)
    {
        // 找到并返回第一个是单词且值为word的位置，否则返回word_npos
        size_t _pos;
        _pos = start == word_npos ? str.find(word) : str.find(word, start.second);
        while (_pos != string::npos)
        {
            word_loc_t loc = make_pair(_pos, _pos + word.length());
            if (isWord(loc))
                return loc;
            _pos = str.find(word, _pos + 1);
        }
        return word_npos;
    }

    word_loc_t current()
    {
        // 返回当前单词的位置
        size_t start = pos;
        if (isspace(str[start]))
        {
            // 如果当前位置是空白，向前找到下一个单词的开头
            while (isspace(str[start]) && start < str.length())
                start++;
        }
        else
        {
            // 如果当前位置不是空白，向后找到当前单词的开头
            while (start > 0 && !isspace(str[start - 1]))
                start--;
        }
        pos = start;
        // 向后找到当前单词的结尾
        size_t end = start;
        while (end < str.length() && !isspace(str[end]))
            end++;
        return make_pair(start, end);
    }

    word_loc_t advance()
    {
        // 如果当前位置不是空白，向前找到当前单词的结尾空白
        while (pos < str.length() && !isspace(str[pos]))
            pos++;
        // 如果当前位置是空白，向前找到下一个单词的开头
        while (isspace(str[pos]) && pos < str.length())
            pos++;
        return current();
    }

    word_loc_t advance(word_loc_t loc)
    {
        // 如果当前位置不是空白，向前找到当前单词的结尾空白
        while (loc.first < str.length() && !isspace(str[loc.first]))
            loc.first++;
        // 如果当前位置是空白，向前找到下一个单词的开头
        while (isspace(str[loc.first]) && loc.first < str.length())
            loc.first++;
        loc.second = loc.first;
        // 向后找到下一个单词的结尾
        while (loc.second < str.length() && !isspace(str[loc.second]))
            loc.second++;
        return loc;
    }

    word_loc_t retreat()
    {
        // 如果当前位置不是空白，向后找到当前单词的前导空白
        while (pos > 0 && !isspace(str[pos]))
            pos--;
        // 如果当前位置是空白，向后找到上一个单词的结尾
        while (isspace(str[pos]) && pos > 0)
            pos--;
        // 如果当前位置不是空白，向后找到当前单词的开头
        while (pos > 0 && !isspace(str[pos - 1]))
            pos--;
        return current();
    }

    word_loc_t retreat(word_loc_t loc)
    {
        // 如果当前位置不是空白，向后找到当前单词的前导空白
        while (loc.first > 0 && !isspace(str[loc.first]))
            loc.first--;
        // 如果当前位置是空白，向后找到上一个单词的结尾
        while (isspace(str[loc.first]) && loc.first > 0)
            loc.first--;
        // 记录下上一个单词的结尾
        loc.second = loc.first + 1;
        // 如果当前位置不是空白，向后找到当前单词的开头
        while (loc.first > 0 && !isspace(str[loc.first - 1]))
            loc.first--;
        return loc;
    }

    string swallow()
    {
        // 删掉当前单词
        word_loc_t loc = current();
        string ret = str.substr(loc.first, loc.second - loc.first);
        str.erase(loc.first, loc.second - loc.first);
        return ret;
    }

    string swallow(word_loc_t loc)
    {
        // 删掉loc指定的单词
        string ret = str.substr(loc.first, loc.second - loc.first);
        str.erase(loc.first, loc.second - loc.first);
        return ret;
    }

    WordViewer &replace(word_loc_t l1, word_loc_t l2, string word)
    {
        // 用word替换l1到l2的单词
        str.replace(l1.first, l2.second - l1.first, word);
        // 更新pos
        if (pos > l1.first)
        {
            if (pos > l2.second)
                pos = pos - (l2.second - l1.first) + word.length();
            else
                pos = l1.first + word.length();
        }
        return *this;
    }

    WordViewer &jumpToLoc(word_loc_t loc)
    {
        // 跳转到loc
        pos = loc.first;
        return *this;
    }

    WordViewer &jumpHome()
    {
        // 跳转到开头
        pos = 0;
        return *this;
    }
};