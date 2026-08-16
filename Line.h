#pragma once
#include <vector>
#include <iostream>
using namespace std;
class Line {
private:
    vector<char> text;
    int curCol;
    bool isupr(char ch) const;
    bool islower(char ch) const;
    char tolwr(char ch) const;
    char toupper(char ch) const;
public:
    Line();
    Line(const vector<char>& text);
    void insert_ch(int col, char ch);
    void dlt_chr(int col);
    void del_frm(int col);
    void toggl(int col);
    void indent();
    void unindent();
    int get_len() const;
    vector<char> get_txt() const;
    void settext(const vector<char>& nt);
    void print() const;
    bool is_emp() const;
    bool move_left();
    bool move_right();
    void moveToStart();
    void moveToEnd();
    int get_cur_col() const;
    void set_cur_col(int col);
    void insert_at_cur(char ch);
    void delete_at_Cur();
    void back_sp();
    void deleteFromCursor();
    void toggle_cursor();
    bool word_frwd();
    bool word_backwrd();
};