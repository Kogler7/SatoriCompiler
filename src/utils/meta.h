/**
 * @file utils/meta.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Meta Definitions Parser
 * @date 2023-06-03
 *
 * @copyright Copyright (c) 2023
 *
 */

/**
 * @brief Meta Definitions Parser
 * 核心特性：
 * 1、支持嵌套定义
 * 2、支持多个定义
 * 3、文件标记格式：#meta <name> <mark>
 * 4、内容标记格式：[name] <mark-1> <content> <mark-2>
 * 5、嵌套内容解析后 [name] <mark-1> <content> <mark-2> 会被替换为 <name>
 */

#pragma once

#include "view/wrd_view.h"
#include "utils/log.h"
#include <map>
#include <set>

using meta_name_t = string;
using meta_mark_t = pair<string, string>;
using meta_content_t = string;
using meta_t = vector<meta_content_t>;

static inline const meta_t meta_null = vector<meta_content_t>();

inline bool operator<(const word_loc_t &a, const word_loc_t &b)
{
    return a.first < b.first;
}

class MetaParser
{
    WordViewer viewer;
    map<meta_name_t, meta_t> metas;
    map<meta_mark_t, set<meta_name_t>> metaMap;

    void parseMetaMarks();
    meta_content_t parseMetas(string text);

public:
    MetaParser() = default;
    MetaParser(Viewer &v) : viewer(v)
    {
        parseMetaMarks();
        parseMetas(viewer.getStr());
    }

    static MetaParser fromFile(string filename)
    {
        Viewer v = Viewer::fromFile(filename);
        return MetaParser(v);
    }

    const meta_t &operator[](meta_name_t name) const;
    const meta_t &getMeta(meta_name_t name, const meta_t init) const;
    const bool hasMeta(meta_name_t name) const;
    void printMetas() const;
};