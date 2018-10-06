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

#include "tui-exp.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

class tuiexp_sample : public tui_exp<128, 48>
{
public:
	uint64_t cnt;

protected:
	int initialize(void);
	int update(int c);
	int finalize(void);
} ui;

int
main(void)
{
	return ui.main(1000 / 15);
}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
int
tuiexp_sample::initialize(void)
{
	ui.cnt = 0;
	return 0;
}

int
tuiexp_sample::update(int c)
{
	ui.clear();
	ui.gclear();
	ui.printf(3, 3, "test=%lu", ui.cnt);
	ui.printf(3, 4, "time=%lu", time(NULL));
	ui.cnt++;

	const uint64_t bmp[16] = { 0x0B, 0x0B, 0x16, 0x16, 0x0C, 0x0C, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 };
	const uint64_t bmp_line[16] = { 0xFFFFFFFFFFFFFFFF };
	ui.gprint(10, 10, (uint64_t*)bmp, 16);
	ui.gprint(127, 0, (uint64_t*)bmp_line, 1);
	ui.printf(0, 47, "################################################################################################################################");
	return 0;
}

int
tuiexp_sample::finalize(void)
{
	return 0;
}

