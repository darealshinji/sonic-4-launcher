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

typedef unsigned char uchar;

typedef struct {
	uint16_t w;
	uint16_t h;
	const char *l;
} res_t;


class configuration
{
public:
	enum {
		KEYBOARD_CTRLS = 0,
		GAMEPAD_CTRLS = 1
	};

	/* hardcoded resolution list */
	static const res_t resList[12];

private:
	const wchar_t *_confFile;
	uchar _screenCount;
	size_t _resN;

	uint16_t _resX;
	uint16_t _resY;

	uchar _fullscreen;
	uchar _language;
	uchar _controls;
	uchar _vibra;
	uchar _display;
	uchar _keyLeft;
	uchar _keyRight;
	uchar _keyUp;
	uchar _keyDown;
	uchar _keyA;
	uchar _keyB;
	uchar _keyX;
	uchar _keyY;
	uchar _keyStart;

public:
	configuration(const wchar_t *filename);

	static bool isIgnoredKey(uchar dx);

	bool loadConfig();
	void setDefaultKeys();
	void loadDefaultConfig();
	bool saveConfig();

	uchar screenCount() { return _screenCount; }
	uint16_t resXN(int n);
	uint16_t resYN(int n);
	void resN(size_t n);
	size_t resN() { return _resN; }

	/* get config values */
	uint16_t resX()      { return _resX; }
	uint16_t resY()      { return _resY; }
	uchar fullscreen() { return _fullscreen; }
	uchar language()   { return _language; }
	uchar controls()   { return _controls; }
	uchar vibra()      { return _vibra; }
	uchar display()    { return _display; }
	uchar keyLeft()   { return _keyLeft; }
	uchar keyRight()  { return _keyRight; }
	uchar keyUp()     { return _keyUp; }
	uchar keyDown()   { return _keyDown; }
	uchar keyA()      { return _keyA; }
	uchar keyB()      { return _keyB; }
	uchar keyX()      { return _keyX; }
	uchar keyY()      { return _keyY; }
	uchar keyStart()  { return _keyStart; }

	/* set config values */
	void resX(uint16_t n)      { _resX = n; }
	void resY(uint16_t n)      { _resY = n; }
	void fullscreen(uchar n) { _fullscreen = n; }
	void language(uchar n)   { _language = n; }
	void controls(uchar n)   { _controls = n; }
	void vibra(uchar n)      { _vibra = n; }
	void display(uchar n)    { _display = n; }
	void keyLeft(uchar n)   { _keyLeft = n; }
	void keyRight(uchar n)  { _keyRight = n; }
	void keyUp(uchar n)     { _keyUp = n; }
	void keyDown(uchar n)   { _keyDown = n; }
	void keyA(uchar n)      { _keyA = n; }
	void keyB(uchar n)      { _keyB = n; }
	void keyX(uchar n)      { _keyX = n; }
	void keyY(uchar n)      { _keyY = n; }
	void keyStart(uchar n)  { _keyStart = n; }
};

