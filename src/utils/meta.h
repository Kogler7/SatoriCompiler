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
using meta_mark_t = string;
using meta_content_t = string;
using meta_t = pair<meta_name_t, vector<meta_content_t>>;

inline bool operator<(const word_loc_t &a, const word_loc_t &b)
{
    return a.first < b.first;
}

inline string trim(const string &str)
{
    string::size_type pos = str.find_first_not_of(" \t\n\r\f\v");
    if (pos == string::npos)
    {
        return str;
    }
    string::size_type pos2 = str.find_last_not_of(" \t\n\r\f\v");
    if (pos2 != string::npos)
    {
        return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
}

class MetaParser
{
    WordViewer viewer;
    map<meta_name_t, meta_t> metas;
    map<meta_mark_t, set<meta_name_t>> metaMap;

public:
    MetaParser(Viewer &v) : viewer(v)
    {
        parseMetaMarks();
        parseMetas(viewer.getStr());
        printMetas();
    }

    void parseMetaMarks()
    {
        word_loc_t metaDefLoc;
        string metaName, metaMark;
        metaDefLoc = viewer.find("#meta");
        if (metaDefLoc == word_npos)
        {
            warn << "MetaParser: No meta definition found." << endl;
            return;
        }
        while (metaDefLoc != word_npos)
        {
            viewer.jumpToLoc(metaDefLoc).advance();
            metaName = viewer.swallow();
            metaMark = viewer.swallow();
            info << "MetaParser: Meta mark " << metaMark << " defined as " << metaName << endl;
            metaDefLoc = viewer.find("#meta", metaDefLoc);
            assert(metaMark.length() == 4, "MetaParser: Invalid meta mark.");
            metaMap[metaMark].insert(metaName);
            metas[metaName] = meta_t(metaName, vector<meta_content_t>());
        }
        info << viewer.getStr() << endl;
    }

    meta_content_t parseMetas(string text)
    {
        WordViewer vTmp(text);
        meta_mark_t metaMark;
        set<meta_name_t> metaNameSet;
        word_loc_t lastLoc = make_pair(0, 0);
        word_loc_t nextLoc = word_npos;
        for (auto &mark : metaMap)
        {
            word_loc_t tmp = vTmp.find(mark.first.substr(0, 2));
            if (nextLoc > tmp)
            {
                nextLoc = tmp;
                metaMark = mark.first;
                metaNameSet = mark.second;
            }
        }
        while (nextLoc != word_npos)
        {
            info << "find" << vTmp[nextLoc] << endl;
            meta_name_t hintName = vTmp[vTmp.retreat(nextLoc)];
            info << "Hint name: " << hintName << endl;
            meta_name_t metaName;
            metaName = metaNameSet.find(hintName) != metaNameSet.end() ? hintName : *metaNameSet.begin();
            lastLoc = vTmp.find(metaMark.substr(2, 2), nextLoc);
            assert(lastLoc != word_npos, "MetaParser: Invalid meta mark.");
            meta_content_t metaContent = parseMetas(
                vTmp.getStr().substr(nextLoc.second, lastLoc.first - nextLoc.second));
            metas[metaName].second.push_back(trim(metaContent));
            vTmp.replace(nextLoc, lastLoc, metaName);
            lastLoc = nextLoc;
            nextLoc = word_npos;
            for (auto &mark : metaMap)
            {
                word_loc_t tmp = vTmp.find(mark.first.substr(0, 2), lastLoc);
                if (nextLoc > tmp)
                {
                    nextLoc = tmp;
                    metaMark = mark.first;
                    metaNameSet = mark.second;
                }
            }
        }
        return vTmp.getStr();
    }

    void printMetas()
    {
        info << "MetaParser: Printing metas ..." << endl;
        for (auto &meta : metas)
        {
            info << "MetaParser: Meta " << meta.first << " defined as:" << endl;
            for (auto &content : meta.second.second)
                std::cout << content << endl;
        }
    }
};