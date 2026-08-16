#pragma once
#include <vector>
#include "chapter.h"
using namespace std;
class Document
{
private:
    vector<chapter> chapters;
    int curChap;
public:
    Document();
    void add_chap(const chapter& chap);
    void rmv_chap(int id);
    chapter& get_chap(int idx);
    const chapter& get_chap(int idx) const;
    int chap_ct() const;
    void print() const;
    bool move_left();
    bool move_right();
    bool move_down();
    bool move_up();
    void move_start();
    void move_end();
    bool word_frwd();
    bool word_backwrd();
    void go_top();
    void go_bottom();
    chapter& get_cur_chap();
    int get_cur_idx() const;
    void set_cur_idx(int idx);
    void new_chap();
    Line& get_cur_line();
    bool del_cur_line(Line& out);
    Line copy_cur_line();
    void add_after(const Line& ln);
    void add_before(const Line& ln);
};