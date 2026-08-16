#include"Section.h"
section::section()
{
    paragraphs.clear();
    curPara = 0;
}
void section::add_para(const Paragraph& para)
{
    paragraphs.push_back(para);
    return;
}
void section::rmv_para(int id)
{
    if (id >= 0 && id < paragraphs.size())
    {
        paragraphs.erase(paragraphs.begin() + id);
    }
}
Paragraph& section::get_para(int idx)
{
    return paragraphs[idx];
}
const Paragraph& section::getParagraph(int idx) const
{
    return paragraphs[idx];
}
int section::para_ct() const
{
    int si = paragraphs.size();
    return si;
}
void section::print() const
{
    for (int qt = 0; qt < paragraphs.size(); qt++)
    {
        paragraphs[qt].print();
        cout << endl;
    }
}
bool section::move_left()
{
    if (paragraphs.empty())
    {
        return false;
    }
    return paragraphs[curPara].move_left();
}
bool section::move_right()
{
    if (paragraphs.empty())
    {
        return false;
    }
    return paragraphs[curPara].move_right();
}
bool section::move_down()
{
    if (paragraphs.empty())
    {
        return false;
    }
    if (paragraphs[curPara].move_down())
    {
        return true;
    }
    if (curPara + 1 < paragraphs.size())
    {
        curPara++;
        paragraphs[curPara].set_cur_idx(0);
        return true;
    }
    return false;
}
bool section::move_up()
{
    if (paragraphs.empty())
    {
        return false;
    }
    if (paragraphs[curPara].move_up())
    {
        return true;
    }
    if (curPara > 0)
    {
        curPara--;
        int lst = paragraphs[curPara].get_line_ct() - 1;
        paragraphs[curPara].set_cur_idx(lst);
        return true;
    }
    return false;
}
void section::move_start()
{
    if (!paragraphs.empty())
    {
        paragraphs[curPara].move_start();
    }
}
void section::move_end()
{
    if (!paragraphs.empty())
    {
        paragraphs[curPara].move_end();
    }
}
bool section::word_frwd()
{
    if (paragraphs.empty())
    {
        return false;
    }
    if (paragraphs[curPara].word_frwd())
    {
        return true;
    }
    return move_down();
}
bool section::word_backwrd()
{
    if (paragraphs.empty())
    {
        return false;
    }
    if (paragraphs[curPara].word_backwrd())
    {
        return true;
    }
    return move_up();
}
Paragraph& section::get_cur_para()
{
    return paragraphs[curPara];
}
int section::get_cur_idx() const
{
    return curPara;
}
void section::set_cur_idx(int idx)
{
    curPara = idx;
}
bool section::del_cur_line(Line& out)
{
    if (paragraphs.empty())
    {
        return false;
    }
    return paragraphs[curPara].del_cur_line(out);
}
Line section::copy_cur_line()
{
    return paragraphs[curPara].copy_cur_line();
}
void section::add_after(const Line& ln)
{
    paragraphs[curPara].add_after(ln);
}
void section::add_before(const Line& ln)
{
    paragraphs[curPara].add_before(ln);
}
void section::new_para()
{
    Paragraph p;
    Line l;
    p.apndLine(l);
    paragraphs.push_back(p);
    curPara = paragraphs.size() - 1;
}