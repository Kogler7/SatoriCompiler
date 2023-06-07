/**
 * @file utils/meta.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Meta Definitions Parser
 * @date 2023-06-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "meta.h"

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

void MetaParser::parseMetaMarks()
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
        viewer.jumpToLoc(metaDefLoc);
        vector<word_loc_t> metaDecls = viewer.wordsOfRestLine();
        if (metaDecls.size() > 1)
        {
            meta_name_t name = viewer[metaDecls[1]];
            meta_mark_t mark;
            mark.first = metaDecls.size() > 2 ? viewer[metaDecls[2]] : name;
            mark.second = metaDecls.size() > 3 ? viewer[metaDecls[3]] : "";
            metaMap[mark].insert(name);
            info << format(
                "MetaParser: Meta mark <$, $> defined as [$].\n",
                mark.first, mark.second, name);
        }
        else
        {
            warn << "MetaParser: Too few meta declaration marks." << endl;
        }
        viewer.swallowWords(metaDecls);
        metaDefLoc = viewer.find("#meta");
    }
}

meta_content_t MetaParser::parseMetas(string text)
{
    WordViewer vTmp(text);
    meta_mark_t metaMark;
    set<meta_name_t> metaNameSet;
    word_loc_t lastLoc = make_pair(0, 0);
    word_loc_t nextLoc = word_npos;
    for (auto &mark_name : metaMap)
    {
        word_loc_t tmp = vTmp.find(mark_name.first.first);
        if (nextLoc > tmp)
        {
            nextLoc = tmp;
            metaMark = mark_name.first;
            metaNameSet = mark_name.second;
        }
    }
    while (nextLoc != word_npos)
    {
        meta_name_t hintName = vTmp[vTmp.retreat(nextLoc)];
        meta_name_t metaName;
        metaName = metaNameSet.find(hintName) != metaNameSet.end() ? hintName : *metaNameSet.begin();
        if (metaMark.second.size() == 0)
        {
            lastLoc.first = vTmp.getStr().find('\n', nextLoc.second);
            if (lastLoc.first == string::npos)
                lastLoc.first = vTmp.getStr().length();
            lastLoc.second = lastLoc.first;
        }
        else
        {
            lastLoc = vTmp.find(metaMark.second, nextLoc);
        }
        assert(lastLoc != word_npos, "MetaParser: Invalid meta mark.");
        meta_content_t metaContent = parseMetas(
            vTmp.getStr().substr(nextLoc.second, lastLoc.first - nextLoc.second));
        metas[metaName].push_back(trim(metaContent));
        vTmp.replace(nextLoc, lastLoc, metaName);
        lastLoc = nextLoc;
        nextLoc = word_npos;
        for (auto &mark_name : metaMap)
        {
            word_loc_t tmp = vTmp.find(mark_name.first.first, lastLoc);
            if (nextLoc > tmp)
            {
                nextLoc = tmp;
                metaMark = mark_name.first;
                metaNameSet = mark_name.second;
            }
        }
    }
    return vTmp.getStr();
}

const meta_t &MetaParser::operator[](meta_name_t name) const
{
    return metas.at(name);
}

const meta_t &MetaParser::getMeta(meta_name_t name, const meta_t init) const
{
    return hasMeta(name) ? metas.at(name) : init;
}

const bool MetaParser::hasMeta(meta_name_t name) const
{
    return metas.find(name) != metas.end();
}

void MetaParser::printMetas() const
{
    info << "MetaParser: Printing metas ..." << endl;
    for (auto &meta : metas)
    {
        info << "MetaParser: Meta " << meta.first << " defined as:" << endl;
        for (auto &content : meta.second)
            std::cout << content << endl;
    }
}