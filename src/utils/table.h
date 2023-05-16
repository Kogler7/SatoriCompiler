/**
 * @file utils/table.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief My Custom Table Render
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "log.h"
#include <map>
#include <vector>

#define TB_TAB "\t"
#define LF_TAB "\tl"
#define MD_TAB "\tm"
#define RT_TAB "\tr"
#define TB_GAP ""

namespace table
{
    typedef unsigned char sign;
    typedef unsigned short int l_sign;
    enum align_t : sign
    {
        AL_LFT,
        AL_CTR,
        AL_RGT,
    };
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

    class Cell
    {
    public:
        l_sign style;
        align_t align;
        std::string field;
        Cell() : field("")
        {
            style = FONT_NON;
            align = AL_LFT;
        }
        Cell(std::string s) : field(s)
        {
            style = FONT_NON;
            align = AL_LFT;
        }
        Cell(std::string s, l_sign sty, align_t ali) : field(s)
        {
            style = sty;
            align = ali;
        }
        std::string render(size_t width);
    };

    class TableRender
    {
    public:
        TableRender();
        TableRender &reset();
        TableRender &carriRet();
        TableRender &lineFeed();
        TableRender &moveNext();
        TableRender &setLine(const int i = 0);
        TableRender &enableLineSetting(bool b = true);
        TableRender &useHorizDirection(bool b = true);
        TableRender &operator=(const l_sign style);
        TableRender &operator=(const align_t align);
        TableRender &operator=(const std::string &field);
        TableRender &operator|(const l_sign style);
        TableRender &operator|(const align_t align);
        TableRender &operator|(const std::string &field);
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

        void resize();
        void resetTabLayout();
        std::pair<size_t, align_t> calcTabLayout(const Cell &cell, size_t i, bool V);
        std::string geneField(const std::string &field, size_t width, size_t c, align_t align, l_sign style);
        std::string geneHorizLine(int r, bool L, bool R, bool V);
    };
}

extern table::TableRender _tableRender;

#define tb_head _tableRender.reset().lineFeed().setLine()

#define tb_cont _tableRender

#define set_col _tableRender.useHorizDirection().carriRet().enableLineSetting()

#define set_row _tableRender.useHorizDirection(false).lineFeed().enableLineSetting()

#define new_col _tableRender.useHorizDirection(false).lineFeed().carriRet().enableLineSetting(false)

#define new_row _tableRender.useHorizDirection().lineFeed().carriRet().enableLineSetting(false)

#define tb_line(x) _tableRender.setLine(x)

#define tb_view(s) _tableRender.geneView(s)