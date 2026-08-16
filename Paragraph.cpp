#include"Paragraph.h"
Paragraph::Paragraph()
{
    lines.clear();
    curLine = 0;
}
int Paragraph::get_line_ct() const
{
    int res = 0;
    res = lines.size();
    return res;
}
vector<Line> Paragraph::get_line() const
{
    return lines;
}
void Paragraph::setLines(const vector<Line>& neline)
{
    lines = neline;
}
Line& Paragraph::line_rf(int index)
{
    if (index >= 0 && index < lines.size())
    {
        return lines[index];
    }
    else
    {
        return lines[0];
    }
}
const Line& Paragraph::getLine(int index) const
{
    return lines[index];
}
void Paragraph::insrtLine(int index, const Line& line)
{
    if (index >= 0 && index <= lines.size())
    {
        lines.insert(lines.begin() + index, line);
    }
    return;
}
void Paragraph::apndLine(const Line& li)
{
    lines.insert(lines.end(), li);
    return;
}
void Paragraph::del_line(int id)
{
    if (id >= 0 && id < lines.size())
    {
        lines.erase(lines.begin() + id);
    }
    return;
}
void Paragraph::splitLine(int lid, int col)
{
    if (lid < 0 || lid >= lines.size())
    {
        return;
    }
    Line& cl = lines[lid];
    Line ne;
    vector<char> txt = cl.get_txt();
    for (int a = col; a < cl.get_len(); a++)
    {
        ne.insert_ch(ne.get_len(), txt[a]);
    }
    cl.del_frm(col);
    lines.insert(lines.begin() + lid + 1, ne);
}
void Paragraph::join_line(int id)
{
    if (id <= 0 || id >= lines.size())
    {
        return;
    }
    Line& temp = lines[id - 1];
    vector<char> txt = lines[id].get_txt();
    for (int e = 0; e < lines[id].get_len(); e++)
    {
        temp.insert_ch(temp.get_len(), txt[e]);
    }
    lines.erase(lines.begin() + id);
}
bool Paragraph::emty() const
{
    return lines.size() == 0;
}
void Paragraph::print() const
{
    for (int i = 0; i < lines.size(); i++)
    {
        lines[i].print();
        cout << endl;
    }
}
bool Paragraph::move_left()
{
    if (lines.empty())
    {
        return false;
    }
    return lines[curLine].move_left();
}
bool Paragraph::move_right()
{
    if (lines.empty())
    {
        return false;
    }
    return lines[curLine].move_right();
}
bool Paragraph::move_down()
{
    if (curLine + 1 < lines.size())
    {
        curLine++;
        return true;
    }
    return false;
}
bool Paragraph::move_up()
{
    if (curLine > 0)
    {
        curLine--;
        return true;
    }
    return false;
}
void Paragraph::move_start()
{
    if (!lines.empty())
    {
        lines[curLine].moveToStart();
    }
}
void Paragraph::move_end()
{
    if (!lines.empty())
    {
        lines[curLine].moveToEnd();
    }
}
bool Paragraph::word_frwd()
{
    if (lines.empty())
    {
        return false;
    }
    if (lines[curLine].word_frwd())
    {
        return true;
    }
    return move_down();
}
bool Paragraph::word_backwrd()
{
    if (lines.empty())
    {
        return false;
    }
    if (lines[curLine].word_backwrd())
    {
        return true;
    }
    return move_up();
}
Line& Paragraph::get_cur_line()
{
    return lines[curLine];
}
int Paragraph::get_cur_idx() const
{
    return curLine;
}
void Paragraph::set_cur_idx(int idx)
{
    curLine = idx;
}
bool Paragraph::del_cur_line(Line& out)
{
    if (lines.size() > 1)
    {
        out = lines[curLine];
        lines.erase(lines.begin() + curLine);
        if (curLine >= lines.size())
        {
            curLine = lines.size() - 1;
        }
        return true;
    }
    lines[curLine].settext(vector<char>());
    return false;
}
Line Paragraph::copy_cur_line()
{
    return lines[curLine];
}
void Paragraph::add_after(const Line& ln)
{
    lines.insert(lines.begin() + curLine + 1, ln);
    curLine++;
}
void Paragraph::add_before(const Line& ln)
{
    lines.insert(lines.begin() + curLine, ln);
}