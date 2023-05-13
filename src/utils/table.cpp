/**
 * @file utils/table.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief My Custom Table Render
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "table.h"
#include <iomanip>
#include <algorithm>

TableRender _table;

TableRender::TableRender()
{
    op = ADD_HEAD;
    reset();
}

void TableRender::resetTabLayout()
{
    tabWidths = 0;
    tabAlign = AL_MID;
}

std::pair<size_t, t_align> TableRender::calcTabLayout(std::string &s, size_t i)
{
    std::pair<size_t, t_align> res;
    if (s.starts_with("\t"))
    {
        tabWidths += widths[i] + 3;
        if (s == LF_TAB)
            tabAlign = AL_LFT;
        else if (s == MD_TAB)
            tabAlign = AL_MID;
        else if (s == RT_TAB)
            tabAlign = AL_RGT;
        else if (s != "\t")
            warn << "Table Render: Invalid tab align placeholder " << s << std::endl;
        res = std::make_pair(0, AL_MID);
    }
    else
    {
        res = std::make_pair(tabWidths, tabWidths > 0 ? tabAlign : aligns[i]);
        tabWidths = 0;
        tabAlign = AL_MID;
    }
    return res;
}

std::string TableRender::geneField(std::string s, size_t width, t_align align)
{
    std::stringstream ss;
    if (align == AL_MID)
    {
        size_t l = (width - s.length()) / 2;
        size_t r = width - s.length() - l;
        ss << "| " << std::setw(l) << std::setfill(' ') << "";
        ss << s;
        ss << std::setw(r) << std::setfill(' ') << "";
        ss << " ";
        return ss.str();
    }
    else
    {
        ss << "| " << std::setw(width) << std::setfill(' ');
        switch (align)
        {
        case AL_LFT:
            ss << std::left;
            break;
        case AL_RGT:
            ss << std::right;
            break;
        }
        ss << s << " ";
    }
    return ss.str();
}

std::string TableRender::geneLine()
{
    std::stringstream ss;
    for (size_t i = 0; i < colMax; i++)
        ss << "+-" << std::string(widths[i], '-') << "-";
    ss << "+" << std::endl;
    return ss.str();
}

TableRender &TableRender::reset()
{
    rowCur = -1;
    colCur = 0;
    rowMax = 1;
    colMax = 0;
    tabWidths = 0;
    tabAlign = AL_MID;
    lines.clear();
    heads.clear();
    table.clear();
    table.push_back(std::vector<std::string>());
    widths.clear();
    aligns.clear();
    return *this;
}

TableRender &TableRender::setOp(op_type op)
{
    this->op = op;
    return *this;
}

TableRender &TableRender::setHead(std::string title)
{
    if (colCur >= colMax)
    {
        colMax = colCur + 1;
        heads.resize(colMax);
        widths.resize(colMax);
        aligns.resize(colMax, AL_LFT);
        for (auto &row : table)
            row.resize(colMax);
    }
    heads[colCur] = title;
    widths[colCur] = std::max(widths[colCur], title.size());
    colCur++;
    resetTabLayout();
    return *this;
}

TableRender &TableRender::setAlign(t_align align)
{
    assert(colCur < colMax, "Column index out of range.");
    aligns[colCur] = align;
    colCur++;
    return *this;
}

TableRender &TableRender::curAlign(t_align align)
{
    assert(colCur <= colMax, "Column index out of range.");
    aligns[colCur > 0 ? colCur - 1 : 0] = align;
    return *this;
}

TableRender &TableRender::addField(std::string field)
{
    assert(colCur < colMax, "Column index out of range.");
    assert(rowCur >= 0 && rowCur < rowMax, "Row index out of range.");
    size_t leadWidth;
    t_align align;
    std::tie(leadWidth, align) = calcTabLayout(field, colCur);
    table[rowCur][colCur] = field;
    size_t fSize = field.size();
    if (leadWidth > 0)
    {
        widths[colCur] = std::max(
            widths[colCur],
            leadWidth >= fSize ? 0 : fSize - leadWidth);
    }
    else
    {
        widths[colCur] = std::max(widths[colCur], fSize);
    }
    colCur++;
    return *this;
}

TableRender &TableRender::resetColCur()
{
    colCur = 0;
    return *this;
}

TableRender &TableRender::resetRowCur()
{
    rowCur = 0;
    return *this;
}

TableRender &TableRender::nextRow()
{
    rowCur++;
    colCur = 0;
    if (rowCur >= rowMax)
    {
        rowMax = rowCur + 1;
        table.resize(rowMax);
        for (size_t i = 0; i < rowMax; i++)
            table[i].resize(colMax);
    }
    resetTabLayout();
    return *this;
}

TableRender &TableRender::setLine()
{
    assert(
        rowCur < int(rowMax),
        format("Table Render: Row index out of range. rowCur = $, rowMax = $", rowCur, rowMax));
    lines.push_back(rowCur);
    return *this;
}

TableRender &TableRender::setPrevLine()
{
    int i = rowCur - 1;
    assert(
        i >= 0 && i < int(rowMax),
        format("Table Render: Row index out of range. rowCur = $, rowMax = $", rowCur, rowMax));
    lines.push_back(i);
    return *this;
}

std::string TableRender::geneView()
{
    std::stringstream ss;
    ss << geneLine();
    resetTabLayout();
    size_t leadWidth;
    t_align align;
    std::string s;
    for (size_t i = 0; i < colMax; i++)
    {
        s = heads[i];
        std::tie(leadWidth, align) = calcTabLayout(s, i);
        if (!s.starts_with("\t"))
        {
            ss << geneField(s, leadWidth + widths[i], align);
        }
    }
    ss << "|" << std::endl;
    ss << geneLine();
    for (size_t i = 0; i < rowMax; i++)
    {
        resetTabLayout();
        for (size_t j = 0; j < colMax; j++)
        {
            s = table[i][j];
            std::tie(leadWidth, align) = calcTabLayout(s, j);
            if (!s.starts_with("\t"))
            {
                std::string str = geneField(s, leadWidth + widths[j], align);
                ss << str;
            }
        }
        ss << "|" << std::endl;
        if (std::find(lines.begin(), lines.end(), i) != lines.end() && i != rowMax - 1)
            ss << geneLine();
    }
    ss << geneLine();
    return ss.str();
}

TableRender &TableRender::operator|(std::string field)
{
    switch (op)
    {
    case ADD_HEAD:
        setHead(field);
        break;
    case ADD_FIELD:
        addField(field);
        break;
    default:
        warn << "Invalid operation type." << std::endl;
        break;
    }
    return *this;
}

TableRender &TableRender::operator|(t_align align)
{
    switch (op)
    {
    case SET_ALIGN:
        setAlign(align);
        break;
    default:
        warn << "Invalid operation type." << std::endl;
        break;
    }
    return *this;
}