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

#include <algorithm>
#include <vector>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>

#include "configuration.hpp"

#define CONF_SIZE     53
#define TO_UINT16(x)  static_cast<uint16_t>(((0xFF & x[0]) << 0 | (0xFF & x[1]) << 8))
#define TO_UINT32(x)  static_cast<uint32_t>(((0xFF & x[0]) << 0 | (0xFF & x[1]) << 8 | (0xFF & x[2]) << 16 | (0xFF & x[3]) << 24))


void configuration::resN(size_t n)
{
	if (n <= 0) {
		n = 0;
	} else if (n >= resList.size()) {
		n = resList.size() - 1;
	}
	_resW = resList.at(n).w;
	_resH = resList.at(n).h;
	_resN = n;
}

bool configuration::isIgnoredKey(uchar dx)
{
	switch (dx) {
	case DIK_APPS:
	case DIK_CALCULATOR:
	case DIK_CAPITAL:
	case DIK_CONVERT:
	case DIK_ESCAPE:
	case DIK_KANA:
	case DIK_KANJI:
	case DIK_LWIN:
	case DIK_MAIL:
	case DIK_MEDIASELECT:
	case DIK_MEDIASTOP:
	case DIK_MUTE:
	case DIK_MYCOMPUTER:
#ifdef DIK_NEXTTRACK
	case DIK_NEXTTRACK:
#endif
	case DIK_NOCONVERT:
	case DIK_NUMLOCK:
	case DIK_PLAYPAUSE:
	case DIK_POWER:
#ifdef DIK_PREVTRACK
	case DIK_PREVTRACK:
#endif
	case DIK_RWIN:
	case DIK_SCROLL:
	case DIK_SLEEP:
	case DIK_STOP:
	case DIK_VOLUMEDOWN:
	case DIK_VOLUMEUP:
	case DIK_WAKE:
	case DIK_WEBBACK:
	case DIK_WEBFAVORITES:
	case DIK_WEBFORWARD:
	case DIK_WEBHOME:
	case DIK_WEBREFRESH:
	case DIK_WEBSEARCH:
	case DIK_WEBSTOP:
		return true;
	}

	return false;
}

bool configuration::loadConfig(void)
{
	FILE *fp = NULL;
	unsigned char buf[CONF_SIZE];
	unsigned char *p = buf;
	bool resFound = false;
	std::vector<uchar> v;

	if (_wfopen_s(&fp, _confFile, L"rb") != 0) {
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

	_resW = TO_UINT16(p);
	p += 2;
	_resH = TO_UINT16(p);
	p += 2;

	for (size_t i = 0; i < resList.size(); ++i) {
		if (_resW == resList.at(i).w && _resH == resList.at(i).h) {
			_resN = i;
			resFound = true;
			break;
		}
	}

	if (!resFound) {
		_resW = resList.at(0).w;
		_resH = resList.at(0).h;
		_resN = 0;
	}

	_fullscreen = (p[0] == 0) ? 0 : 1;
	_language = p[1];
	_controls = (p[2] == GAMEPAD_CTRLS) ? GAMEPAD_CTRLS : KEYBOARD_CTRLS;
	_vibra = (p[3] == 0) ? 0 : 1;
	_display = (p[4] > _screenCount - 1) ? 0 : p[4];
	p += 5;

#define GETKEY(var,def) \
	var=p[0]; \
	p+=4; \
	if (isIgnoredKey(var)) { var=def; } \
	v.push_back(var);

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

	std::sort(v.begin(), v.end());
	
	if (std::unique(v.begin(), v.end()) != v.end()) {
		/* duplicate keys */
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
	_resW = resList.at(_resN).w;
	_resH = resList.at(_resN).h;
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

	if (_wfopen_s(&fp, _confFile, L"wb") != 0) {
		return false;
	}

	/* magic number (20111005) */
	putc(0x9D, fp);
	putc(0xDE, fp);
	putc(0x32, fp);
	putc(0x01, fp);

	putc(static_cast<uchar>(_resW), fp);
	putc(static_cast<uchar>(_resW >> 8), fp);
	putc(static_cast<uchar>(_resH), fp);
	putc(static_cast<uchar>(_resH >> 8), fp);

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

/* get key */
uchar configuration::key(int type)
{
	switch (type)
	{
	case KEYUP:
		return _keyUp;
	case KEYDOWN:
		return _keyDown;
	case KEYLEFT:
		return _keyLeft;
	case KEYRIGHT:
		return _keyRight;
	case KEYA:
		return _keyA;
	case KEYB:
		return _keyB;
	case KEYX:
		return _keyX;
	case KEYY:
		return _keyY;
	case KEYSTART:
		return _keyStart;
	default:
		break;
	}

	return _keyUp;
}

/* set key */
void configuration::key(uchar n, int type)
{
	switch (type)
	{
	case KEYUP:
		_keyUp = n;
		break;
	case KEYDOWN:
		_keyDown = n;
		break;
	case KEYLEFT:
		_keyLeft = n;
		break;
	case KEYRIGHT:
		_keyRight = n;
		break;
	case KEYA:
		_keyA = n;
		break;
	case KEYB:
		_keyB = n;
		break;
	case KEYX:
		_keyX = n;
		break;
	case KEYY:
		_keyY = n;
		break;
	case KEYSTART:
		_keyStart = n;
		break;
	default:
		break;
	}
}

bool configuration::compareRes(res_t a, res_t b) {
	return (a.w * a.h) > (b.w * b.h);
}

bool configuration::predRes(res_t a, res_t b) {
	return (a.w == b.w && a.h == b.h);
}

void configuration::initReslist(void)
{
	DEVMODEA dm;
	char name[32];

	memset(&dm, 0, sizeof(dm));
	dm.dmSize = sizeof(dm);
	resList.erase(resList.begin(), resList.end());

	// TODO: use EnumDisplayDevices?
	_snprintf_s(name, sizeof(name) - 1, "\\\\.\\DISPLAY%d", _display + 1);

	for (int i=0; EnumDisplaySettingsA(name, i, &dm); ++i) {
		res_t res;
		res.w = static_cast<uint16_t>(dm.dmPelsWidth);
		res.h = static_cast<uint16_t>(dm.dmPelsHeight);
		_snprintf_s(res.l, sizeof(res.l) - 1, "%dx%d", res.w, res.h);
		resList.push_back(res);
	}

	std::sort(resList.begin(), resList.end(), compareRes);
	auto last = std::unique(resList.begin(), resList.end(), predRes);

	if (last != resList.end()) {
		resList.erase(last, resList.end());
	}
}

configuration::configuration(const wchar_t *filename)
{
	_confFile = filename;
	_screenCount = static_cast<uchar>(Fl::screen_count());

	if (_screenCount == 0) {
		_screenCount = 1;
	}

	initReslist();
	//res_t r = { 0, 0, "" };
	//resList.push_back(r);
}

