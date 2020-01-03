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

#include <Windows.h>

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <Dinput.h>

#include <FL/Fl.H>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>

#include "configuration.hpp"

#define CONF_SIZE        53
#define MAX_PATH_LENGTH  2048 * sizeof(wchar_t)
#define TO_UINT32(x)     static_cast<uint32_t>(((0xFF & x[0]) << 0 | (0xFF & x[1]) << 8 | (0xFF & x[2]) << 16 | (0xFF & x[3]) << 24))
#define ARRLEN(x)        (sizeof(x) / sizeof(*x))


const res_t configuration::resList[] =
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
	return (n < ARRLEN(configuration::resList)) ? resList[n].w : resList[0].w;
}

uint16_t configuration::resYN(int n)
{
	return (n < ARRLEN(configuration::resList)) ? resList[n].h : resList[0].h;
}

void configuration::resN(size_t n)
{
	if (n < ARRLEN(configuration::resList)) {
		_resX = resXN(n);
		_resY = resYN(n);
		_resN = n;
	} else {
		_resX = resXN(0);
		_resY = resYN(0);
		_resN = 0;
	}
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
	case DIK_NEXTTRACK:
	case DIK_NOCONVERT:
	case DIK_NUMLOCK:
	case DIK_PLAYPAUSE:
	case DIK_POWER:
	case DIK_PREVTRACK:
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

	for (int i = 0; i < ARRLEN(configuration::resList); ++i) {
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

#define GETKEY(var,def)  var=p[0]; p+=4; if (isIgnoredKey(var)) { var=def; }

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

