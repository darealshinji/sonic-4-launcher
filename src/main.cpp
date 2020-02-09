/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020, djcj <djcj@gmx.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <windows.h>

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>

#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>

#ifdef __GNUC__
#include "images.h"
#else
#include "../Obj/images.h"
#endif

#include "lang.h"
#include "configuration.hpp"

// https://blogs.msdn.microsoft.com/oldnewthing/20041025-00/?p=37483
// https://stackoverflow.com/a/557859
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT  reinterpret_cast<HINSTANCE>(&__ImageBase)

#define MAX_PATH_LENGTH      4096
#define STRINGIFY(x)         #x
#define XSTRINGIFY(x)        STRINGIFY(x)
#define FLTK_VERSION_STRING  XSTRINGIFY(FL_MAJOR_VERSION) "." XSTRINGIFY(FL_MINOR_VERSION) "." XSTRINGIFY(FL_PATCH_VERSION)
#define LS                   12  /* default labelsize */
#define MENUITEM(x)          { x, 0,0,0,0, FL_NORMAL_LABEL, FL_HELVETICA, LS, 0 }
#define ARRLEN(x)            (sizeof(x) / sizeof(*x))


typedef struct {
	uchar dxkey;
	const char *name;
} keyList_t;


class DirectInput
{
private:
	IDirectInput8 *m_directInput = NULL;
	IDirectInputDevice8 *m_keyboard = NULL;

public:
	unsigned char m_keyboardState[256] = { 0 };

	DirectInput() {}

	bool init();
	bool ReadKeyboard();
};

class MyChoice : public Fl_Choice
{
private:
	int _prev = 0;
	Fl_Menu_Item *_menu = NULL;

	void prev(int v) { _prev = v; }
	int prev() { return _prev; }

public:
	MyChoice(int X, int Y, int W, int H, const char *L = NULL)
		: Fl_Choice(X, Y, W, H, L)
	{
		labelsize(LS);
		align(FL_ALIGN_TOP_LEFT);
		clear_visible_focus();
	}

	void menu(const Fl_Menu_Item *m);
	Fl_Menu_Item *menu() { return _menu; }

	int handle(int event);
};

class kbButton : public Fl_Button
{
private:
	configuration *_config = NULL;
	int _keytype = KEYUP;

public:
	kbButton(int X, int Y, int W, int H)
		: Fl_Button(X, Y, W, H, NULL)
	{
		labelsize(LS);
		clear_visible_focus();
	}

	/* get/set pointer to a configuration */
	void config(configuration *c) { _config = c; }
	configuration *config() { return _config; }

	/* get/set the key type for this button */
	void keytype(int k);
	int keytype() { return _keytype; }

	/* get the directX value for this key */
	void dxkey(uchar n);

	/* set the directX value for this key; will also change the label */
	uchar dxkey() {
		return _config ? _config->key(_keytype) : 0;
	}
};

class PadBox : public Fl_Box
{
private:
	const int _minW = 50;
	int measure_width(void);

public:
	PadBox(int X, int Y, int H, const char *L = NULL, Fl_Align align = FL_ALIGN_LEFT);
};

class MyWindow : public Fl_Double_Window
{
private:
	kbButton *_but = NULL;

public:
	MyWindow(int W, int H, const char *L = NULL)
		: Fl_Double_Window(W, H, L)
	{}

	/* associate a kbButton; when a pressed key will be grabbed from DirectInput
	 * we can change the key associated to this button */
	void but(kbButton *o) { _but = o; }
	kbButton *but() { return _but; }

	int handle(int event);
};


static void startWindow(bool restart, int setX, int setY);

static configuration *config = NULL;
static DirectInput *directinput = NULL;
static MyWindow *win = NULL;
static Fl_Group *g2_keyboard, *g2_gamepad;
static kbButton *btUp, *btDown, *btLeft, *btRight, *btA, *btB, *btX, *btY, *btStart;

#define IMAGE(x)  static Fl_PNG_Image x(NULL, x##_png, sizeof(x##_png))
IMAGE(arrow_01);
IMAGE(arrow_02);
IMAGE(arrow_03);
IMAGE(arrow_04);
IMAGE(back1);
IMAGE(back2);
IMAGE(back3);
IMAGE(button_01);
IMAGE(button_02);
IMAGE(button_03);
IMAGE(button_04);
IMAGE(button_05);
IMAGE(icon);
IMAGE(pad_controls_v02);
#undef IMAGE

static int rv = 0;
static unsigned int lang = 0;

static wchar_t moduleRootDir[MAX_PATH_LENGTH];
static wchar_t confFile[MAX_PATH_LENGTH];

static const Fl_Menu_Item langItems[] =
{
	MENUITEM("English"),
	MENUITEM("Deutsch"),
	MENUITEM("Espa" "\xC3\xB1" "ol"),  /* Español */
	MENUITEM("Fran" "\xC3\xA7" "ais"),  /* Français */
	MENUITEM("Italiano"),
	MENUITEM("\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E"),  /* Japanese */
	//MENUITEM("Polski"),  /* not available as in-game language */
	{0}
};


// https://www.daemonology.net/blog/2008-06-05-faster-utf8-strlen.html
static void strip_last_utf8_char(char *s)
{
	int i = 0;
	int last_char = 0;

	/* bytes 0xC0 through 0xFF are the first byte
	 * of a UTF-8 multi-byte character */
	while (s[i]) {
		if ((s[i] & 0xC0) != 0x80) {
			last_char = i;
		}
		i++;
	}

	s[last_char] = 0;
}

bool DirectInput::init()
{
	if (DirectInput8Create(HINST_THISCOMPONENT, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<LPVOID *>(&m_directInput), NULL) != DI_OK)	{
		return false;
	}

	if (m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL) != DI_OK) {
		return false;
	}

	if (m_keyboard->SetDataFormat(&c_dfDIKeyboard) != DI_OK) {
		return false;
	}

	if (m_keyboard->Acquire() != DI_OK) {
		return false;
	}

	return true;
}

bool DirectInput::ReadKeyboard()
{
	memset(m_keyboardState, 0, sizeof(m_keyboardState));

	HRESULT res = m_keyboard->GetDeviceState(sizeof(m_keyboardState), reinterpret_cast<LPVOID>(m_keyboardState));

	if (res != DI_OK) {
		/* If the keyboard lost focus or was not acquired then try to get control back. */
		if (res == DIERR_INPUTLOST || res == DIERR_NOTACQUIRED) {
			m_keyboard->Acquire();
		} else {
			return false;
		}
	}
	return true;
}

int MyWindow::handle(int event)
{
	int evX, evY, minX, minY, maxX, maxY;
	uchar dxNew, dxOld;
	kbButton *bt = but();

	if (bt) {
		/* a key button was already pressed -> ignore mouse events */
		switch (event) {
		case FL_PUSH:
			evX = Fl::event_x();
			evY = Fl::event_y();
			minX = bt->x();
			minY = bt->y();
			maxX = bt->x() + bt->w();
			maxY = bt->y() + bt->h();
			/* if same button is pressed again -> restore */
			if (evX>=minX && evX<=maxX && evY>=minY && evY<=maxY) {
				bt->dxkey(bt->dxkey());
				bt->value(0);
				but(NULL);
				redraw();
				return 0;
			}
		case FL_DRAG:
		case FL_RELEASE:
		case FL_MOVE:
		case FL_MOUSEWHEEL:
			return 0;
		}

		if (bt->config() && event == FL_KEYDOWN) {
			dxNew = dxOld = bt->dxkey();

			if (directinput->init()) {
				while (true) {
					if (!directinput->ReadKeyboard()) {
						continue;
					}

					for (unsigned int i = 0; i < sizeof(directinput->m_keyboardState); ++i) {
						if ((directinput->m_keyboardState[i] & 128) == 0) {
							continue;
						}
						dxNew = static_cast<uchar>(i);
						break;
					}

					/* don't ignore escape */
					if (dxNew != DIK_ESCAPE && configuration::isIgnoredKey(dxNew)) {
						continue;
					}

					break;
				}
			}

			if (dxNew == dxOld) {
				/* just restore the previous button label */
				bt->dxkey(dxOld);
			} else {
				configuration *cfg = bt->config();
				int kt = bt->keytype();
				std::vector<uchar> v;

				for (int i = KEYUP; i <= KEYSTART; ++i) {
					if (kt == i) {
						v.push_back(dxNew);
					} else {
						v.push_back(cfg->key(i));
					}
				}

				std::sort(v.begin(), v.end());

				if (std::unique(v.begin(), v.end()) != v.end()) {
					/* duplicate keys */
					bt->dxkey(dxOld);
				} else {
					bt->dxkey(dxNew);
				}
			}

			bt->value(0);
			but(NULL);
			redraw();
		}
	}

	return Fl_Double_Window::handle(event);
}

void kbButton::dxkey(uchar n)
{
	// https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ee418641(v%3Dvs.85)
	const keyList_t keyNames[] =
	{
		{DIK_ABNT_C1, "ABNT C1"},
		{DIK_ABNT_C2, "ABNT C2"},
		{DIK_APOSTROPHE, "'"},
		{DIK_AT, "@"},
		{DIK_AX, "AX"},
		{DIK_BACK, "Back"},
		{DIK_COLON, ":"},
		{DIK_DELETE, "Delete"},
		{DIK_DOWN, "Down"},
		{DIK_END, "End"},
		{DIK_F13, "F13"},
		{DIK_F14, "F14"},
		{DIK_F15, "F15"},
		{DIK_GRAVE, "`"},
		{DIK_HOME, "Home"},
		{DIK_INSERT, "Insert"},
		{DIK_LCONTROL, "CTRL"},
		{DIK_LEFT, "Left"},
		{DIK_LMENU, "Alt"},
		{DIK_LSHIFT, "Shift"},
		{DIK_NEXT, "Page Down"},
		{DIK_OEM_102, "OEM 102"},
		{DIK_PAUSE, "Pause"},
		{DIK_PRIOR, "Page Up"},
		{DIK_RCONTROL, "Right CTRL"},
		{DIK_RETURN, "Enter"},
		{DIK_RIGHT, "Right"},
		{DIK_RMENU, "Right Alt"},
		{DIK_RSHIFT, "Right Shift"},
		{DIK_SYSRQ, "SYSRQ"},
		{DIK_TAB, "Tab"},
		{DIK_UNDERLINE, "_"},
		{DIK_UNLABELED, "UNLABELED"},
		{DIK_UP, "Up"},
		{DIK_YEN, "Yen"}
	};

	char buf[128] = { 0 };
	uchar dxOld = dxkey();
	uchar dx = n;

	label(NULL);

	/* prefer these labels over the localized ones */
	switch (dx) {
	case DIK_NUMPAD0:
		label("Num 0");
		break;
	case DIK_NUMPAD1:
		label("Num 1");
		break;
	case DIK_NUMPAD2:
		label("Num 2");
		break;
	case DIK_NUMPAD3:
		label("Num 3");
		break;
	case DIK_NUMPAD4:
		label("Num 4");
		break;
	case DIK_NUMPAD5:
		label("Num 5");
		break;
	case DIK_NUMPAD6:
		label("Num 6");
		break;
	case DIK_NUMPAD7:
		label("Num 7");
		break;
	case DIK_NUMPAD8:
		label("Num 8");
		break;
	case DIK_NUMPAD9:
		label("Num 9");
		break;
	case DIK_DECIMAL:
		label("Num .");
		break;
	case DIK_NUMPADCOMMA:
		label("Num ,");
		break;
	case DIK_DIVIDE:
		label("Num /");
		break;
	case DIK_MULTIPLY:
		label("Num *");
		break;
	case DIK_ADD:
		label("Num +");
		break;
	case DIK_SUBTRACT:
		label("Num -");
		break;
	case DIK_NUMPADEQUALS:
		label("Num =");
		break;
	case DIK_NUMPADENTER:
		label("Num Enter");
		break;
	default:
		break;
	}

	if (label()) {
		/* label was set, now save the key value and quit */
		if (_config) {
			_config->key(dx, keytype());
		}
		return;
	}

	if (dx == 0 || configuration::isIgnoredKey(dx)) {
		dx = dxOld;
	}

	/* save key value */
	if (_config) {
		_config->key(dx, keytype());
	}

	if (GetKeyNameTextA(dx << 16, buf, sizeof(buf) - 1) > 0) {
		fl_font(labelfont(), LS);

		/* test multibyte utf8 character stripping */
		/*
		snprintf(buf, sizeof(buf) - 1, "%s",
			// ÀàÁáÂâÄäÈèÉéÊêÌìÍíÎîÒòÓóÔôÖöÙùÚúÛûÜü
			"\xC3\x80\xC3\xA0\xC3\x81\xC3\xA1\xC3\x82\xC3\xA2\xC3\x84\xC3\xA4\xC3\x88"
			"\xC3\xA8\xC3\x89\xC3\xA9\xC3\x8A\xC3\xAA\xC3\x8C\xC3\xAC\xC3\x8D\xC3\xAD"
			"\xC3\x8E\xC3\xAE\xC3\x92\xC3\xB2\xC3\x93\xC3\xB3\xC3\x94\xC3\xB4\xC3\x96"
			"\xC3\xB6\xC3\x99\xC3\xB9\xC3\x9A\xC3\xBA\xC3\x9B\xC3\xBB\xC3\x9C\xC3\xBC"

			// 今日はこんにちは今日はこんにちは
			//"\xE4\xBB\x8A\xE6\x97\xA5\xE3\x81\xAF\xE3\x81\x93\xE3\x82\x93\xE3\x81\xAB"
			//"\xE3\x81\xA1\xE3\x81\xAF\xE4\xBB\x8A\xE6\x97\xA5\xE3\x81\xAF\xE3\x81\x93"
			//"\xE3\x82\x93\xE3\x81\xAB\xE3\x81\xA1\xE3\x81\xAF"
		);
		*/

		int limit = w() - 2;

		/* shrink label until it fits the widget */
		if (static_cast<int>(fl_width(buf)) > limit) {
			while (buf[0] != 0) {
				strip_last_utf8_char(buf);
				if (static_cast<int>(fl_width(buf)) <= limit) {
					break;
				}
			}
		}

		copy_label(buf);
	} else {
		for (unsigned int i = 0; i < ARRLEN(keyNames); ++i) {
			if (dx == keyNames[i].dxkey) {
				label(keyNames[i].name);
				return;
			}
		}

		_snprintf_s(buf, sizeof(buf) - 1, "0x%X", dx);
		copy_label(buf);
	}
}

void kbButton::keytype(int k)
{
	if (k >= KEYUP && k <= KEYSTART) {
		_keytype = k;
	} else {
		_keytype = KEYUP;
	}

	if (!_config) {
		return;
	}

	/* set label with dxkey() */
	dxkey(_config->key(_keytype));
}

PadBox::PadBox(int X, int Y, int H, const char *L, Fl_Align align)
	: Fl_Box(X, Y, 1, H, L)
{
	int W = measure_width();
	if (align == FL_ALIGN_RIGHT) {
		X = X - (W - _minW);
	}
	resize(X, Y, W, H);

	box(FL_BORDER_BOX);
	color(FL_WHITE);
	labelsize(LS);
}

int PadBox::measure_width(void)
{
	int w = 0;

	if (!label()) {
		return _minW;
	}

	Fl_Box *o = new Fl_Box(0, 0, 0, 0, label());
	fl_font(o->labelfont(), o->labelsize());
	w = static_cast<int>(fl_width(o->label()));

	if (w < _minW) {
		w = _minW;
	}

	delete o;
	return w;
}

void MyChoice::menu(const Fl_Menu_Item *m)
{
	/* make sure we have a local copy of the menu with write access */
	Fl_Choice::copy(m);
	_menu = const_cast<Fl_Menu_Item *>(Fl_Choice::menu());
}

int MyChoice::handle(int event)
{
	const Fl_Menu_Item *m;

	if (!menu() || !menu()->text) {
		return 0;
	}

	_menu[prev()].labelfont_ = FL_HELVETICA;
	_menu[value()].labelfont_ = FL_HELVETICA;
	prev(value());

	if (event == FL_PUSH) {
		/* highlight the currently selected value */
		_menu[value()].labelfont_ = FL_HELVETICA_BOLD;

		if (Fl::visible_focus()) {
			Fl::focus(this);
		}

		if (Fl::scheme() || fl_contrast(textcolor(), FL_BACKGROUND2_COLOR) != textcolor()) {
			m = menu()->pulldown(x(), y(), w(), h(), NULL, this);
		} else {
			Fl_Color c = color();
			color(FL_BACKGROUND2_COLOR);
			m = menu()->pulldown(x(), y(), w(), h(), NULL, this);
			color(c);
		}

		if (!m) {
			return 1;
		}
		
		if (m != mvalue()) {
			redraw();
		}

		picked(m);
		return 1;
	}

	return Fl_Choice::handle(event);
}

static bool getModuleRootDir(void)
{
	wchar_t mod[MAX_PATH_LENGTH];
	wchar_t *wcp = NULL;

	if (!GetModuleFileNameW(NULL, mod, MAX_PATH_LENGTH)) {
		return false;
	}

	if ((wcp = wcsrchr(mod, L'\\')) == NULL) {
		return false;
	}
	*wcp = 0;

	SecureZeroMemory(&moduleRootDir, MAX_PATH_LENGTH * sizeof(wchar_t));
	SecureZeroMemory(&confFile, MAX_PATH_LENGTH * sizeof(wchar_t));

	wcscpy_s(moduleRootDir, MAX_PATH_LENGTH - 1, mod);
	wcscpy_s(confFile, MAX_PATH_LENGTH - 1, mod);
	wcscat_s(confFile, MAX_PATH_LENGTH - 1, L"\\main.conf");

	return true;
}

static int launchGame(void)
{
	const char *title = "Error: Sonic_vis.exe";
	wchar_t command[MAX_PATH_LENGTH];
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;

	wcscpy_s(command, MAX_PATH_LENGTH - 1, moduleRootDir);
	wcscat_s(command, MAX_PATH_LENGTH - 1, L"\\Sonic_vis.exe");

	SecureZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	SecureZeroMemory(&pi, sizeof(pi));

	if (CreateProcessW(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == FALSE) {
		MessageBoxA(0, "Failed calling CreateProcess()", title, MB_ICONERROR|MB_OK);
		return 1;
	}

	DWORD wait = WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (wait == WAIT_ABANDONED) {
		MessageBoxA(0, "Process abandoned.", title, MB_ICONERROR|MB_OK);
	} else if (wait == WAIT_TIMEOUT) {
		MessageBoxA(0, "Process time-out error.", title, MB_ICONERROR|MB_OK);
	} else if (wait == WAIT_FAILED) {
		MessageBoxA(0, "Process failed", title, MB_ICONERROR|MB_OK);
	}

	return wait;
}

static void setResolution_cb(Fl_Widget *o, void *)
{
	MyChoice *b = dynamic_cast<MyChoice *>(o);
	config->resN(b->value());
}

static void setDisplay_cb(Fl_Widget *o, void *)
{
	MyChoice *b = dynamic_cast<MyChoice *>(o);
	config->display(static_cast<uchar>(b->value()));
}

static void setLang_cb(Fl_Widget *o, void *)
{
	MyChoice *b = dynamic_cast<MyChoice *>(o);
	int setX = win->x();
	int setY = win->y();
	lang = b->value();
	config->language(static_cast<uchar>(lang));
	win->hide();
	startWindow(true, setX, setY);
}

static void fullscreen_cb(Fl_Widget *, void *)
{
	config->fullscreen(config->fullscreen() == 0 ? 1 : 0);
}

static void vibrate_cb(Fl_Widget *, void *)
{
	config->vibra(config->vibra() == 0 ? 1 : 0);
}

static void setController_cb(Fl_Widget *o, void *v)
{
	MyChoice *p = dynamic_cast<MyChoice *>(o);
	Fl_Box *b = reinterpret_cast<Fl_Box *>(v);
	int n = p->value();

	if (n == GAMEPAD_CTRLS) {
		config->controls(n);
		b->image(&back2);
		g2_keyboard->hide();
		g2_gamepad->show();
	} else {
		config->controls(KEYBOARD_CTRLS);
		b->image(&back3);
		g2_keyboard->show();
		g2_gamepad->hide();
	}

	win->redraw();
}

static void setDefaultKeys_cb(Fl_Widget *, void *)
{
	config->setDefaultKeys();

	/* "refresh" buttons */
	btUp->dxkey(config->key(KEYUP));
	btDown->dxkey(config->key(KEYDOWN));
	btLeft->dxkey(config->key(KEYLEFT));
	btRight->dxkey(config->key(KEYRIGHT));
	btA->dxkey(config->key(KEYA));
	btB->dxkey(config->key(KEYB));
	btX->dxkey(config->key(KEYX));
	btY->dxkey(config->key(KEYY));
	btStart->dxkey(config->key(KEYSTART));

	win->redraw();
}

static void setKey_cb(Fl_Widget *o, void *)
{
	kbButton *b = dynamic_cast<kbButton *>(o);
	b->label(ui_Press[lang]);  /* "Press!" */
	b->value(1);
	win->but(b);
	win->redraw();
}

static void bigButton_cb(Fl_Widget *, void *)
{
	if (!config->saveConfig()) {
		MessageBoxA(0, "Couldn't save configuration.", "Error", MB_ICONERROR|MB_OK);
	}
	win->hide();
	rv = launchGame();
}

static int esc_handler(int event)
{
	if (event == FL_SHORTCUT && Fl::event_key() == FL_Escape) {
		return 1; /* ignore Escape key */
	}
	return 0;
}

static void startWindow(bool restart, int setX, int setY)
{
	Fl_Tabs *tabs;
	Fl_Group *g1, *g2;
	Fl_Button *bigButton;
	std::string *devLabels;
	Fl_Menu_Item *devItems;
	char buf[128], bufJB[128], bufJS[128];

	int sc = config->screenCount();
	devLabels = new std::string[sc];
	devItems = new Fl_Menu_Item[sc + 1];

	if (!restart && !config->loadConfig()) {
		config->loadDefaultConfig();
	}
	lang = config->language();

	if (lang >= ARRLEN(langItems) - 1) {
		lang = 0;  /* English */
	}

	Fl::add_handler(esc_handler);
	Fl::get_system_colors();

	/* The icon from the exe's resources isn't used by default. Weird. */
	Fl_Window::default_icon(&icon);

	win = new MyWindow(762, 656, "SONIC THE HEDGEHOG 4 Episode I");
	{
		tabs = new Fl_Tabs(32, 16, 698, 532);
		{
			/* "Settings" */
			g1 = new Fl_Group(32, 36, 698, 512, ui_Settings[lang]);
			{
				/* Resolution list */
				Fl_Menu_Item resItems[SZRESLIST + 1];

				for (int i = 0; i < SZRESLIST; ++i) {
					resItems[i] = MENUITEM(configuration::getReslistL(i));
				}
				resItems[SZRESLIST] = {0};

				/* Display list */
				for (int i = 0; i < sc; ++i) {
					_snprintf_s(buf, sizeof(buf) - 1, "Display %d", i);
					devLabels[i] = buf;
					devItems[i] = MENUITEM(devLabels[i].c_str());
				}
				devItems[sc] = {0};

				/* Background image */
				{ Fl_Box *o = new Fl_Box(-1, 9, 1, 1);
				o->align(FL_ALIGN_BOTTOM_LEFT);
				o->image(&back1); }

				/* Display selection */
				{ MyChoice *o = new MyChoice(42, 64, 328, 24, ui_GraphicsDevice[lang]);
				o->menu(devItems);
				o->callback(setDisplay_cb); }

				/* Resolution */
				{ MyChoice *o = new MyChoice(42, 112, 328, 24, ui_Resolution[lang]);
				o->menu(resItems);
				o->value(config->resN());
				o->callback(setResolution_cb); }
				
				/* Fullscreen */
				{ Fl_Check_Button *o = new Fl_Check_Button(42, 150, 328, 24, ui_Fullscreen[lang]);
				o->labelsize(LS);
				o->value(config->fullscreen() == 0 ? 0 : 1);
				o->clear_visible_focus();
				o->callback(fullscreen_cb); }

				/* Language */
				{ MyChoice *o = new MyChoice(42, 228, 328, 24, ui_Language[lang]);
				o->menu(langItems);
				o->value(lang);
				o->callback(setLang_cb); }
			}
			g1->end();
			g1->labelsize(LS);

			_snprintf_s(buf, sizeof(buf) - 1, "%s %d", ui_Player[lang], 1);
			_snprintf_s(bufJB, sizeof(bufJB) - 1, "%s / %s", ui_Jump[lang], ui_Back[lang]);
			_snprintf_s(bufJS, sizeof(bufJS) - 1, "%s / %s", ui_Jump[lang], ui_Select[lang]);

			/* "Player 1" */
			g2 = new Fl_Group(32, 36, 698, 512);
			g2->label(buf);
			{
				const Fl_Menu_Item conItems[] = {
					MENUITEM(ui_Keyboard[lang]),
					MENUITEM(ui_Gamepad[lang]),
					{0}
				};

				/* Background image */
				Fl_Box *bg = new Fl_Box(-1, 2, 1, 1);
				bg->align(FL_ALIGN_BOTTOM_LEFT);

				/* Keyboard bindings */
				g2_keyboard = new Fl_Group(32, 36, 698, 512);
				{
					/* Reset settings */
					{ Fl_Button *o = new Fl_Button(42, 102, 328, 24, ui_ResetToDefault[lang]);
					o->labelsize(LS);
					o->clear_visible_focus();
					o->callback(setDefaultKeys_cb); }

					/* "Movement" frame */
					{ Fl_Box *o = new Fl_Box(59, 192, 312, 277, ui_Movement[lang]);
					o->labelsize(LS);
					o->align(FL_ALIGN_TOP_LEFT);
					o->box(FL_ENGRAVED_FRAME); }
					
					/* Up */
					btUp = new kbButton(174, 241, 89, 38);
					btUp->config(config);
					btUp->keytype(KEYUP);
					btUp->callback(setKey_cb);
					{ Fl_Box *o = new Fl_Box(174, 203, 89, 38, ui_Up[lang]);
					o->labelsize(LS); }
					{ Fl_Box *o = new Fl_Box(216, 299, 1, 1);
					o->image(&arrow_04); }

					/* Left */
					btLeft = new kbButton(70, 311, 89, 38);
					btLeft->config(config);
					btLeft->keytype(KEYLEFT);
					btLeft->callback(setKey_cb);
					{ Fl_Box *o = new Fl_Box(70, 273, 89, 38, ui_Left[lang]);
					o->labelsize(LS); }
					{ Fl_Box *o = new Fl_Box(179, 330, 1, 1);
					o->image(&arrow_01); }

					/* Right */
					btRight = new kbButton(274, 311, 89, 38);
					btRight->config(config);
					btRight->keytype(KEYRIGHT);
					btRight->callback(setKey_cb);
					{ Fl_Box *o = new Fl_Box(274, 273, 89, 38, ui_Right[lang]);
					o->labelsize(LS); }
					{ Fl_Box *o = new Fl_Box(254, 330, 1, 1);
					o->image(&arrow_02); }

					/* Down */
					btDown = new kbButton(174, 381, 89, 38);
					btDown->config(config);
					btDown->keytype(KEYDOWN);
					btDown->callback(setKey_cb);
					{ Fl_Box *o = new Fl_Box(174, 423, 89, 38, ui_Down[lang]);
					o->labelsize(LS); }
					{ Fl_Box *o = new Fl_Box(216, 365, 1, 1);
					o->image(&arrow_03); }

					/* "Action" frame */
					{ Fl_Box *o = new Fl_Box(407, 192, 294, 277, ui_Action[lang]);
					o->labelsize(LS);
					o->align(FL_ALIGN_TOP_LEFT);
					o->box(FL_ENGRAVED_FRAME); }

					/* Score Attack / Time Attack */
					btX = new kbButton(432, 243, 89, 38);
					btX->config(config);
					btX->keytype(KEYX);
					btX->callback(setKey_cb);
					{ Fl_Box *o = new Fl_Box(452, 223, 1, 1, ui_ScoreAttack[lang]);
					o->labelsize(LS);
					o->align(FL_ALIGN_RIGHT); }
					{ Fl_Box *o = new Fl_Box(432, 223, 1, 1);
					o->image(&button_04); }
					
					/* Super Sonic */
					btY = new kbButton(432, 329, 89, 38);
					btY->config(config);
					btY->keytype(KEYY);
					btY->callback(setKey_cb);
					{ Fl_Box *o = new Fl_Box(452, 305, 1, 1, ui_SuperSonic[lang]);
					o->labelsize(LS);
					o->align(FL_ALIGN_RIGHT); }
					{ Fl_Box *o = new Fl_Box(432, 305, 1, 1);
					o->image(&button_01); }

					/* Jump / Back */
					btB = new kbButton(432, 411, 89, 38);
					btB->config(config);
					btB->keytype(KEYB);
					btB->callback(setKey_cb);
					{ Fl_Box *o = new Fl_Box(452, 387, 1, 1, bufJB);
					o->labelsize(LS);
					o->align(FL_ALIGN_RIGHT); }
					{ Fl_Box *o = new Fl_Box(432, 387, 1, 1);
					o->image(&button_02); }

					/* Start */
					btStart = new kbButton(590, 329, 89, 38);
					btStart->config(config);
					btStart->keytype(KEYSTART);
					btStart->callback(setKey_cb);
					{ Fl_Box *o = new Fl_Box(590, 305, 1, 1, ui_Start[lang]);
					o->labelsize(LS);
					o->align(FL_ALIGN_RIGHT); }
					{ Fl_Box *o = new Fl_Box(570, 305, 1, 1);
					o->image(&button_05); }

					/* Jump / Select */
					btA = new kbButton(590, 411, 89, 38);
					btA->config(config);
					btA->keytype(KEYA);
					btA->callback(setKey_cb);
					{ Fl_Box *o = new Fl_Box(590, 387, 1, 1, bufJS);
					o->labelsize(LS);
					o->align(FL_ALIGN_RIGHT); }
					{ Fl_Box *o = new Fl_Box(570, 387, 1, 1);
					o->image(&button_03); }
				}
				g2_keyboard->end();

				/* Gamepad bindings */
				g2_gamepad = new Fl_Group(32, 36, 698, 512);
				{
					/* Vibrate */
					{ Fl_Check_Button *o = new Fl_Check_Button(42, 102, 328, 24, ui_Vibrate[lang]);
					o->labelsize(LS);
					o->value(config->vibra() == 0 ? 0 : 1);
					o->clear_visible_focus();
					o->callback(vibrate_cb); }

					/* Gamepad overlay image */
					{ Fl_Box *o = new Fl_Box(368, 298, 1, 1);
					o->image(&pad_controls_v02); }

					new PadBox(144, 207, 18, ui_Back[lang], FL_ALIGN_RIGHT);
					new PadBox(144, 240, 18, ui_Up[lang], FL_ALIGN_RIGHT);
					new PadBox(144, 268, 18, ui_Right[lang], FL_ALIGN_RIGHT);
					new PadBox(144, 295, 18, ui_Left[lang], FL_ALIGN_RIGHT);
					new PadBox(144, 322, 18, ui_Down[lang], FL_ALIGN_RIGHT);
					new PadBox(542, 207, 18, ui_Start[lang]);
					new PadBox(542, 234, 18, ui_SuperSonic[lang]);
					new PadBox(542, 258, 18, ui_ScoreAttack[lang]);
					new PadBox(542, 302, 18, bufJB);
					new PadBox(542, 328, 18, bufJS);
				}
				g2_gamepad->end();

				/* Select keyboard/controller */
				{ MyChoice *o = new MyChoice(42, 64, 328, 24, ui_ControllerSelection[lang]);
				o->menu(conItems);
				o->value(config->controls());
				o->callback(setController_cb, reinterpret_cast<void *>(bg));
				/* Run callback once */
				setController_cb(o, reinterpret_cast<void *>(bg)); }
			}
			g2->end();
			g2->labelsize(LS);
		}
		tabs->end();
		tabs->clear_visible_focus();

		/* launch button */
		bigButton = new Fl_Button(62, 564, 642, 68, ui_SaveSettings[lang]);
		bigButton->labelsize(16);
		bigButton->clear_visible_focus();
		bigButton->callback(bigButton_cb);

		{ Fl_Box *o = new Fl_Box(762, 641, 1, 1, "using FLTK " FLTK_VERSION_STRING);
		o->align(FL_ALIGN_LEFT_TOP);
		o->labelsize(10);
		o->deactivate(); }
	}
	win->end();

	if (restart) {
		/* window restarted, restore old positions */
		win->position(setX, setY);
	} else {
		/* new window, position in center */
		win->position((Fl::w() - 762) / 2, (Fl::h() - 656) / 2);
	}
	win->show();

	Fl::run();

	delete[] devItems;
	delete[] devLabels;
}

int main(int argc, char *argv[])
{
	if (!getModuleRootDir()) {
		MessageBoxA(0, "Failed calling GetModuleFileName()", "Error", MB_ICONERROR|MB_OK);
		return 1;
	}

	config = new configuration(confFile);

	if (argc > 0) {
		for (int i = 0; i < argc; ++i) {
			if (stricmp(argv[i], "-QuickBoot") == 0) {
				if (!config->loadConfig()) {
					config->loadDefaultConfig();
					config->saveConfig();
				}
				delete config;
				return launchGame();
			}
		}
	}

	directinput = new DirectInput();

	/* needs to be initialized before we launch our window */
	directinput->init();

	startWindow(false, 0, 0);

	delete directinput;
	delete config;
	return rv;
}
