#include"Line.h"
Line::Line()
{
    this->text.clear();
    this->curCol = 0;
}
Line::Line(const vector<char>& txt)
{
    this->text = txt;
    this->curCol = 0;
}
void Line::insert_ch(int col, char ch)
{
    text.insert(text.begin() + col, ch);
}
void Line::dlt_chr(int col)
{
    text.erase(text.begin() + col);
}
void Line::del_frm(int col)
{
    text.erase(text.begin() + col, text.end());
}
bool Line::isupr(char ch) const
{
    return (ch >= 'A' && ch <= 'Z');
}

bool Line::islower(char ch) const
{
    return (ch >= 'a' && ch <= 'z');
}

char Line::tolwr(char ch) const
{
    if (isupr(ch))
    {
        return ch + 32;
    }
    return ch;
}

char Line::toupper(char ch) const
{
    if (islower(ch))
    {
        return ch - 32;
    }
    return ch;
}
void Line::toggl(int col)
{
    if (col >= 0 && col < text.size())
    {
        if (isupr(text[col]))
        {
            text[col] = tolwr(text[col]);
        }
        else if (islower(text[col]))
        {
            text[col] = toupper(text[col]);
        }
    }
}
void Line::indent()
{
    text.insert(text.begin(), 4, ' ');
}
void Line::unindent()
{
    for (int qt = 0; qt < 4; qt++)
    {
        if (!text.empty() && text[0] == ' ')
        {
            text.erase(text.begin());
        }
        else
        {
            break;
        }
    }
}
int Line::get_len() const
{
    return text.size();
}
vector<char> Line::get_txt() const
{
    return text;
}
void Line::settext(const vector<char>& nt)
{
    text = nt;
}
void Line::print() const
{
    for (int se = 0; se < text.size(); se++)
    {
        cout << text[se];
    }
}
bool Line::is_emp() const
{
    return text.size() == 0;
}
bool Line::move_left()
{
    if (curCol > 0)
    {
        curCol--;
        return true;
    }
    return false;
}
bool Line::move_right()
{
    if (curCol < (int)text.size())   
    {
        curCol++;
        return true;
    }
    return false;
}
void Line::moveToStart()
{
    curCol = 0;
}
void Line::moveToEnd()
{
    curCol = text.size();
    if (curCol > 0)
    {
        curCol--;
    }
}
int Line::get_cur_col() const
{
    return curCol;
}
void Line::set_cur_col(int col)
{
    curCol = col;
}
void Line::insert_at_cur(char ch)
{
    insert_ch(curCol, ch);
    curCol++;
}
void Line::delete_at_Cur()
{
    if (curCol >= 0 && curCol < text.size())
    {
        dlt_chr(curCol);
        if (curCol >= text.size() && curCol > 0)
        {
            curCol--;
        }
    }
}
void Line::deleteFromCursor()
{
    del_frm(curCol);
}
void Line::toggle_cursor()
{
    toggl(curCol);
    if (curCol + 1 < text.size())
    {
        curCol++;
    }
}
bool Line::word_frwd()
{
    int zi = text.size();
    while (curCol < zi && text[curCol] != ' ')
    {
        curCol++;
    }
    while (curCol < zi && text[curCol] == ' ')
    {
        curCol++;
    }
    if (curCol >= zi)
    {
        return false;
    }
    return true;
}
void Line::back_sp()
{
    if (curCol > 0)
    {
        curCol--;
        dlt_chr(curCol);
    }
}
bool Line::word_backwrd()
{
    if (curCol <= 0)
    {
        return false;
    }
    curCol--;
    while (curCol > 0 && text[curCol] == ' ')
    {
        curCol--;
    }
    while (curCol > 0 && text[curCol - 1] != ' ')
    {
        curCol--;
    }
    return true;
}