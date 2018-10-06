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

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

class tui
{
public:
	tui() {
		__initialize();
	}
	virtual ~tui() {
		__finalize();
	}

	int main(long delay_ms) {
		int x, y, ret;
		int key;

		ret = initialize();
		if (ret) {
			return -1;
		}
		while (1) {
			key = __input();
			ret = update(key);
			if (ret < 0) {
				finalize();
				return -1;
			}

			printf("\033[%d;%dH", 0, 0);
			ret = disp();
			if (ret) {
				break;
			}

			usleep(delay_ms * 1000);
		}
		finalize();
		return 0;
	}

protected:
	virtual int initialize(void) { return -1; }
	virtual int update(int c) { return -1; }
	virtual int disp(void) { return -1; }
	virtual int finalize(void) { return -1; }

private:
	struct termios save_settings;
	void __initialize(void) {
		struct termios settings;

		tcgetattr(fileno(stdin), &save_settings);
		settings = save_settings;

		settings.c_lflag &= ~( ECHO | ICANON ); /* echobackしない & LFを待たない */
		tcsetattr(fileno(stdin), TCSANOW, &settings);
		fcntl(fileno(stdin), F_SETFL, O_NONBLOCK); /* non blocking */
	}
	void __finalize(void) {
		tcsetattr(fileno(stdin), TCSANOW, &save_settings);
	}

	int __input(void) {
		int c, ret;
		ret = getchar();
		for (;;) {
			c = getchar();
			if (c != EOF) {
				break;
			} else {
				if (errno != EAGAIN) {
					return -1;
				}
				break;
			}

			if (c == 0x04) {
				return -1;
			}
		}

		return ret;
	}
};

void
tui_locate(int x, int y) {
	printf("\033[%d;%dH", y, x);
}


