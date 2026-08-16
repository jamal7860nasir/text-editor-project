#pragma once
#include <vector>
#include "Line.h"
using namespace std;
class Paragraph
{
private:
    vector<Line> lines;
    int curLine;
public:
    Paragraph();
    int get_line_ct() const;
    vector<Line> get_line() const;
    void setLines(const vector<Line>& nl);
    Line& line_rf(int index);
    const Line& getLine(int index) const;
    void insrtLine(int index, const Line& line);
    void apndLine(const Line& li);
    void del_line(int id);
    void splitLine(int l_id, int cl);
    void join_line(int id);
    bool emty() const;
    void print() const;
    bool move_left();
    bool move_right();
    bool move_down();
    bool move_up();
    void move_start();
    void move_end();
    bool word_frwd();
    bool word_backwrd();
    Line& get_cur_line();
    int get_cur_idx() const;
    void set_cur_idx(int idx);
    bool del_cur_line(Line& out);
    Line copy_cur_line();
    void add_after(const Line& ln);
    void add_before(const Line& ln);
};