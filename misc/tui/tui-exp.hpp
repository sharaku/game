/*-
 *
 * MIT License
 * 
 * Copyright (c) 2018 Abe Takafumi
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
 * SOFTWARE. *
 *
 */

#include "tui.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

template<int W, int H>
class tui_exp : public tui
{
public:
	tui_exp() {
		::printf("\033[2J");
	}

	int gprint(int x, int y, uint64_t *g, int width) {
		int i, cnt;
		for (i = x, cnt = 0; (i < x + width) && (i < W); i++, cnt++) {
			__bmp[i] |= g[cnt] << y;
		}
		return 0;
	}
	int gclear(void) {
		memset(__bmp, 0, sizeof __bmp);
		return 0;
	}
	int printf(int x, int y, const char *fmt, ...) {
		va_list ap;

		va_start(ap, fmt);
		vsnprintf(&(__text[y][x]), W - x, fmt, ap);
		va_end(ap);
		return 0;
	}
	int clear(void) {
		memset(__text, 0, sizeof __text);
		return 0;
	}

protected:
	virtual int disp(void) { return __disp(); }
	virtual int __disp(void) {
		::printf("\033[%d;%dH", 0, 0);
		int y;
		int x;

		char temp[H][W + 1];
		memset(temp, 0, sizeof temp);
		for (x = 0; x < W; x++) {
			for (y = 0; y < H; y++) {
				if (__bmp[x] & (1UL << y)) {
					temp[y][x] = '#';
				} else {
					temp[y][x] = ' ';
				}
			}
		}

		for (y = 0; y < H; y++) {
			for (x = 0; x < W; x++) {
				if (__text[y][x]) {
					temp[y][x] = __text[y][x];
				}
			}
			::printf("%s\n", &(temp[y][0]));
		}
		fflush(stdout);
		return 0;
	}

protected:
	// \0も含めて +1する。
	char __text[H][W + 1];
	uint64_t __bmp[W];
};

