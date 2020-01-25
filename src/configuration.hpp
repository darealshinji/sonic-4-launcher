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

#include <stdint.h>
#include <wchar.h>

#define SZRESLIST 12

#define KEYBOARD_CTRLS 0
#define GAMEPAD_CTRLS 1

#define KEYUP 1
#define KEYDOWN 2
#define KEYLEFT 3
#define KEYRIGHT 4
#define KEYA 5
#define KEYB 6
#define KEYX 7
#define KEYY 8
#define KEYSTART 9

typedef unsigned char uchar;

typedef struct {
	uint16_t w;
	uint16_t h;
	const char *l;
} res_t;


class configuration
{
public:
	/* hardcoded resolution list */
	static const res_t resList[SZRESLIST];

private:
	const wchar_t *_confFile = NULL;

	uchar _screenCount = 0;
	size_t _resN = 0;
	uint16_t _resW = 0;
	uint16_t _resH = 0;

	uchar _fullscreen = 0;
	uchar _language = 0;
	uchar _controls = 0;
	uchar _vibra = 0;
	uchar _display = 0;
	uchar _keyLeft = 0;
	uchar _keyRight = 0;
	uchar _keyUp = 0;
	uchar _keyDown = 0;
	uchar _keyA = 0;
	uchar _keyB = 0;
	uchar _keyX = 0;
	uchar _keyY = 0;
	uchar _keyStart = 0;

public:
	configuration(const wchar_t *filename);

	bool loadConfig();
	void setDefaultKeys();
	void loadDefaultConfig();
	bool saveConfig();

	uchar screenCount() { return _screenCount; }
	static bool isIgnoredKey(uchar dx);
	static const char *getReslistL(int n);

	/* get config values */
	size_t resN()      { return _resN; }
	uint16_t resW()    { return _resW; }
	uint16_t resH()    { return _resH; }
	uchar fullscreen() { return _fullscreen; }
	uchar language()   { return _language; }
	uchar controls()   { return _controls; }
	uchar vibra()      { return _vibra; }
	uchar display()    { return _display; }
	uchar key(int type);

	/* set config values */
	void resN(size_t n);
	void resW(uint16_t n)    { _resW = n; }
	void resH(uint16_t n)    { _resH = n; }
	void fullscreen(uchar n) { _fullscreen = n; }
	void language(uchar n)   { _language = n; }
	void controls(uchar n)   { _controls = n; }
	void vibra(uchar n)      { _vibra = n; }
	void display(uchar n)    { _display = n; }
	void key(uchar n, int type);
};

