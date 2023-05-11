/**
 * @file log.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Level-based Log
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "log.h"

StrFormatter::StrFormatter(std::string s)
{
    std::stringstream ss(s);
    std::string seg;
    while (std::getline(ss, seg, '$'))
        segs.push(seg);
}

std::string StrFormatter::str()
{
    while (!segs.empty())
    {
        ss << segs.front();
        segs.pop();
    }
    return ss.str();
}

TableRender _table;

TableRender::TableRender()
{
    op = ADD_HEAD;
    reset();
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
    table[rowCur][colCur] = field;
    widths[colCur] = std::max(widths[colCur], field.size());
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
    return *this;
}

TableRender &TableRender::setLine()
{
    assert(rowCur < rowMax, "Row index out of range.");
    lines.push_back(rowCur);
    return *this;
}

std::string TableRender::geneView()
{
    std::stringstream ss;
    ss << geneLine();
    for (size_t i = 0; i < colMax; i++)
    {
        ss << geneField(heads[i], widths[i], aligns[i]);
    }
    ss << "|" << std::endl;
    ss << geneLine();
    for (size_t i = 0; i < rowMax; i++)
    {
        size_t tab_widths = 0;
        t_align tab_align = AL_MID;
        for (size_t j = 0; j < colMax; j++)
        {
            std::string s = table[i][j];
            if (s.starts_with("\t"))
            {
                tab_widths += widths[j] + 3;
                if (s == LF_TAB)
                    tab_align = AL_LFT;
                else if (s == MD_TAB)
                    tab_align = AL_MID;
                else if (s == RT_TAB)
                    tab_align = AL_RGT;
                else
                    warn << "Table Render: Invalid tab align placeholder " << s << std::endl;
            }
            else
            {
                ss << geneField(s, widths[j] + tab_widths, tab_widths > 0 ? tab_align : aligns[j]);
                tab_widths = 0;
                tab_align = AL_MID;
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