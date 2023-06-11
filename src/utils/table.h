/**
 * @file utils/table.h
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

#pragma once

#include "log.h"

#include <map>
#include <vector>
#include <stack>

// TAB在渲染器中控制同一行中的单元格内容合并
#define TB_TAB "\t"

// 合并后的单元格的内容的对齐方式由TAB后的字符决定
#define LF_TAB "\tl"
#define MD_TAB "\tm"
#define RT_TAB "\tr"

// GAP在渲染器中代表一个空的单元格，即跳过该单元格的渲染
#define TB_GAP ""

namespace table
{
    using sign = unsigned char;
    using l_sign = unsigned short int;

    // 用于设置单元格的内容对齐方式
    enum align_t : sign
    {
        AL_DFT,
        AL_LFT,
        AL_CTR,
        AL_RGT,
    };

    // 用于设置单元格的字体样式、颜色样式
    // 由于颜色样式和字体样式是互斥的，因此可以使用位运算来组合样式
    // 比如：FORE_RED | BACK_GRE | FONT_BOL代表绿色背景下的红色粗体字
    enum style_t : l_sign
    {
        FONT_NON = 0b0000000000000000,
        FORE_RED = 0b0000000000000001,
        FORE_GRE = 0b0000000000000010,
        FORE_YEL = 0b0000000000000011,
        FORE_BLU = 0b0000000000000100,
        FORE_PUR = 0b0000000000000101,
        FORE_CYA = 0b0000000000000110,
        FORE_WHI = 0b0000000000000111,
        BACK_RED = 0b0000000000010000,
        BACK_GRE = 0b0000000000100000,
        BACK_YEL = 0b0000000000110000,
        BACK_BLU = 0b0000000001000000,
        BACK_PUR = 0b0000000001010000,
        BACK_CYA = 0b0000000001100000,
        BACK_WHI = 0b0000000001110000,
        FONT_BOL = 0b0000000100000000,
        FONT_ITA = 0b0000001000000000,
        FONT_UND = 0b0000010000000000,
    };

    // 用于设置单元格的边框样式
    // 其中部分边框样式是互斥的，因此可以使用位运算来组合样式
    // 比如：BDR_RUD | BDR_VRT代表含有垂线和外部边框的样式
    enum border_t : sign
    {
        BDR_NON = 0b000000, // No Border
        BDR_LFT = 0b000001, // Left Border
        BDR_TOP = 0b000010, // Top Border
        BDR_RGT = 0b000100, // Right Border
        BDR_BTM = 0b001000, // Bottom Border
        BDR_VRT = 0b010000, // Vertical Border
        BDR_HRT = 0b100000, // Horizontal Border
        BDR_ALL = 0b111111, // All Border
        BDR_RUD = 0b001111, // Rounded Border
    };

    /**
     * @brief 单元格内容，可以由string内容隐式构造
     * 
     */
    class Cell
    {
    public:
        l_sign style;
        align_t align;
        std::string field;
        // 各类构造函数，用于快速设置单元格的内容、样式和对齐方式
        Cell() : field("\x07"), style(FONT_NON), align(AL_DFT) {}
        Cell(const char *s) : field(s), style(FONT_NON), align(AL_DFT) {}
        Cell(l_sign sty) : field("\x07"), style(sty), align(AL_DFT) {}
        Cell(align_t ali) : field("\x07"), style(FONT_NON), align(ali) {}
        Cell(std::string s) : field(s), style(FONT_NON), align(AL_DFT) {}
        Cell(std::string s, l_sign sty, align_t ali) : field(s), style(sty), align(ali) {}

        // 重载的运算符，可以使用&运算符快速设置单元格的内容、样式和对齐方式
        Cell &operator&(const char *s)
        {
            field = s;
            return *this;
        }
        Cell &operator&(const l_sign sty)
        {
            style = sty;
            return *this;
        }
        Cell &operator&(const align_t ali)
        {
            align = ali;
            return *this;
        }
        Cell &operator&(const std::string &s)
        {
            field = s;
            return *this;
        }
    };

    /**
     * @brief 表格渲染器核心类
     * 
     */
    class TableRender
    {
    public:
        TableRender();
        TableRender &reset();
        TableRender &moveTo(int r, int c);
        TableRender &jumpTo(int r, int c);
        TableRender &JumpBack(int n = 1);
        TableRender &carriRet();
        TableRender &lineFeed();
        TableRender &moveNext();
        TableRender &setLine(const int i = 0);
        TableRender &enableLineSetting(bool b = true);
        TableRender &useHorizDirection(bool b = true);
        TableRender &operator=(const l_sign style);
        TableRender &operator=(const align_t align);
        TableRender &operator=(const std::string &field);
        TableRender &operator|(const Cell &cell);
        std::string geneView(sign style = BDR_RUD | BDR_VRT);

    private:
        int rowCur, colCur;
        size_t rowMax, colMax;
        size_t tabWidths;
        align_t tabAlign;
        bool lineSetting;
        bool horizDirected;
        std::vector<size_t> widths;
        std::vector<bool> hLines;
        std::vector<bool> vLines;
        std::vector<std::vector<Cell>> table;
        std::stack<std::pair<int, int>> posStack;

        void resize();
        void resetTabLayout();
        std::pair<size_t, align_t> calcTabLayout(const Cell &cell, size_t i, bool V);
        std::string geneField(const std::string &field, size_t width, size_t c, align_t align, l_sign style) const;
        std::string geneHorizLine(int r, bool L, bool R, bool V) const;
    };
}

// 用于全局的表格渲染器，可以直接使用宏定义来快速设置单元格的内容、样式和对齐方式
// 该方式可以使得代码更加简洁、易读，但不是线程安全的
extern table::TableRender _tableRender;

// 设置表格的头部，即设置表格的第一行，调用该宏后，表格渲染器会先清空表格，然后跳转到第一行
#define tb_head _tableRender.reset().lineFeed().setLine()
// 接着上面继续操作，可以使用tb_cont来继续设置表格的内容
#define tb_cont _tableRender

// 设置表格的列，调用该宏后，表格渲染器会跳转到第一列，并开启整列设置
#define set_col _tableRender.useHorizDirection().carriRet().enableLineSetting()
// 设置表格的行，调用该宏后，表格渲染器会跳转到下一行，并开启整行设置
#define set_row _tableRender.useHorizDirection(false).carriRet().enableLineSetting()

// 新建表格的列，调用该宏后，表格渲染器会跳转到下一列，并关闭整列设置
#define new_col _tableRender.useHorizDirection(false).lineFeed().carriRet().enableLineSetting(false)
// 新建表格的行，调用该宏后，表格渲染器会跳转到下一行，并关闭整行设置
#define new_row _tableRender.useHorizDirection().lineFeed().carriRet().enableLineSetting(false)

// 相对偏移到指定的行和列
#define tb_move(r, c) _tableRender.moveTo(r, c)
// 绝对跳转到指定的行和列
#define tb_jump(r, c) _tableRender.jumpTo(r, c)
// 返回到上一个跳转点
#define tb_back(n) _tableRender.JumpBack(n)

// 新建一条分割线，该线可以是水平线，也可以是垂直线，可以设置偏移量
#define tb_line(x) _tableRender.setLine(x)
// 返回表格的渲染结果，可以设置边框样式
#define tb_view(s) _tableRender.geneView(s)