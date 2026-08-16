#pragma once
#include <vector>
#include "Paragraph.h"
using namespace std;
class section
{
private:
    vector<Paragraph> paragraphs;
    int curPara;
public:
    section();
    void add_para(const Paragraph& para);
    void rmv_para(int id);
    Paragraph& get_para(int idx);
    const Paragraph& getParagraph(int idx) const;
    int para_ct() const;
    void print() const;
    bool move_left();
    bool move_right();
    bool move_down();
    bool move_up();
    void move_start();
    void move_end();
    bool word_frwd();
    bool word_backwrd();
    Paragraph& get_cur_para();
    int get_cur_idx() const;
    void set_cur_idx(int idx);
    bool del_cur_line(Line& out);
    Line copy_cur_line();
    void add_after(const Line& ln);
    void new_para();
    void add_before(const Line& ln);
};