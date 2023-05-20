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
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace table;

std::pair<std::string, std::string> parseStyle(const std::string &s, l_sign style)
{
    if (style == FONT_NON)
        return std::make_pair("", "");
    static int styles[5];
    static std::vector<int> vecStyles;
    vecStyles.clear();
    styles[0] = style & 0x0f ? (style & 0x0f) + 30 : 0;
    styles[1] = style & 0xf0 ? ((style & 0xf0) >> 4) + 40 : 0;
    styles[2] = style & 0x100 ? 1 : 0;
    styles[3] = style & 0x200 ? 3 : 0;
    styles[4] = style & 0x400 ? 4 : 0;
    std::stringstream ss;
    ss << "\033[";
    for (int i = 0; i < 5; i++)
    {
        if (styles[i])
            vecStyles.push_back(styles[i]);
    }
    for (int i = 0; i < vecStyles.size(); i++)
    {
        if (i != 0)
            ss << ";";
        ss << vecStyles[i];
    }
    ss << "m";
    return std::make_pair(ss.str(), "\033[0m");
}

TableRender::TableRender()
{
    reset();
}

TableRender &TableRender::reset()
{
    rowCur = colCur = -1;
    rowMax = colMax = 0;
    tabWidths = 0;
    tabAlign = AL_CTR;
    lineSetting = false;
    horizDirected = true;
    widths.clear();
    hLines.assign(rowMax, false);
    vLines.assign(colMax, false);
    table.clear();
    posStack = std::stack<std::pair<int, int>>();
    return *this;
}

void TableRender::resize()
{
    if (colCur >= colMax)
    {
        colMax = colCur + 1;
        widths.resize(colMax, 0);
        for (auto &row : table)
            row.resize(colMax);
        vLines.resize(colMax, false);
    }
    if (rowCur >= rowMax)
    {
        size_t newMax = rowCur + 1;
        table.resize(newMax);
        for (size_t i = rowMax; i < newMax; i++)
            table[i].resize(colMax);
        rowMax = newMax;
        hLines.resize(rowMax, false);
    }
}

TableRender &TableRender::moveTo(int r, int c)
{
    posStack.push(std::make_pair(rowCur, colCur));
    rowCur += r;
    rowCur = rowCur >= -1 ? rowCur : -1;
    colCur += c;
    colCur = colCur >= -1 ? colCur : -1;
    resize();
    return *this;
}

TableRender &TableRender::jumpTo(int r, int c)
{
    posStack.push(std::make_pair(rowCur, colCur));
    rowCur = r;
    rowCur = rowCur >= -1 ? rowCur : -1;
    colCur = c < 0 ? colCur + c : c;
    colCur = c;
    resize();
    return *this;
}

TableRender &TableRender::JumpBack(int n)
{
    while (n--)
    {
        if (posStack.empty())
            break;
        std::tie(rowCur, colCur) = posStack.top();
        posStack.pop();
    }
    return *this;
}

TableRender &TableRender::carriRet()
{
    if (horizDirected)
    {
        colCur = -1;
    }
    else
    {
        rowCur = -1;
    }
    return *this;
}

TableRender &TableRender::lineFeed()
{
    if (horizDirected)
    {
        rowCur++;
    }
    else
    {
        colCur++;
    }
    resize();
    return *this;
}

TableRender &TableRender::moveNext()
{
    if (horizDirected)
    {
        colCur++;
    }
    else
    {
        rowCur++;
    }
    resize();
    return *this;
}

TableRender &TableRender::setLine(const int i)
{
    if (horizDirected)
    {
        int row = i <= 0 ? rowCur + i : i;
        assert(
            row >= 0 && row < rowMax,
            format("Table Render: Row index out of range. rowCur = $, rowMax = $", rowCur, rowMax));
        hLines[row] = true;
    }
    else
    {
        int col = i <= 0 ? colCur + i : i;
        assert(
            col >= 0 && col < colMax,
            format("Table Render: Column index out of range. colCur = $, colMax = $", colCur, colMax));
        vLines[col] = true;
    }
    return *this;
}

TableRender &TableRender::enableLineSetting(bool b)
{
    lineSetting = b;
    return *this;
}

TableRender &TableRender::useHorizDirection(bool b)
{
    horizDirected = b;
    return *this;
}

TableRender &TableRender::operator=(const l_sign style)
{
    if (lineSetting)
    {
        if (horizDirected)
        {
            for (size_t r = 0; r < rowMax; r++)
                table[r][colCur].style = style;
        }
        else
        {
            for (size_t c = 0; c < colMax; c++)
                table[rowCur][c].style = style;
        }
    }
    else
    {
        table[rowCur][colCur].style = style;
    }
    return *this;
}

TableRender &TableRender::operator=(const align_t align)
{
    if (lineSetting)
    {
        if (horizDirected)
        {
            for (size_t r = 0; r < rowMax; r++)
                table[r][colCur].align = align;
        }
        else
        {
            for (size_t c = 0; c < colMax; c++)
                table[rowCur][c].align = align;
        }
    }
    else
    {
        table[rowCur][colCur].align = align;
    }
    return *this;
}

TableRender &TableRender::operator=(const std::string &field)
{
    size_t leadWidth;
    align_t align;
    l_sign style = FONT_NON;
    std::tie(leadWidth, align) = calcTabLayout(field, colCur, true);
    if (leadWidth == 0)
    {
        align = rowCur > 0 ? table[rowCur - 1][colCur].align : AL_DFT;
    }
    table[rowCur][colCur] = Cell(field, style, align);
    size_t fSize = field.starts_with("\t") ? 0 : field.length();
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
    return *this;
}

TableRender &TableRender::operator|(const Cell &cell)
{
    this->moveNext();
    if (cell.field != "\x07")
        *this = cell.field;
    if (cell.style != FONT_NON)
        *this = cell.style;
    if (cell.align != AL_DFT)
        *this = cell.align;
    return *this;
}

void TableRender::resetTabLayout()
{
    tabWidths = 0;
    tabAlign = AL_CTR;
}

std::pair<size_t, align_t> TableRender::calcTabLayout(const Cell &cell, size_t i, bool V)
{
    std::pair<size_t, align_t> res;
    const std::string &s = cell.field;
    if (s.starts_with("\t"))
    {
        tabWidths += widths[i] + ((V || vLines[i]) ? 3 : 1);
        if (s == LF_TAB)
            tabAlign = AL_LFT;
        else if (s == MD_TAB)
            tabAlign = AL_CTR;
        else if (s == RT_TAB)
            tabAlign = AL_RGT;
        else if (s != "\t")
            warn << "Table Render: Invalid tab align placeholder " << s << std::endl;
        res = std::make_pair(0, AL_CTR);
    }
    else
    {
        res = std::make_pair(tabWidths, tabWidths > 0 ? tabAlign : cell.align);
        tabWidths = 0;
        tabAlign = cell.align;
    }
    return res;
}

std::string TableRender::geneField(const std::string &field, size_t width, size_t c, align_t align, l_sign style)
{
    std::stringstream ss;
    std::string pre, post;
    std::tie(pre, post) = parseStyle(field, style);
    ss << pre;
    if (align == AL_CTR)
    {
        size_t l = (width - field.length()) / 2;
        size_t r = width - field.length() - l;
        ss << std::setw(l) << std::setfill(' ') << "";
        ss << (field == "\x07" ? " " : field);
        ss << std::setw(r) << std::setfill(' ') << "";
    }
    else
    {
        ss << std::setw(width) << std::setfill(' ');
        switch (align)
        {
        case AL_DFT:
            break;
        case AL_LFT:
            ss << std::left;
            break;
        case AL_RGT:
            ss << std::right;
            break;
        }
        ss << (field == "\x07" ? " " : field);
    }
    ss << post;
    return ss.str();
}

std::string TableRender::geneHorizLine(int r, bool L, bool R, bool V)
{
    assert(r < int(rowMax), "Table Render: Row index out of range.");
    static std::vector<bool> flags;
    flags.assign(colMax, false);
    for (size_t c = 0; c < colMax - 1; c++)
    {
        flags[c] = V || vLines[c];
        if (r >= 0)
        {
            if (table[r][c].field.starts_with("\t"))
            {
                if (r == rowMax - 1 || table[r + 1][c].field.starts_with("\t"))
                {
                    flags[c] = false;
                }
            }
        }
        else
        {
            if (table[0][c].field.starts_with("\t"))
            {
                flags[c] = false;
            }
        }
    }
    std::stringstream ss;
    if (L)
        ss << "+-";
    for (size_t i = 0; i < colMax; i++)
    {
        ss << std::string(widths[i], '-');
        if (i != colMax - 1)
        {
            if (V || vLines[i])
                ss << (flags[i] ? "-+-" : "---");
            else
                ss << "-";
        }
    }
    if (R)
        ss << "-+";
    ss << std::endl;
    return ss.str();
}

std::string TableRender::geneView(sign style)
{
    static bool T, B, L, R, V, H;
    T = style & BDR_TOP;
    B = style & BDR_BTM;
    L = style & BDR_LFT;
    R = style & BDR_RGT;
    V = style & BDR_VRT;
    H = style & BDR_HRT;
    std::stringstream ss;
    if (T)
        ss << geneHorizLine(-1, L, R, V);
    size_t leadWidth;
    align_t align;
    for (size_t r = 0; r < rowMax; r++)
    {
        if (L)
            ss << "| ";
        resetTabLayout();
        for (size_t c = 0; c < colMax; c++)
        {
            Cell &cell = table[r][c];
            std::tie(leadWidth, align) = calcTabLayout(cell, c, V);
            if (!cell.field.starts_with("\t"))
            {
                ss << geneField(cell.field, leadWidth + widths[c], c, align, cell.style);
                if (c != colMax - 1)
                {
                    if (V || vLines[c])
                        ss << " | ";
                    else
                        ss << " ";
                }
            }
        }
        if (R)
            ss << " |";
        ss << std::endl;
        if (r != rowMax - 1 && (H || hLines[r]))
            ss << geneHorizLine(r, L, R, V);
    }
    if (B)
        ss << geneHorizLine(rowMax - 1, L, R, V);
    return ss.str();
}

TableRender _tableRender;