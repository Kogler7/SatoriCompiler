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
#include <vector>

enum t_align
{
    AL_LFT,
    AL_MID,
    AL_RGT,
};

#define TB_TAB "\t"
#define LF_TAB "\tl"
#define MD_TAB "\tm"
#define RT_TAB "\tr"
#define TB_GAP ""

class TableRender
{
    int rowCur;
    size_t colCur;
    size_t rowMax, colMax;
    size_t tabWidths;
    t_align tabAlign;
    std::vector<size_t> lines;
    std::vector<std::string> heads;
    std::vector<std::vector<std::string>> table;
    std::vector<size_t> widths;
    std::vector<t_align> aligns;

    void resetTabLayout();
    std::pair<size_t, t_align> calcTabLayout(std::string &s, size_t i);
    std::string geneField(std::string s, size_t width, t_align align);
    std::string geneLine();

public:
    enum op_type
    {
        ADD_HEAD,
        ADD_FIELD,
        SET_ALIGN
    };
    op_type op;

    TableRender();
    TableRender &reset();
    TableRender &setOp(op_type op);
    TableRender &setHead(std::string title);
    TableRender &setAlign(t_align align);
    TableRender &curAlign(t_align align);
    TableRender &addField(std::string field);
    TableRender &resetColCur();
    TableRender &resetRowCur();
    TableRender &nextRow();
    TableRender &setLine();
    TableRender &setPrevLine();
    std::string geneView();
    TableRender &operator|(std::string field);
    TableRender &operator|(t_align align);
};

extern TableRender _table;

#define set_col _table.resetColCur().setOp(TableRender::SET_ALIGN)

#define set_row _table.nextRow().setOp(TableRender::ADD_FIELD)

#define tb_line _table.setLine()

#define tb_head _table.reset().setOp(TableRender::ADD_HEAD)

#define tb_view _table.geneView()

#define tb_cont _table

#define set_cur_col(x) _table.curAlign(x)

#define tb_line_p _table.setPrevLine()