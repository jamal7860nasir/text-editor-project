#include "document.h"
#include <iostream>
using namespace std;
Document::Document()
{
    chapters.clear();
    curChap = 0;
}
void Document::add_chap(const chapter& chap)
{
    chapters.push_back(chap);
    return;
}
void Document::rmv_chap(int id)
{
    if (id >= 0 && id < chapters.size())
    {
        chapters.erase(chapters.begin() + id);
    }
}
chapter& Document::get_chap(int idx)
{
    return chapters[idx];
}
const chapter& Document::get_chap(int idx) const
{
    return chapters[idx];
}
int Document::chap_ct() const
{
    int si = chapters.size();
    return si;
}
void Document::print() const
{
    for (int qt = 0; qt < chapters.size(); qt++)
    {
        chapters[qt].print();
        cout << endl;
    }
}
bool Document::move_left()
{
    if (chapters.empty())
    {
        return false;
    }
    return chapters[curChap].move_left();
}
bool Document::move_right()
{
    if (chapters.empty())
    {
        return false;
    }
    return chapters[curChap].move_right();
}
bool Document::move_down()
{
    if (chapters.empty())
    {
        return false;
    }
    if (chapters[curChap].move_down())
    {
        return true;
    }
    if (curChap + 1 < chapters.size())
    {
        curChap++;
        chapters[curChap].set_cur_idx(0);
        return true;
    }
    return false;
}
void Document::new_chap()
{
    chapter c;
    section s;
    Paragraph p;
    Line l;
    p.apndLine(l);
    s.add_para(p);
    c.add_sec(s);
    chapters.push_back(c);
    curChap = chapters.size() - 1;
}
bool Document::move_up()
{
    if (chapters.empty())
    {
        return false;
    }
    if (chapters[curChap].move_up())
    {
        return true;
    }
    if (curChap > 0)
    {
        curChap--;
        int lst = chapters[curChap].sec_ct() - 1;
        chapters[curChap].set_cur_idx(lst);
        return true;
    }
    return false;
}
void Document::move_start()
{
    if (!chapters.empty())
    {
        chapters[curChap].move_start();
    }
}
void Document::move_end()
{
    if (!chapters.empty())
    {
        chapters[curChap].move_end();
    }
}
bool Document::word_frwd()
{
    if (chapters.empty())
    {
        return false;
    }
    if (chapters[curChap].word_frwd())
    {
        return true;
    }
    return move_down();
}
bool Document::word_backwrd()
{
    if (chapters.empty())
    {
        return false;
    }
    if (chapters[curChap].word_backwrd())
    {
        return true;
    }
    return move_up();
}
void Document::go_top()
{
    curChap = 0;
    if (!chapters.empty())
    {
        chapters[curChap].set_cur_idx(0);
    }
}
void Document::go_bottom()
{
    curChap = chapters.size() - 1;
    int lst = chapters[curChap].sec_ct() - 1;
    chapters[curChap].set_cur_idx(lst);
}
chapter& Document::get_cur_chap()
{
    return chapters[curChap];
}
int Document::get_cur_idx() const
{
    return curChap;
}
void Document::set_cur_idx(int idx)
{
    curChap = idx;
}
Line& Document::get_cur_line()
{
    return chapters[curChap].get_cur_sec().get_cur_para().get_cur_line();
}
bool Document::del_cur_line(Line& out)
{
    if (chapters.empty())
    {
        return false;
    }
    return chapters[curChap].del_cur_line(out);
}
Line Document::copy_cur_line()
{
    return chapters[curChap].copy_cur_line();
}
void Document::add_after(const Line& ln)
{
    chapters[curChap].add_after(ln);
}
void Document::add_before(const Line& ln)
{
    chapters[curChap].add_before(ln);
}