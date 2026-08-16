#include "chapter.h"
#include <iostream>
using namespace std;
chapter::chapter()
{
    sections.clear();
    curSec = 0;
}
void chapter::add_sec(const section& sec)
{
    sections.push_back(sec);
    return;
}
void chapter::rmv_sec(int id)
{
    if (id >= 0 && id < sections.size())
    {
        sections.erase(sections.begin() + id);
    }
}
section& chapter::get_sec(int idx)
{
    return sections[idx];
}
const section& chapter::get_sec(int idx) const
{
    return sections[idx];
}
int chapter::sec_ct() const
{
    int si = sections.size();
    return si;
}
void chapter::print() const
{
    for (int qt = 0; qt < sections.size(); qt++)
    {
        sections[qt].print();
        cout << endl;
    }
}
bool chapter::move_left()
{
    if (sections.empty())
    {
        return false;
    }
    return sections[curSec].move_left();
}
bool chapter::move_right()
{
    if (sections.empty())
    {
        return false;
    }
    return sections[curSec].move_right();
}
void chapter::new_sec()
{
    section s;
    Paragraph p;
    Line l;
    p.apndLine(l);
    s.add_para(p);
    sections.push_back(s);
    curSec = sections.size() - 1;
}
bool chapter::move_down()
{
    if (sections.empty())
    {
        return false;
    }
    if (sections[curSec].move_down())
    {
        return true;
    }
    if (curSec + 1 < sections.size())
    {
        curSec++;
        sections[curSec].set_cur_idx(0);
        return true;
    }
    return false;
}
bool chapter::move_up()
{
    if (sections.empty())
    {
        return false;
    }
    if (sections[curSec].move_up())
    {
        return true;
    }
    if (curSec > 0)
    {
        curSec--;
        int lst = sections[curSec].para_ct() - 1;
        sections[curSec].set_cur_idx(lst);
        return true;
    }
    return false;
}
void chapter::move_start()
{
    if (!sections.empty())
    {
        sections[curSec].move_start();
    }
}
void chapter::move_end()
{
    if (!sections.empty())
    {
        sections[curSec].move_end();
    }
}
bool chapter::word_frwd()
{
    if (sections.empty())
    {
        return false;
    }
    if (sections[curSec].word_frwd())
    {
        return true;
    }
    return move_down();
}
bool chapter::word_backwrd()
{
    if (sections.empty())
    {
        return false;
    }
    if (sections[curSec].word_backwrd())
    {
        return true;
    }
    return move_up();
}
section& chapter::get_cur_sec()
{
    return sections[curSec];
}
int chapter::get_cur_idx() const
{
    return curSec;
}
void chapter::set_cur_idx(int idx)
{
    curSec = idx;
}
bool chapter::del_cur_line(Line& out)
{
    if (sections.empty())
    {
        return false;
    }
    return sections[curSec].del_cur_line(out);
}
Line chapter::copy_cur_line()
{
    return sections[curSec].copy_cur_line();
}
void chapter::add_after(const Line& ln)
{
    sections[curSec].add_after(ln);
}
void chapter::add_before(const Line& ln)
{
    sections[curSec].add_before(ln);
}