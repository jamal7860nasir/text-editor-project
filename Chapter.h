#pragma once
#include <vector>
#include "Section.h"
using namespace std;
class chapter
{
private:
    vector<section> sections;
    int curSec;
public:
    chapter();
    void add_sec(const section& sec);
    void rmv_sec(int id);
    section& get_sec(int idx);
    const section& get_sec(int idx) const;
    int sec_ct() const;
    void print() const;
    bool move_left();
    bool move_right();
    bool move_down();
    bool move_up();
    void move_start();
    void move_end();
    bool word_frwd();
    bool word_backwrd();
    section& get_cur_sec();
    int get_cur_idx() const;
    void set_cur_idx(int idx);
    bool del_cur_line(Line& out);
    Line copy_cur_line();
    void new_sec();
    void add_after(const Line& ln);
    void add_before(const Line& ln);
};