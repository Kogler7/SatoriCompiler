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
#include "utils/log.h"

using word_loc_t = std::pair<size_t, size_t>;
static constexpr inline const word_loc_t word_npos = std::make_pair(-1, -1);
static constexpr inline const word_loc_t word_end = std::make_pair(-1, 0);

class WordViewer : public Viewer
{
    bool isWord(word_loc_t loc) const
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
    WordViewer() = default;
    WordViewer(std::string str) : Viewer(str) {}
    WordViewer(Viewer &v) : Viewer(v) {}

    std::string operator[](word_loc_t loc) const
    {
        assert(loc != word_npos && loc != word_end, "Invalid word location!");
        return str.substr(loc.first, loc.second - loc.first);
    }

    std::vector<word_loc_t> wordsOfRestLine() const
    {
        std::vector<word_loc_t> ret;
        size_t start = current().first;
        size_t end = str.find('\n', pos);
        while (start < end)
        {
            word_loc_t loc = std::make_pair(start, start);
            while (loc.second < end && !isspace(str[loc.second]))
                loc.second++;
            if (loc.second > loc.first)
                ret.push_back(loc);
            start = loc.second;
            while (start < end && isspace(str[start]))
                start++;
        }
        return ret;
    }

    word_loc_t find(const std::string &word, word_loc_t start = word_npos) const
    {
        // 找到并返回第一个是单词且值为word的位置，否则返回word_npos
        size_t _pos;
        _pos = start == word_npos ? str.find(word) : str.find(word, start.second);
        while (_pos != std::string::npos)
        {
            word_loc_t loc = std::make_pair(_pos, _pos + word.length());
            if (isWord(loc))
                return loc;
            _pos = str.find(word, _pos + 1);
        }
        return word_npos;
    }

    word_loc_t current(size_t p = -1) const
    {
        // 返回loc指定的单词的位置
        size_t start = p == -1 ? pos : p;
        if (isspace(str[start]))
        {
            // 如果当前位置是空白，向前找到下一个单词的开头
            while (isspace(str[start]) && start < str.length())
                start++;
            if (start == str.length())
                return word_end;
        }
        else
        {
            // 如果当前位置不是空白，向后找到当前单词的开头
            while (start > 0 && !isspace(str[start - 1]))
                start--;
        }
        // 向后找到当前单词的结尾
        size_t end = start;
        while (end < str.length() && !isspace(str[end]))
            end++;
        return std::make_pair(start, end);
    }

    word_loc_t advance()
    {
        // 如果当前位置不是空白，向前找到当前单词的结尾空白
        while (pos < str.length() && !isspace(str[pos]))
            pos++;
        // 如果当前位置是空白，向前找到下一个单词的开头
        while (isspace(str[pos]) && pos < str.length())
            pos++;
        if (pos == str.length())
            return word_end;
        return current();
    }

    word_loc_t advance(word_loc_t loc) const
    {
        // 如果当前位置不是空白，向前找到当前单词的结尾空白
        while (loc.first < str.length() && !isspace(str[loc.first]))
            loc.first++;
        // 如果当前位置是空白，向前找到下一个单词的开头
        while (isspace(str[loc.first]) && loc.first < str.length())
            loc.first++;
        if (loc.first == str.length())
            return word_end;
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

    word_loc_t retreat(word_loc_t loc) const
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

    std::string swallow()
    {
        // 删掉当前单词
        word_loc_t loc = current();
        std::string ret = str.substr(loc.first, loc.second - loc.first);
        str.erase(loc.first, loc.second - loc.first);
        return ret;
    }

    std::string swallow(word_loc_t loc)
    {
        // 删掉loc指定的单词
        std::string ret = str.substr(loc.first, loc.second - loc.first);
        str.erase(loc.first, loc.second - loc.first);
        return ret;
    }

    void swallowWords(const std::vector<word_loc_t> &locs)
    {
        // 先排序
        std::vector<word_loc_t> locs_ = locs;
        sort(
            locs_.begin(), locs_.end(),
            [](const word_loc_t &a, const word_loc_t &b) -> bool
            { return a.first < b.first; });
        // 从后向前删掉locs指定的单词
        for (auto rit = locs_.rbegin(); rit != locs_.rend(); rit++)
            str.erase(rit->first, rit->second - rit->first);
    }

    WordViewer &replace(word_loc_t l1, word_loc_t l2, std::string word)
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

    bool terminate() const
    {
        // 判断是否已经到达末尾
        return pos >= str.length();
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