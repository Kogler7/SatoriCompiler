/**
 * @file utils/table.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief My Custom Table Render
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

/**
 * 本文件实现了一个自定义的表格渲染器
 * 该渲染器可以将一系列单元格渲染为表格，并且支持自定义表格的样式
 * 渲染器支持的样式包括：内容对齐、字体样式、颜色样式、边框样式
 * 渲染器提供了一系列的接口和操作符重载，用于快速设置单元格的样式
 */

#include <sstream>
#include <iomanip>
#include <algorithm>

#include "table.h"

using namespace table;

/**
 * @brief 将样式定义解析为ANSI控制序列的前后缀
 *
 * @param style 待解析的样式定义
 * @return std::pair<std::string, std::string> ANSI控制序列的前后缀
 */
std::pair<std::string, std::string> parseStyle(l_sign style)
{
    if (style == FONT_NON)
        return std::make_pair("", "");
    static int styles[5];
    static std::vector<int> vecStyles;
    vecStyles.clear();
    // 利用位运算将样式定义解析为ANSI控制序列前缀的方法调用标号序列
    styles[0] = style & 0x0f ? (style & 0x0f) + 30 : 0;
    styles[1] = style & 0xf0 ? ((style & 0xf0) >> 4) + 40 : 0;
    styles[2] = style & 0x100 ? 1 : 0;
    styles[3] = style & 0x200 ? 3 : 0;
    styles[4] = style & 0x400 ? 4 : 0;
    // 将ANSI控制序列前缀的方法调用标号序列转换为ANSI控制序列前缀
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

/**
 * @brief 重置渲染器
 *
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::reset()
{
    rowCur = colCur = -1;
    rowMax = colMax = 0;
    tabWidths = 0;
    tabAlign = AL_CTR;
    lineSetting = false;
    horizDirected = true;
    widths.assign(colMax, 1);
    hLines.assign(rowMax, false);
    vLines.assign(colMax, false);
    table.clear();
    posStack = std::stack<std::pair<int, int>>();
    return *this;
}

/**
 * @brief 调整表格的大小，使其能够容纳当前的行列索引
 *
 */
void TableRender::resize()
{
    if (colCur >= colMax)
    {
        colMax = colCur + 1;
        widths.resize(colMax, 1);
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

/**
 * @brief 将当前的行列索引压入栈中，并将行列索引移动到指定位置
 *
 * @param r 移动的行偏移量
 * @param c 移动的列偏移量
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::moveTo(int r, int c)
{
    posStack.push(std::make_pair(rowCur, colCur));
    rowCur += r;
    rowCur = rowCur >= -1 ? rowCur : -1;
    colCur += c;
    colCur = colCur >= -1 ? colCur : -1;
    resize(); // 若行列索引超出了表格的大小，则调整表格的大小
    return *this;
}

/**
 * @brief 将当前的行列索引压入栈中，并将行列索引跳转到指定位置
 *
 * @param r 跳转目标的行索引
 * @param c 跳转目标的列索引
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::jumpTo(int r, int c)
{
    posStack.push(std::make_pair(rowCur, colCur));
    rowCur = r;
    rowCur = rowCur >= -1 ? rowCur : -1;
    colCur = c < 0 ? colCur + c : c;
    colCur = c;
    resize(); // 若行列索引超出了表格的大小，则调整表格的大小
    return *this;
}

/**
 * @brief 将当前的行列索引跳转到栈顶的位置，并将栈顶的行列索引弹出
 *
 * @param n 跳转的次数
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
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

/**
 * @brief 将当前的行列索引跳转到行首或列首（回车）
 *
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::carriRet()
{
    if (horizDirected)
    {
        // 如果当前是水平方向，则将行索引跳转到行首
        colCur = -1;
    }
    else
    {
        // 如果当前是垂直方向，则将列索引跳转到列首
        rowCur = -1;
    }
    return *this;
}

/**
 * @brief 将当前的行列索引跳转到下一行或下一列（换行）
 *
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::lineFeed()
{
    if (horizDirected)
    {
        // 如果当前是水平方向，则将行索引跳转到下一行
        rowCur++;
    }
    else
    {
        // 如果当前是垂直方向，则将列索引跳转到下一列
        colCur++;
    }
    // 调整表格的大小
    resize();
    return *this;
}

/**
 * @brief 沿着当前方向向下移动一个单元格
 *
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::moveNext()
{
    if (horizDirected)
    {
        // 如果当前是水平方向，则将列索引跳转到下一列
        colCur++;
    }
    else
    {
        // 如果当前是垂直方向，则将行索引跳转到下一行
        rowCur++;
    }
    // 调整表格的大小
    resize();
    return *this;
}

/**
 * @brief 在指定的行或列上设置一条分割线
 *
 * @param i 分割线的位置，若为负数，则表示相对于当前位置的偏移量
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::setLine(const int i)
{
    if (horizDirected)
    {
        // 如果当前是水平方向，则在指定的行上设置一条水平分割线
        int row = i <= 0 ? rowCur + i : i;
        assert(
            row >= 0 && row < rowMax,
            format("Table Render: Row index out of range. rowCur = $, rowMax = $", rowCur, rowMax));
        hLines[row] = true;
    }
    else
    {
        // 如果当前是垂直方向，则在指定的列上设置一条垂直分割线
        int col = i <= 0 ? colCur + i : i;
        assert(
            col >= 0 && col < colMax,
            format("Table Render: Column index out of range. colCur = $, colMax = $", colCur, colMax));
        vLines[col] = true;
    }
    return *this;
}

/**
 * @brief 设置是否启用整行或整列的样式设置，用于批量设置整行或整列的样式
 *
 * @param b 是否启用整行或整列的样式设置
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::enableLineSetting(bool b)
{
    lineSetting = b;
    return *this;
}

/**
 * @brief 设置当前的渲染方向
 *
 * @param b 当前的渲染方向，true表示水平方向，false表示垂直方向
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::useHorizDirection(bool b)
{
    horizDirected = b;
    return *this;
}

/**
 * @brief 运算符重载，可以设置当前单元格的样式
 *
 * @param style 当前单元格的样式
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::operator=(const l_sign style)
{
    if (lineSetting)
    {
        // 如果启用了整行或整列的样式设置，则将当前行或列上的所有单元格的样式设置为指定的样式
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

/**
 * @brief 运算符重载，可以设置当前单元格的对齐方式
 *
 * @param align 当前单元格的对齐方式
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::operator=(const align_t align)
{
    if (lineSetting)
    {
        // 如果启用了整行或整列的样式设置，则将当前行或列上的所有单元格的对齐方式设置为指定的对齐方式
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

/**
 * @brief 运算符重载，可以设置当前单元格的内容
 *
 * @param field 当前单元格的内容
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::operator=(const std::string &field)
{
    // 在设置当前单元格的内容时，需要考虑很多情况
    // 比如当前所在列列宽的更新、同一行单元格的合并等
    size_t leadWidth;
    align_t align;
    l_sign style = FONT_NON;
    // 计算出当前单元格的内容的前导空格的长度和对齐方式
    // 前导空格的长度是指该单元格之前的所有空单元格的宽度之和
    // 对齐方式是指该单元格之前的所有空单元格的对齐方式
    // 前导空格的长度和对齐方式可能会受到前面单元格的影响
    // 这样设计的目的是为了支持单元格的合并，在视觉上多个单元格仅展示为一个单元格
    std::tie(leadWidth, align) = calcTabLayout(field, colCur, true);
    if (leadWidth == 0)
    {
        // 如果前导空格的长度为0，则说明当前单元格并不是合并单元格的组成部分
        // 因此需要考虑当前单元格的对齐方式，默认继承上一行单元格的对齐方式
        align = rowCur > 0 ? table[rowCur - 1][colCur].align : AL_DFT;
    }
    // 将单元格内容添加到表格中
    table[rowCur][colCur] = Cell(field, style, align);
    // 更新当前列的列宽
    // 如果当前单元格的内容为制表符（在这里代表单元格合并的标记）
    // 则不更新列宽，因为制表符的宽度须由后续单元格决定
    size_t fieldSize = field.starts_with("\t") ? 0 : field.length();
    if (leadWidth > 0)
    {
        // 如果前导空格的长度大于0，则说明当前单元格是合并单元格的组成部分
        // 因此需要考虑当前单元格的前导空格的长度
        // 如果前导空格的长度不足以容纳当前单元格的内容，则需要更新列宽
        widths[colCur] = std::max(
            widths[colCur],
            leadWidth >= fieldSize ? 1 : fieldSize - leadWidth);
    }
    else
    {
        // 如果前导空格的长度为0，则说明当前单元格并不是合并单元格的组成部分
        // 只需要将列宽更新为当前单元格的内容的最大长度即可
        widths[colCur] = std::max(widths[colCur], fieldSize);
    }
    return *this;
}

/**
 * @brief 运算符重载，将当前行列索引跳到下一个位置并设置单元格
 *
 * @param cell 当前单元格
 * @return TableRender& 渲染器自身的引用，用于链式调用
 */
TableRender &TableRender::operator|(const Cell &cell)
{
    this->moveNext();         // 将当前行列索引跳到下一个位置
    if (cell.field != "\x07") // 约定的空白符
        *this = cell.field;
    if (cell.style != FONT_NON)
        *this = cell.style;
    if (cell.align != AL_DFT)
        *this = cell.align;
    return *this;
}

/**
 * @brief 重置当前单元格的制表符布局，即清空合并记录
 *
 */
void TableRender::resetTabLayout()
{
    tabWidths = 0;
    tabAlign = AL_CTR;
}

/**
 * @brief 计算当前单元格的制表符布局，返回前导空格的长度和合并后单元格的对齐方式
 *
 * @param cell 待计算的单元格
 * @param c 当前单元格的列索引
 * @param V 是否开启了垂直分割线的渲染
 * @return std::pair<size_t, align_t>
 */
std::pair<size_t, align_t> TableRender::calcTabLayout(const Cell &cell, size_t c, bool V)
{
    std::pair<size_t, align_t> res;
    const std::string &s = cell.field;
    if (s.starts_with("\t"))
    {
        // 如果当前单元格的内容为制表符（在这里代表单元格合并的标记）
        // 则需要更新前导空格的长度和合并后单元格的对齐方式
        // 当前需要考虑的长度按照内容宽度为零计算
        // 但要考虑垂直分割线的宽度（3），没有垂直分割线就只算一个空格（1）
        tabWidths += widths[c] + ((V || vLines[c]) ? 3 : 1);
        // 设置合并后单元格的对齐方式
        // 这样设计，合并单元格的对齐方式实质上是由其内容单元格的前一个制表单元格决定的
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

/**
 * @brief 渲染单元格的内容
 *
 * @param field 单元格的内容
 * @param width 单元格的宽度
 * @param c 单元格的列索引
 * @param align 单元格的对齐方式
 * @param style 单元格的样式
 * @return std::string 渲染后的单元格内容
 */
std::string TableRender::geneField(const std::string &field, size_t width, size_t c, align_t align, l_sign style) const
{
    std::stringstream ss;
    std::string pre, post;
    // 解析样式定义的ANSI控制序列的前后缀
    std::tie(pre, post) = parseStyle(style);
    ss << pre; // 添加ANSI控制序列的前缀
    if (align == AL_CTR)
    {
        // 居中对齐（实际上是居中靠左）
        size_t l = (width - field.length()) / 2;
        size_t r = width - field.length() - l;
        ss << std::setw(l) << std::setfill(' ') << "";
        ss << (field == "\x07" ? " " : field); // 约定的空白符
        ss << std::setw(r) << std::setfill(' ') << "";
    }
    else
    {
        // 其他的对齐方式通过流的格式化输出来实现
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
    ss << post; // 添加ANSI控制序列的后缀
    return ss.str();
}

/**
 * @brief 渲染水平分割线
 *
 * @param r 水平分割线的行索引
 * @param L 是否渲染左边框
 * @param R 是否渲染右边框
 * @param V 是否渲染垂直分割线
 * @return std::string 渲染后的水平分割线
 */
std::string TableRender::geneHorizLine(int r, bool L, bool R, bool V) const
{
    assert(r < int(rowMax), "Table Render: Row index out of range.");
    static std::vector<bool> flags;
    flags.assign(colMax, false);
    // 计算出整个分割线中哪些位置需要渲染出节点，例如：+---+---+---+
    for (size_t c = 0; c < colMax - 1; c++)
    {
        flags[c] = V || vLines[c];
        if (r >= 0)
        {
            // 如果某个单元格位置上下都是合并单元格，说明不需要渲染节点
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
            // 如果该行是表格的第一行，且某个单元格是合并单元格，说明不需要渲染节点
            if (table[0][c].field.starts_with("\t"))
            {
                flags[c] = false;
            }
        }
    }
    std::stringstream ss;
    if (L)
        ss << "+-"; // 渲染左边框
    // 渲染分割线的内容
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
        ss << "-+"; // 渲染右边框
    ss << std::endl;
    return ss.str();
}

/**
 * @brief 渲染整个表格
 *
 * @param style 表格的边框样式
 * @return std::string 渲染后的表格
 */
std::string TableRender::geneView(sign style)
{
    // 先解析出表格的边框样式
    static bool T, B, L, R, V, H;
    T = style & BDR_TOP; // 是否渲染上边框
    B = style & BDR_BTM; // 是否渲染下边框
    L = style & BDR_LFT; // 是否渲染左边框
    R = style & BDR_RGT; // 是否渲染右边框
    V = style & BDR_VRT; // 是否渲染垂直分割线
    H = style & BDR_HRT; // 是否渲染水平分割线
    std::stringstream ss;
    if (T)
        ss << geneHorizLine(-1, L, R, V); // 渲染上边框
    size_t leadWidth;
    align_t align;
    // 逐行渲染表格内容
    for (size_t r = 0; r < rowMax; r++)
    {
        if (L)
            ss << "| "; // 渲染左边框
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
            ss << " |"; // 渲染右边框
        ss << std::endl;
        if (r != rowMax - 1 && (H || hLines[r]))
            ss << geneHorizLine(r, L, R, V); // 渲染水平分割线
    }
    if (B)
        ss << geneHorizLine(rowMax - 1, L, R, V); // 渲染下边框
    return ss.str();
}

// 用于全局的表格渲染器，可以直接使用宏定义来快速设置单元格的内容、样式和对齐方式
TableRender _tableRender;