#define _WIN32_WINNT 0x0600
#include<iostream>
#include<windows.h>
#include<conio.h>
#include<fstream>
#include<cstdlib>
#include"Document.h"
using namespace std;
Document docc;
void clearScreen(WORD attr)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 0, 0 };
	DWORD count;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hOut, &csbi);
	int cells = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hOut, ' ', cells, coord, &count);
	FillConsoleOutputAttribute(hOut, attr, cells, coord, &count);
	SetConsoleCursorPosition(hOut, coord);
}
void setColor(WORD color)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, color);
}
void gotoRowCol(int rpos, int cpos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = cpos;
	scrn.Y = rpos;
	SetConsoleCursorPosition(hOuput, scrn);
}
void paintBand(int row, WORD bandColor)
{
	gotoRowCol(row, 0);
	setColor(bandColor);
	for (int bp = 0; bp <= 99; bp++)
	{
		cout << ' ';
	}
}
void SetConsoleSettings()
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleTitleA("OOPX Project 02 Text Editor");
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;
	cfi.dwFontSize.Y = 18;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	const wchar_t* face = L"Consolas";
	int wi = 0;
	while (face[wi] != L'\0')
	{
		cfi.FaceName[wi] = face[wi];
		wi++;
	}
	cfi.FaceName[wi] = L'\0';
	SetCurrentConsoleFontEx(hOut, false, &cfi);
	COORD bufSz;
	bufSz.X = 100;
	bufSz.Y = 35;
	SetConsoleScreenBufferSize(hOut, bufSz);
	SMALL_RECT winSz;
	winSz.Left = 0;
	winSz.Top = 0;
	winSz.Right = 99;
	winSz.Bottom = 33;
	SetConsoleWindowInfo(hOut, true, &winSz);
	DWORD dwMode = 0;
	GetConsoleMode(hIn, &dwMode);
	dwMode &= ~ENABLE_PROCESSED_INPUT;
	SetConsoleMode(hIn, dwMode);

	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 25;
	cci.bVisible = false;
	SetConsoleCursorInfo(hOut, &cci);
}

// ============================================================
// DOUBLE BUFFERING (isi se flicker khatam hota hai)
// ------------------------------------------------------------
// Idea: screen pe seedha cout se likhne ki bajaye, hum pehle
// pura frame ek INVISIBLE buffer (jo user ko nazar nahi aata)
// mein taiyar karte hain, character-by-character. Jab pura
// frame ready ho jata hai, tab EK HI call (WriteConsoleOutputA)
// se poora blit karte hain aur SetConsoleActiveScreenBuffer se
// turant swap kar dete hain. User ko kabhi "aadha bana hua"
// frame nazar nahi aata, isliye flicker zero ho jata hai.
// ============================================================
const int BUF_W = 100;
const int BUF_H = 35;
CHAR_INFO frameBuf[BUF_H][BUF_W];
HANDLE hBufA;
HANDLE hBufB;
bool activeIsA = true;

void setupBuffers()
{
	hBufA = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	hBufB = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	HANDLE bufs[2];
	bufs[0] = hBufA;
	bufs[1] = hBufB;

	for (int i = 0; i < 2; i++)
	{
		HANDLE hb = bufs[i];

		COORD bufSz;
		bufSz.X = BUF_W;
		bufSz.Y = BUF_H;
		SetConsoleScreenBufferSize(hb, bufSz);

		SMALL_RECT winSz;
		winSz.Left = 0;
		winSz.Top = 0;
		winSz.Right = BUF_W - 1;
		winSz.Bottom = 33;
		SetConsoleWindowInfo(hb, true, &winSz);

		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = 0;
		cfi.dwFontSize.Y = 18;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		const wchar_t* face = L"Consolas";
		int wi = 0;
		while (face[wi] != L'\0')
		{
			cfi.FaceName[wi] = face[wi];
			wi++;
		}
		cfi.FaceName[wi] = L'\0';
		SetCurrentConsoleFontEx(hb, false, &cfi);

		CONSOLE_CURSOR_INFO cci;
		cci.dwSize = 25;
		cci.bVisible = false;
		SetConsoleCursorInfo(hb, &cci);
	}

	SetConsoleActiveScreenBuffer(hBufA);
	activeIsA = true;
}

void fb_clear(WORD attr)
{
	for (int r = 0; r < BUF_H; r++)
	{
		for (int c = 0; c < BUF_W; c++)
		{
			frameBuf[r][c].Char.AsciiChar = ' ';
			frameBuf[r][c].Attributes = attr;
		}
	}
}

void fb_putc(int row, int col, char ch, WORD attr)
{
	if (row < 0 || row >= BUF_H || col < 0 || col >= BUF_W)
	{
		return;
	}
	frameBuf[row][col].Char.AsciiChar = ch;
	frameBuf[row][col].Attributes = attr;
}

void fb_hline(int row, int colStart, int count, char ch, WORD attr)
{
	for (int i = 0; i < count; i++)
	{
		fb_putc(row, colStart + i, ch, attr);
	}
}

int fb_print_str(int row, int col, const char* s, WORD attr)
{
	int c = col;
	while (*s != '\0')
	{
		fb_putc(row, c, *s, attr);
		c++;
		s++;
	}
	return c;
}

void fb_print_int(int row, int col, int val, WORD attr)
{
	char buf[12];
	int n = 0;
	if (val == 0)
	{
		buf[n] = '0';
		n++;
	}
	else
	{
		int v = val;
		while (v > 0)
		{
			buf[n] = '0' + (v % 10);
			n++;
			v = v / 10;
		}
	}
	for (int i = n - 1; i >= 0; i--)
	{
		fb_putc(row, col, buf[i], attr);
		col++;
	}
}

void fb_present(int cursorRow, int cursorCol)
{
	HANDLE hDraw = activeIsA ? hBufB : hBufA;

	SMALL_RECT region;
	region.Left = 0;
	region.Top = 0;
	region.Right = BUF_W - 1;
	region.Bottom = BUF_H - 1;

	COORD bufSize;
	bufSize.X = BUF_W;
	bufSize.Y = BUF_H;

	COORD bufCoord;
	bufCoord.X = 0;
	bufCoord.Y = 0;

	WriteConsoleOutputA(hDraw, &frameBuf[0][0], bufSize, bufCoord, &region);

	COORD curPos;
	curPos.X = cursorCol;
	curPos.Y = cursorRow;
	SetConsoleCursorPosition(hDraw, curPos);

	SetConsoleActiveScreenBuffer(hDraw);
	activeIsA = !activeIsA;
}

vector<Document> undo_stk;
vector<Document> redo_stk;
vector<char> cur_fname;
vector<char> srch_pat;
bool srch_fwd = true;
bool num_mode = false;
char cmnd_type = ':';
void push_undo()
{
	undo_stk.push_back(docc);
	if (undo_stk.size() > 200)
	{
		undo_stk.erase(undo_stk.begin());
	}
	redo_stk.clear();
}
bool vec_eq(vector<char>& store, const char* s)
{
	int n = 0;
	while (s[n] != '\0')
	{
		n++;
	}
	if (store.size() != n)
	{
		return false;
	}
	for (int i = 0; i < n; i++)
	{
		if (store[i] != s[i])
		{
			return false;
		}
	}
	return true;
}
bool find_pattern(vector<char>& tx, vector<char>& ptrn, int& posi)
{
	int ne = tx.size();
	int mr = ptrn.size();
	if (mr == 0)
	{
		return false;
	}
	for (int ae = 0; ae + mr <= ne; ae++)
	{
		bool rgt = true;
		for (int re = 0; re < mr; re++)
		{
			if (tx[ae + re] != ptrn[re])
			{
				rgt = false;
				break;
			}
		}
		if (rgt)
		{
			posi = ae;
			return true;
		}
	}
	return false;
}
struct posi
{
	int chap;
	int sec;
	int para;
	int line;
};
bool is_before(const posi& p1, const posi& p2)
{
	if (p1.chap != p2.chap)
	{
		return p1.chap < p2.chap;
	}
	if (p1.sec != p2.sec)
	{
		return p1.sec < p2.sec;
	}
	if (p1.para != p2.para)
	{
		return p1.para < p2.para;
	}
	return p1.line < p2.line;
}
bool find_and_jump(Document& doc, vector<char>& ptrn, bool forwd)
{
	if (ptrn.empty())
	{
		return false;
	}
	posi cur_pos;
	cur_pos.chap = doc.get_cur_idx();
	cur_pos.sec = doc.get_cur_chap().get_cur_idx();
	cur_pos.para = doc.get_cur_chap().get_cur_sec().get_cur_idx();
	cur_pos.line = doc.get_cur_chap().get_cur_sec().get_cur_para().get_cur_idx();
	vector<posi> matches;
	for (int cid = 0; cid < doc.chap_ct(); cid++)
	{
		chapter& ch = doc.get_chap(cid);
		for (int sid = 0; sid < ch.sec_ct(); sid++)
		{
			section& sc = ch.get_sec(sid);
			for (int pid = 0; pid < sc.para_ct(); pid++)
			{
				Paragraph& pr = sc.get_para(pid);
				for (int lid = 0; lid < pr.get_line_ct(); lid++)
				{
					vector<char> txt = pr.getLine(lid).get_txt();
					int match_col = 0;
					if (find_pattern(txt, ptrn, match_col))
					{
						posi pos;
						pos.chap = cid;
						pos.sec = sid;
						pos.para = pid;
						pos.line = lid;
						matches.push_back(pos);
					}
				}
			}
		}
	}
	if (matches.empty())
	{
		return false;
	}
	posi target_pos;
	bool found_target = false;
	if (forwd)
	{
		for (int i = 0; i < matches.size(); i++)
		{
			if (is_before(cur_pos, matches[i]))
			{
				target_pos = matches[i];
				found_target = true;
				break;
			}
		}
		if (!found_target)
		{
			target_pos = matches[0];
		}
	}
	else
	{
		for (int i = matches.size() - 1; i >= 0; i--)
		{
			if (is_before(matches[i], cur_pos))
			{
				target_pos = matches[i];
				found_target = true;
				break;
			}
		}
		if (!found_target)
		{
			target_pos = matches.back();
		}
	}
	doc.set_cur_idx(target_pos.chap);
	doc.get_cur_chap().set_cur_idx(target_pos.sec);
	doc.get_cur_chap().get_cur_sec().set_cur_idx(target_pos.para);
	doc.get_cur_chap().get_cur_sec().get_cur_para().set_cur_idx(target_pos.line);
	doc.get_cur_line().moveToStart();
	return true;
}
vector<char> replace_in_line(vector<char> txt, vector<char> pat, vector<char> rep)
{
	int pat_len = pat.size();
	int rep_len = rep.size();
	if (pat_len == 0)
	{
		return txt;
	}
	int cur_pos = 0;
	while (cur_pos + pat_len <= txt.size())
	{
		bool matched = true;
		for (int char_idx = 0; char_idx < pat_len; char_idx++)
		{
			if (txt[cur_pos + char_idx] != pat[char_idx])
			{
				matched = false;
				break;
			}
		}
		if (matched)
		{
			txt.erase(txt.begin() + cur_pos, txt.begin() + cur_pos + pat_len);
			txt.insert(txt.begin() + cur_pos, rep.begin(), rep.end());
			cur_pos += rep_len;
		}
		else
		{
			cur_pos++;
		}
	}

	return txt;
}
void replace_all(Document& d, vector<char>& oldPat, vector<char>& newPat)
{
	if (oldPat.empty())
	{
		return;
	}
	for (int re = 0; re < d.chap_ct(); re++)
	{
		chapter& ch = d.get_chap(re);
		for (int ct = 0; ct < ch.sec_ct(); ct++)
		{
			section& sc = ch.get_sec(ct);
			for (int ee = 0; ee < sc.para_ct(); ee++)
			{
				Paragraph& pr = sc.get_para(ee);
				for (int ae = 0; ae < pr.get_line_ct(); ae++)
				{
					Line& ln = pr.line_rf(ae);
					vector<char> nt = replace_in_line(ln.get_txt(), oldPat, newPat);
					ln.settext(nt);
				}
			}
		}
	}
}
void save_file(Document& d, vector<char>& fname)
{
	char buf[200];
	int i = 0;
	while (i < fname.size() && i < 199)
	{
		buf[i] = fname[i];
		i++;
	}
	buf[i] = '\0';
	ofstream fout(buf);
	for (int a = 0; a < d.chap_ct(); a++)
	{
		chapter& ch2 = d.get_chap(a);
		for (int b = 0; b < ch2.sec_ct(); b++)
		{
			section& sc = ch2.get_sec(b);
			for (int e = 0; e < sc.para_ct(); e++)
			{
				Paragraph& pr = sc.get_para(e);
				for (int f = 0; f < pr.get_line_ct(); f++)
				{
					vector<char> txt = pr.getLine(f).get_txt();
					for (int g = 0; g < txt.size(); g++)
					{
						fout << txt[g];
					}
					fout << "\n";
				}
			}
		}
	}
	fout.close();
}
int main()
{
	SetConsoleSettings();

	WORD white_bg = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
	WORD black_txt = white_bg;
	WORD blue_txt = white_bg | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

	WORD title_bg = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
	WORD title_txt = title_bg | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

	WORD help_bg = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
	WORD help_txt = help_bg;

	WORD cursor_hi = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;

	// splash screen -- yeh abhi bhi original (single-buffer) tareeqe se hi
	// dikhta hai, kyuke yeh sirf ek dafa chalta hai, flicker ka masla
	// yahan hai hi nahi. Iske baad hum double-buffering switch on karte hain.
	clearScreen(white_bg);
	setColor(black_txt);
	gotoRowCol(14, 24);
	cout << "================================================";
	gotoRowCol(15, 24);
	cout << "      WELCOME TO TEXT EDITOR MADE BY JAMAL         ";
	gotoRowCol(16, 24);
	cout << "================================================";
	Sleep(1500);

	// ab se har frame double-buffered off-screen banega, taake
	// koi flicker na ho, chahe har letter type ho ya kuch aur
	setupBuffers();

	char defname[9] = "save.txt";
	for (int i = 0; i < 8; i++)
	{
		cur_fname.push_back(defname[i]);
	}
	chapter c1;
	section s1;
	Paragraph p1;
	Line l1;
	p1.apndLine(l1);
	s1.add_para(p1);
	c1.add_sec(s1);
	docc.add_chap(c1);
	char mode = 'N';
	Line copy_li;
	bool has_cp = false;
	bool sel_on = false;
	int sel_col = 0;
	vector<char> clip_txt;
	bool has_clip = false;
	char pend_ch = 0;
	int enter_ct = 0;
	char last_cmd = 0;
	vector<char> cmnd_ch;
	bool going = true;
	while (going == true)
	{
		// ============================================================
		// RENDER: sab kuch pehle invisible frameBuf mein banaya jata
		// hai, screen pe seedha nahi likha jata. Isi wajah se flicker
		// nahi hota -- user ko sirf poora tayyar frame hi dikhta hai.
		// ============================================================
		fb_clear(white_bg);

		fb_hline(0, 0, BUF_W, '=', title_txt);

		fb_hline(1, 0, BUF_W, ' ', title_bg);
		if (mode == 'N')
		{
			fb_print_str(1, 2, "NORMAL MODE", title_txt);
		}
		else if (mode == 'I')
		{
			fb_print_str(1, 2, "INSERT MODE", title_txt);
		}
		else if (mode == 'C')
		{
			int tcol = fb_print_str(1, 2, "COMMAND MODE  ", title_txt);
			fb_putc(1, tcol, cmnd_type, title_txt);
			tcol++;
			for (int q = 0; q < (int)cmnd_ch.size(); q++)
			{
				fb_putc(1, tcol, cmnd_ch[q], title_txt);
				tcol++;
			}
		}

		fb_hline(2, 0, BUF_W, '=', title_txt);

		int sr = 0;
		int cr = 3;
		int ccl = 2;

		for (int a = 0; a < docc.chap_ct(); a++)
		{
			chapter& ch = docc.get_chap(a);
			for (int b = 0; b < ch.sec_ct(); b++)
			{
				section& sc = docc.get_chap(a).get_sec(b);
				for (int e = 0; e < sc.para_ct(); e++)
				{
					Paragraph& pr = sc.get_para(e);
					for (int f = 0; f < pr.get_line_ct(); f++)
					{
						Line& ln = pr.line_rf(f);

						bool is_curr = (a == docc.get_cur_idx() && b == ch.get_cur_idx() && e == sc.get_cur_idx() && f == pr.get_cur_idx());

						int row = 3 + sr;
						int textStartCol = 2;

						if (num_mode)
						{
							fb_print_int(row, 2, sr + 1, black_txt);
							textStartCol = 10;
						}

						WORD lineColor = is_curr ? blue_txt : black_txt;

						vector<char> txt = ln.get_txt();
						int col = textStartCol;
						for (int ti = 0; ti < (int)txt.size(); ti++)
						{
							fb_putc(row, col, txt[ti], lineColor);
							col++;
						}

						if (is_curr)
						{
							cr = row;
							ccl = textStartCol + ln.get_cur_col();
						}

						sr++;
					}
				}
			}
		}

		fb_hline(28, 0, BUF_W, '=', help_txt);

		fb_hline(29, 0, BUF_W, ' ', help_bg);
		fb_hline(30, 0, BUF_W, ' ', help_bg);

		if (mode == 'N')
		{
			fb_print_str(29, 2, "h j k l:move w b:word 0 $:line gg G:file dd:delline x:char D:toend", help_txt);
			fb_print_str(30, 2, "~:case yy:copy p P:paste u:undo Ctrl+R:redo .:repeat i:insert ::cmd", help_txt);
		}
		else if (mode == 'I')
		{
			fb_print_str(29, 2, "Esc:normal Enter:newline Backspace Delete Tab:indent Ctrl+T:unindent", help_txt);
			fb_print_str(30, 2, "Ctrl+C/X/V:copy cut paste Ctrl+Left/Right:select text", help_txt);
		}
		else if (mode == 'C')
		{
			fb_print_str(29, 2, ":w save :q quit :wq :q! :nc :ns :np new chap/sec/para", help_txt);
			fb_print_str(30, 2, "/pat ?pat search :n :N next/prev %s/old/new/g replace set number", help_txt);
		}

		fb_hline(31, 0, BUF_W, '=', help_txt);

		Line& curln_for_cursor = docc.get_cur_line();
		int realCol = curln_for_cursor.get_cur_col();
		vector<char> curTxt = curln_for_cursor.get_txt();
		char curChar = ' ';
		if (realCol < (int)curTxt.size())
		{
			curChar = curTxt[realCol];
		}
		fb_putc(cr, ccl, curChar, cursor_hi);

		fb_present(cr, ccl);

		char ch = _getch();
		if (mode == 'N')
		{
			if (pend_ch == 'g' && ch == 'g')
			{
				docc.go_top();
				pend_ch = 0;
			}
			else if (pend_ch == 'd' && ch == 'd')
			{
				push_undo();
				Line tmp;
				if (docc.del_cur_line(tmp))
				{
					copy_li = tmp;
					has_cp = true;
				}
				last_cmd = 'Z';
				pend_ch = 0;
			}
			else if (pend_ch == 'y' && ch == 'y')
			{
				copy_li = docc.copy_cur_line();
				has_cp = true;
				pend_ch = 0;
			}
			else if (ch == 'g')
			{
				pend_ch = 'g';
			}
			else if (ch == 'd')
			{
				pend_ch = 'd';
			}
			else if (ch == 'y')
			{
				pend_ch = 'y';
			}
			else
			{
				pend_ch = 0;

				if (ch == 'h')
				{
					docc.move_left();
				}
				else if (ch == 'l')
				{
					docc.move_right();
				}
				else if (ch == 'j')
				{
					docc.move_down();
				}
				else if (ch == 'k')
				{
					docc.move_up();
				}
				else if (ch == 'w')
				{
					docc.word_frwd();
				}
				else if (ch == 'b')
				{
					docc.word_backwrd();
				}
				else if (ch == '0')
				{
					docc.move_start();
				}
				else if (ch == '$')
				{
					docc.move_end();
				}
				else if (ch == 'G')
				{
					docc.go_bottom();
				}
				else if (ch == 4)
				{
					for (int t = 0; t < 10; t++)
					{
						docc.move_down();
					}
				}
				else if (ch == 21)
				{
					for (int t = 0; t < 10; t++)
					{
						docc.move_up();
					}
				}
				else if (ch == 'x')
				{
					push_undo();
					docc.get_cur_line().delete_at_Cur();
					last_cmd = 'x';
				}
				else if (ch == 'D')
				{
					push_undo();
					docc.get_cur_line().deleteFromCursor();
					last_cmd = 'D';
				}
				else if (ch == '~')
				{
					push_undo();
					docc.get_cur_line().toggle_cursor();
					last_cmd = '~';
				}
				else if (ch == 'p')
				{
					if (has_cp)
					{
						push_undo();
						docc.add_after(copy_li);
					}
				}
				else if (ch == 'P')
				{
					if (has_cp)
					{
						push_undo();
						docc.add_before(copy_li);
					}
				}
				else if (ch == 'u')
				{
					if (!undo_stk.empty())
					{
						redo_stk.push_back(docc);
						docc = undo_stk.back();
						undo_stk.pop_back();
					}
				}
				else if (ch == 18)
				{
					if (!redo_stk.empty())
					{
						undo_stk.push_back(docc);
						docc = redo_stk.back();
						redo_stk.pop_back();
					}
				}
				else if (ch == '.')
				{
					if (last_cmd == 'x')
					{
						push_undo();
						docc.get_cur_line().delete_at_Cur();
					}
					else if (last_cmd == 'D')
					{
						push_undo();
						docc.get_cur_line().deleteFromCursor();
					}
					else if (last_cmd == '~')
					{
						push_undo();
						docc.get_cur_line().toggle_cursor();
					}
					else if (last_cmd == 'Z')
					{
						push_undo();
						Line tmp2;
						if (docc.del_cur_line(tmp2))
						{
							copy_li = tmp2;
							has_cp = true;
						}
					}
				}
				else if (ch == 'i')
				{
					mode = 'I';
				}
				else if (ch == ':')
				{
					mode = 'C';
					cmnd_ch.clear();
					cmnd_type = ':';
				}
				else if (ch == '/')
				{
					mode = 'C';
					cmnd_ch.clear();
					cmnd_type = '/';
				}
				else if (ch == '?')
				{
					mode = 'C';
					cmnd_ch.clear();
					cmnd_type = '?';
				}
			}
		}
		else if (mode == 'I')
		{
			Line& curln = docc.get_cur_line();

			if (ch == 27)
			{
				mode = 'N';
				sel_on = false;
			}
			else if (ch == 8)
			{
				push_undo();
				if (curln.get_cur_col() > 0)
				{
					curln.move_left();
					curln.delete_at_Cur();
				}
				else
				{
					Paragraph& pr = docc.get_cur_chap().get_cur_sec().get_cur_para();
					int li = pr.get_cur_idx();
					if (li > 0)
					{
						int prevLen = pr.getLine(li - 1).get_len();
						pr.join_line(li);
						pr.set_cur_idx(li - 1);
						pr.get_cur_line().set_cur_col(prevLen);
					}
				}
			}
			else if (ch == 13)
			{
				push_undo();
				if (curln.is_emp())
				{
					enter_ct++;
					if (enter_ct == 1)
					{
						docc.get_cur_chap().get_cur_sec().new_para();
					}
					else if (enter_ct == 2)
					{
						docc.get_cur_chap().new_sec();
					}
					else
					{
						docc.new_chap();
						enter_ct = 0;
					}
				}
				else
				{
					Paragraph& pr = docc.get_cur_chap().get_cur_sec().get_cur_para();
					int li = pr.get_cur_idx();
					int col = curln.get_cur_col();
					pr.splitLine(li, col);
					pr.set_cur_idx(li + 1);
					pr.get_cur_line().set_cur_col(0);
					enter_ct = 0;
				}
			}
			else if (ch == 9)
			{
				push_undo();
				curln.indent();
				curln.set_cur_col(curln.get_cur_col() + 4);
			}
			else if (ch == 20)
			{
				push_undo();
				curln.unindent();
			}
			else if (ch == 127)
			{
				push_undo();
				if (curln.get_cur_col() < curln.get_len())
				{
					curln.delete_at_Cur();
				}
			}
			else if (ch == 3)
			{
				if (sel_on)
				{
					vector<char> full = curln.get_txt();
					int c1 = sel_col;
					int c2 = curln.get_cur_col();
					if (c1 > c2)
					{
						int t = c1;
						c1 = c2;
						c2 = t;
					}
					clip_txt.clear();
					for (int q = c1; q < c2; q++)
					{
						clip_txt.push_back(full[q]);
					}
					has_clip = true;
				}
			}
			else if (ch == 24)
			{
				if (sel_on)
				{
					push_undo();
					vector<char> full = curln.get_txt();
					int c1 = sel_col;
					int c2 = curln.get_cur_col();
					if (c1 > c2)
					{
						int t = c1;
						c1 = c2;
						c2 = t;
					}
					clip_txt.clear();
					for (int q = c1; q < c2; q++)
					{
						clip_txt.push_back(full[q]);
					}
					has_clip = true;
					for (int q = c1; q < c2; q++)
					{
						curln.set_cur_col(c1);
						curln.delete_at_Cur();
					}
					curln.set_cur_col(c1);
					sel_on = false;
				}
			}
			else if (ch == 22)
			{
				if (has_clip)
				{
					push_undo();
					for (int q = 0; q < clip_txt.size(); q++)
					{
						curln.insert_at_cur(clip_txt[q]);
					}
				}
			}
			else if (ch == 0 || ch == 224)
			{
				char ch2 = _getch();
				if (ch2 == 75)
				{
					curln.move_left();
					sel_on = false;
				}
				else if (ch2 == 77)
				{
					curln.move_right();
					sel_on = false;
				}
				else if (ch2 == 72)
				{
					docc.move_up();
					sel_on = false;
				}
				else if (ch2 == 80)
				{
					docc.move_down();
					sel_on = false;
				}
				else if (ch2 == 83)
				{
					push_undo();
					if (curln.get_cur_col() < curln.get_len())
					{
						curln.delete_at_Cur();
					}
				}
				else if (ch2 == 115)
				{
					if (!sel_on)
					{
						sel_on = true;
						sel_col = curln.get_cur_col();
					}
					curln.move_left();
				}
				else if (ch2 == 116)
				{
					if (!sel_on)
					{
						sel_on = true;
						sel_col = curln.get_cur_col();
					}
					curln.move_right();
				}
			}
			else
			{
				push_undo();
				curln.insert_at_cur(ch);
				enter_ct = 0;
			}
		}
		else if (mode == 'C')
		{
			if (ch == 27)
			{
				mode = 'N';
				cmnd_ch.clear();
				cmnd_type = ':';
			}
			else if (ch == 13)
			{
				if (cmnd_type == '/')
				{
					srch_pat = cmnd_ch;
					srch_fwd = true;
					find_and_jump(docc, srch_pat, true);
				}
				else if (cmnd_type == '?')
				{
					srch_pat = cmnd_ch;
					srch_fwd = false;
					find_and_jump(docc, srch_pat, false);
				}
				else if (vec_eq(cmnd_ch, "w"))
				{
					save_file(docc, cur_fname);
				}
				else if (cmnd_ch.size() > 2 && cmnd_ch[0] == 'w' && cmnd_ch[1] == ' ')
				{
					vector<char> newName;
					for (int i = 2; i < cmnd_ch.size(); i++)
					{
						newName.push_back(cmnd_ch[i]);
					}
					save_file(docc, newName);
					cur_fname = newName;
				}
				else if (vec_eq(cmnd_ch, "q"))
				{
					going = false;
				}
				else if (vec_eq(cmnd_ch, "wq"))
				{
					save_file(docc, cur_fname);
					going = false;
				}
				else if (vec_eq(cmnd_ch, "q!"))
				{
					going = false;
				}
				else if (vec_eq(cmnd_ch, "nc"))
				{
					docc.new_chap();
				}
				else if (vec_eq(cmnd_ch, "ns"))
				{
					docc.get_cur_chap().new_sec();
				}
				else if (vec_eq(cmnd_ch, "np"))
				{
					docc.get_cur_chap().get_cur_sec().new_para();
				}
				else if (vec_eq(cmnd_ch, "n"))
				{
					if (!srch_pat.empty())
					{
						find_and_jump(docc, srch_pat, srch_fwd);
					}
				}
				else if (vec_eq(cmnd_ch, "N"))
				{
					if (!srch_pat.empty())
					{
						find_and_jump(docc, srch_pat, !srch_fwd);
					}
				}
				else if (vec_eq(cmnd_ch, "set number"))
				{
					num_mode = true;
				}
				else if (vec_eq(cmnd_ch, "set nonumber"))
				{
					num_mode = false;
				}
				else if (cmnd_ch.size() >= 3 && cmnd_ch[0] == '%' && cmnd_ch[1] == 's' && cmnd_ch[2] == '/')
				{
					int p2 = -1, p3 = -1;
					int slashCt = 0;
					for (int i = 3; i < cmnd_ch.size(); i++)
					{
						if (cmnd_ch[i] == '/')
						{
							slashCt++;
							if (slashCt == 1)
							{
								p2 = i;
							}
							else if (slashCt == 2)
							{
								p3 = i;
							}
						}
					}
					if (p2 != -1)
					{
						vector<char> oldPat;
						for (int i = 3; i < p2; i++)
						{
							oldPat.push_back(cmnd_ch[i]);
						}
						vector<char> newPat;
						int endPos = (p3 != -1) ? p3 : cmnd_ch.size();
						for (int i = p2 + 1; i < endPos; i++)
						{
							newPat.push_back(cmnd_ch[i]);
						}
						push_undo();
						replace_all(docc, oldPat, newPat);
					}
				}
				cmnd_ch.clear();
				cmnd_type = ':';
				mode = 'N';
			}
			else if (ch == 8)
			{
				if (!cmnd_ch.empty())
				{
					cmnd_ch.pop_back();
				}
			}
			else
			{
				cmnd_ch.push_back(ch);
			}
		}
	}
	return 0;
}