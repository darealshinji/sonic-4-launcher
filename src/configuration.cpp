/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, djcj <djcj@gmx.de>
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

#include <Windows.h>
#include <FL/Fl.H>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>

#include "keyCodes.h"
#include "configuration.hpp"

#define CONF_SIZE        53
#define MAX_PATH_LENGTH  2048 * sizeof(wchar_t)
#define TO_UINT32(x)     static_cast<uint32_t>(((0xFF & x[0]) << 0 | (0xFF & x[1]) << 8 | (0xFF & x[2]) << 16 | (0xFF & x[3]) << 24))


typedef struct {
	int fltk;
	uchar dxkey;
	const char *name;
} keyList_t;


static const keyList_t keylist[] =
{
	//{FL_Escape, DIK_ESCAPE, "Escape"},
	{'1', DIK_1, "1"},
	{'2', DIK_2, "2"},
	{'3', DIK_3, "3"},
	{'4', DIK_4, "4"},
	{'5', DIK_5, "5"},
	{'6', DIK_6, "6"},
	{'7', DIK_7, "7"},
	{'8', DIK_8, "8"},
	{'9', DIK_9, "9"},
	{'0', DIK_0, "0"},
	{'-', DIK_MINUS, "-"},
	{'=', DIK_EQUALS, "="},
	{FL_BackSpace, DIK_BACK, "Back"},
	{FL_Tab, DIK_TAB, "Tab"},
	{'q', DIK_Q, "Q"},
	{'w', DIK_W, "W"},
	{'e', DIK_E, "E"},
	{'r', DIK_R, "R"},
	{'t', DIK_T, "T"},
	{'y', DIK_Y, "Y"},
	{'u', DIK_U, "U"},
	{'i', DIK_I, "I"},
	{'o', DIK_O, "O"},
	{'p', DIK_P, "P"},
	{'[', DIK_LBRACKET, "["},
	{']', DIK_RBRACKET, "]"},
	{FL_Enter, DIK_RETURN, "Enter"},
	{FL_Control_L, DIK_LCONTROL, "Left Ctrl"},
	{'a', DIK_A, "A"},
	{'s', DIK_S, "S"},
	{'d', DIK_D, "D"},
	{'f', DIK_F, "F"},
	{'g', DIK_G, "G"},
	{'h', DIK_H, "H"},
	{'j', DIK_J, "J"},
	{'k', DIK_K, "K"},
	{'l', DIK_L, "L"},
	{';', DIK_SEMICOLON, ";"},
	{'\'', DIK_APOSTROPHE, "'"},
	{'`', DIK_GRAVE, "`"},
	{FL_Shift_L, DIK_LSHIFT, "Shift"},
	{'\\', DIK_BACKSLASH, "\\"},
	{'z', DIK_Z, "Z"},
	{'x', DIK_X, "X"},
	{'c', DIK_C, "C"},
	{'v', DIK_V, "V"},
	{'b', DIK_B, "B"},
	{'n', DIK_N, "N"},
	{'m', DIK_M, "M"},
	{',', DIK_COMMA, ","},
	{'.', DIK_PERIOD, "."},
	{'/', DIK_SLASH, "/"},
	{FL_Shift_R, DIK_RSHIFT, "Right Shift"},
	{FL_KP + '*', DIK_MULTIPLY, "Num *"},
	{FL_Alt_L, DIK_LMENU, "Alt"},
	{' ', DIK_SPACE, "Space"},
	//{FL_Caps_Lock, DIK_CAPITAL, "Caps Lock"},
	{FL_F + 1, DIK_F1, "F1"},
	{FL_F + 2, DIK_F2, "F2"},
	{FL_F + 3, DIK_F3, "F3"},
	{FL_F + 4, DIK_F4, "F4"},
	{FL_F + 5, DIK_F5, "F5"},
	{FL_F + 6, DIK_F6, "F6"},
	{FL_F + 7, DIK_F7, "F7"},
	{FL_F + 8, DIK_F8, "F8"},
	{FL_F + 9, DIK_F9, "F9"},
	{FL_F + 10, DIK_F10, "F10"},
	//{FL_Num_Lock, DIK_NUMLOCK, "Num Lock"},
	//{FL_Scroll_Lock, DIK_SCROLL, "Scroll Lock"},
	{FL_KP + '7', DIK_NUMPAD7, "Num 7"},
	{FL_KP + '8', DIK_NUMPAD8, "Num 8"},
	{FL_KP + '9', DIK_NUMPAD9, "Num 9"},
	{FL_KP + '-', DIK_SUBTRACT, "Num -"},
	{FL_KP + '4', DIK_NUMPAD4, "Num 4"},
	{FL_KP + '5', DIK_NUMPAD5, "Num 5"},
	{FL_KP + '6', DIK_NUMPAD6, "Num 6"},
	{FL_KP + '+', DIK_ADD, "Num +"},
	{FL_KP + '1', DIK_NUMPAD1, "Num 1"},
	{FL_KP + '2', DIK_NUMPAD2, "Num 2"},
	{FL_KP + '3', DIK_NUMPAD3, "Num 3"},
	{FL_KP + '0', DIK_NUMPAD0, "Num 0"},
	{FL_KP + '.', DIK_DECIMAL, "Num ."},
	{FL_Iso_Key, DIK_OEM_102, "<> \\|"},
	{FL_F + 11, DIK_F11, "F11"},
	{FL_F + 12, DIK_F12, "F12"},
	{FL_F + 13, DIK_F13, "F13"},
	{FL_F + 14, DIK_F14, "F14"},
	{FL_F + 15, DIK_F15, "F15"},
	{FL_Kana, DIK_KANA, "Kana"},
	{FL_Yen, DIK_YEN, "\xC2\xA5"},
	//{FL_KP + '=', DIK_NUMPADEQUALS, "Num ="},  // ???
	{'^', DIK_CIRCUMFLEX, "^"},
	{'@', DIK_AT, "@"},
	{':', DIK_COLON, ":"},
	{FL_JIS_Underscore, DIK_UNDERLINE, "_"},
	{FL_KP_Enter, DIK_NUMPADENTER, "Num Enter"},
	{FL_Control_R, DIK_RCONTROL, "Right Ctrl"},
	//{FL_KP + ',', DIK_NUMPADCOMMA, "Num ,"},  // ???
	{FL_KP + '/', DIK_DIVIDE, "Num /"},
	{FL_Alt_R, DIK_RMENU, "Right Alt"},
	{FL_Pause, DIK_PAUSE, "Pause"},
	{FL_Home, DIK_HOME, "Home"},
	{FL_Up, DIK_UP, "Up"},
	{FL_Page_Up, DIK_PRIOR, "Page Up"},
	{FL_Left, DIK_LEFT, "Left"},
	{FL_Right, DIK_RIGHT, "Right"},
	{FL_End, DIK_END, "End"},
	{FL_Down, DIK_DOWN, "Down"},
	{FL_Page_Down, DIK_NEXT, "Page Down"},
	{FL_Insert, DIK_INSERT, "Insert"},
	{FL_Delete, DIK_DELETE, "Delete"},
	//{FL_Meta_L, DIK_LWIN, "Left Win"},
	//{FL_Meta_R, DIK_RWIN, "Right Win"},
	{FL_Menu, DIK_APPS, "Menu"}
};

static const int szResList = 12;

const res_t configuration::resList[szResList] =
{
	{640, 480, "640x480"},
	{800, 600, "800x600"},
	{1024, 768, "1024x768"},
	{1152, 864, "1152x864"},
	{1280, 720, "1280x720"},
	{1280, 800, "1280x800"},
	{1280, 960, "1280x960"},
	{1600, 900, "1600x900"},
	{1680, 1050, "1680x1050"},
	{1400, 1050, "1400x1050"},
	{1440, 900, "1440x900"},
	{1920, 1080, "1920x1080"}
};


uint16_t configuration::resXN(int n)
{
	return (n < szResList) ? resList[n].w : resList[0].w;
}

uint16_t configuration::resYN(int n)
{
	return (n < szResList) ? resList[n].h : resList[0].h;
}

void configuration::resN(size_t n)
{
	if (n < szResList) {
		_resX = resXN(n);
		_resY = resYN(n);
		_resN = n;
	} else {
		_resX = resXN(0);
		_resY = resYN(0);
		_resN = 0;
	}
}

const char *configuration::getNameDx(uchar dx)
{
	for (int i = 0; i < (sizeof(keylist) / sizeof(*keylist)); ++i) {
		if (keylist[i].dxkey == dx) {
			return keylist[i].name;
		}
	}
	return "<not set>";
}

bool configuration::is_valid_dxkey(uchar dx)
{
	for (size_t i = 0; i < (sizeof(keylist) / sizeof(*keylist)); ++i) {
		if (keylist[i].dxkey == dx) {
			return true;
		}
	}
	return false;
}

uchar configuration::getDxKey(int fltk)
{
	switch (fltk) {
	case FL_Escape:
	case FL_Caps_Lock:
	case FL_Num_Lock:
	case FL_Scroll_Lock:
	case FL_Meta_L:  /* Left Windows key */
	case FL_Meta_R:  /* Right Windows key */
		/* Ignore all these keys */
		return 0;
	}

	if (fltk >= 'A' && fltk <= 'Z') {
		fltk = tolower(fltk);
	}

	for (size_t i = 0; i < (sizeof(keylist) / sizeof(*keylist)); ++i) {
		if (keylist[i].fltk == fltk) {
			return keylist[i].dxkey;
		}
	}
	return 0;
}

bool configuration::loadConfig(void)
{
	FILE *fp = NULL;
	unsigned char buf[CONF_SIZE];
	unsigned char *p = buf;

	if (_wfopen_s(&fp, _confFile, L"rb") != NULL) {
		return false;
	}

	if (fread(&buf, 1, CONF_SIZE, fp) != CONF_SIZE) {
		fclose(fp);
		return false;
	}
	fclose(fp);

	/* magic number */
	if (TO_UINT32(p) != 20111005) {
		return false;
	}
	p += 4;

	_resX = static_cast<uint16_t>(((0xFF & p[0]) << 0 | (0xFF & p[1]) << 8));
	p += 2;
	_resY = static_cast<uint16_t>(((0xFF & p[0]) << 0 | (0xFF & p[1]) << 8));
	p += 2;

	for (int i = 0; i < szResList; ++i) {
		if (_resX == resList[i].w && _resY == resList[i].h) {
			_resN = i;
			break;
		} else if (resList[i].w == 0) {
			_resX = resList[0].w;
			_resY = resList[0].h;
			_resN = 0;
		}
	}

	_fullscreen = (p[0] == 0) ? 0 : 1;
	_language = p[1];
	_controls = (p[2] == GAMEPAD_CTRLS) ? GAMEPAD_CTRLS : KEYBOARD_CTRLS;
	_vibra = (p[3] == 0) ? 0 : 1;
	_display = (p[4] > _screenCount - 1) ? 0 : p[4];
	p += 5;

#define GETKEY(var,def)  var=p[0]; p+=4; if (!is_valid_dxkey(var)) { var=def; }

	GETKEY(_keyLeft, DIK_LEFT);
	GETKEY(_keyRight, DIK_RIGHT);
	GETKEY(_keyUp, DIK_UP);
	GETKEY(_keyDown, DIK_DOWN);
	GETKEY(_keyA, DIK_SPACE);
	GETKEY(_keyB, DIK_D);
	GETKEY(_keyX, DIK_A);
	GETKEY(_keyY, DIK_S);
	GETKEY(_keyStart, DIK_RETURN);

#undef GETKEY

	/* end number */
	if (TO_UINT32(p) != 1701) {
		return false;
	}

	return true;
}

void configuration::setDefaultKeys(void)
{
	_keyLeft = DIK_LEFT;
	_keyRight = DIK_RIGHT;
	_keyUp = DIK_UP;
	_keyDown = DIK_DOWN;
	_keyA = DIK_SPACE;
	_keyB = DIK_D;
	_keyX = DIK_A;
	_keyY = DIK_S;
	_keyStart = DIK_RETURN;
}

void configuration::loadDefaultConfig(void)
{
	_resN = 0;
	_resX = resList[_resN].w;
	_resY = resList[_resN].h;
	_fullscreen = 0;
	_language = 0;  /* English */
	_controls = KEYBOARD_CTRLS;
	_vibra = 0;
	_display = 0;
	setDefaultKeys();
}

bool configuration::saveConfig(void)
{
	FILE *fp = NULL;
	unsigned char buf[CONF_SIZE];
	unsigned char *p = buf;

	if (_wfopen_s(&fp, _confFile, L"wb") != NULL) {
		return false;
	}

	/* magic number (20111005) */
	putc(0x9D, fp);
	putc(0xDE, fp);
	putc(0x32, fp);
	putc(0x01, fp);

	putc(static_cast<uchar>(_resX), fp);
	putc(static_cast<uchar>(_resX >> 8), fp);
	putc(static_cast<uchar>(_resY), fp);
	putc(static_cast<uchar>(_resY >> 8), fp);

	putc(_fullscreen, fp);
	putc(_language, fp);
	putc(_controls, fp);
	putc(_vibra, fp);
	putc(_display, fp);

	/* the DIK_* values don't exceed 255, so it's not a real uint32_t value */
#define SETKEY(x)  putc(x, fp); putc(0x00, fp); putc(0x00, fp); putc(0x00, fp);

	SETKEY(_keyLeft);
	SETKEY(_keyRight);
	SETKEY(_keyUp);
	SETKEY(_keyDown);
	SETKEY(_keyA);
	SETKEY(_keyB);
	SETKEY(_keyX);
	SETKEY(_keyY);
	SETKEY(_keyStart);

#undef SETKEY

	/* end number (1701) */
	putc(0xA5, fp);
	putc(0x06, fp);
	putc(0x00, fp);
	putc(0x00, fp);

	fclose(fp);
	return true;
}

configuration::configuration(const wchar_t *filename) :
	_screenCount(0),
	_resN(0),
	_resX(0),
	_resY(0),
	_fullscreen(0),
	_language(0),
	_controls(0),
	_vibra(0),
	_display(0),
	_keyLeft(0),
	_keyRight(0),
	_keyUp(0),
	_keyDown(0),
	_keyA(0),
	_keyB(0),
	_keyX(0),
	_keyY(0),
	_keyStart(0)
{
	_confFile = filename;
	_screenCount = static_cast<uchar>(Fl::screen_count());

	if (_screenCount == 0) {
		_screenCount = 1;
	}
}

