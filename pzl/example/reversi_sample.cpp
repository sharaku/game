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
#include <libsharaku/container/obj_maps.hpp>
#include <libgame/pzl/pzl_reversi_field.hpp>

class reversi_sample : public tui {
public:
	reversi_sample();

protected:
	enum {
		DPZL_STAT_READY,
		DPZL_STAT_DO_ACTION,
		DPZL_STAT_GAMEOVER,
	} __pzl_status;

	int __cursol_x;
	int __cursol_y;
	pzl_object_type_t __set_ball_type;

protected:
	int initialize(void);
	int update(int c);
	int disp(void);
	int finalize(void);

protected:
	int get_ball_type2char(uint16_t type);
	int __cursol(int c);

	void on_stat_ready(int c);
	void on_stat_do_action(int c);
	void on_stat_game(int c);

protected:
	pzl_reversi_field_t	__field;
} ui;

int
main(void)
{
	return ui.main(1000 / 60);
}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
reversi_sample::reversi_sample()
{
	__pzl_status = DPZL_STAT_READY;
	__cursol_x = 0;
	__cursol_y = 0;
	__set_ball_type = 0x0000;
}

int
reversi_sample::get_ball_type2char(uint16_t type)
{
	if (type == pzl_ball::PZL_TYPE_NULLBALL) {
		return ' ';
	} else if (type & pzl_ball::PZL_TYPE_GROUP_MASK) {
		return 'X';
	} else {
		return 'O';
	}
	return ' ';
}

int
reversi_sample::__cursol(int c)
{
	pzl_ball_t *ballp;

	// カーソルを移動する。
	switch (c) {
	case 'w':
		__cursol_y--;
		break;
	case 'a':
		__cursol_x--;
		break;
	case 's':
		__cursol_y++;
		break;
	case 'd':
		__cursol_x++;
		break;
	case ' ':
		// 確定ボタン＋おける場合は、置く＆ひっくり返す
		ballp = __field.set_piece(__set_ball_type | 0x0001, __cursol_x, __cursol_y);
		if (ballp) {
			__field.upd_chain(__set_ball_type | 0x0001, __cursol_x, __cursol_y);
			__field.reverse(__set_ball_type);
			__set_ball_type ^= 0x1000;
		}
		break;
	}
	__cursol_x = __cursol_x & 7;
	__cursol_y = __cursol_y & 7;
	return 0;
}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
void
reversi_sample::on_stat_ready(int c)
{
	__pzl_status = DPZL_STAT_DO_ACTION;
}

void
reversi_sample::on_stat_do_action(int c)
{
	__cursol(c);
}

void
reversi_sample::on_stat_game(int c)
{
	printf("GAME OVER!\n");
	exit(0);
}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
int
reversi_sample::initialize(void)
{
	__cursol_x = 0;
	__cursol_y = 0;
	__set_ball_type = 0x00;
	__field.initialize(10, 10);
	__field.set_fixed(0x0001, 3, 3);
	__field.set_fixed(0x0001, 4, 4);
	__field.set_fixed(0x1001, 4, 3);
	__field.set_fixed(0x1001, 3, 4);
	return 0;
}

int
reversi_sample::update(int c)
{
	switch (__pzl_status) {
	case DPZL_STAT_READY:
		on_stat_ready(c);
		break;

	case DPZL_STAT_DO_ACTION:
		on_stat_do_action(c);
		break;

	case DPZL_STAT_GAMEOVER:
		on_stat_game(c);
		break;
	}

	return 0;
}

int
reversi_sample::disp(void)
{
	int _field[8][8] = {0};
	uint8_t _flags[8][8] = {0};
	int y, x;
	struct pzl_ball *ballp;

	// 固定玉を表示する。
	for (y = 0; y < 8; y++) {
		for (x = 0; x < 8; x++) {
			ballp = __field.get_fixed(x, y);
			_field[y][x] = get_ball_type2char(ballp->type);
			_flags[y][x] = ballp->flags;
		}
	}

	printf("type=%02x x=%d y=%d\n", __set_ball_type, __cursol_x, __cursol_y);
	_field[__cursol_y][__cursol_x] = '#';

	printf("   01234567     00 01 02 03 04 05 06 07\n");
	printf("  +--------+   +-- -- -- -- -- -- -- --+\n");
	for (y = 0; y < 8; y++) {
		printf("%2d|%c%c%c%c%c%c%c%c| %2d|%02x %02x %02x %02x %02x %02x %02x %02x|\n",
			y,
			_field[y][0],
			_field[y][1],
			_field[y][2],
			_field[y][3],
			_field[y][4],
			_field[y][5],
			_field[y][6],
			_field[y][7],
			y,
			_flags[y][0],
			_flags[y][1],
			_flags[y][2],
			_flags[y][3],
			_flags[y][4],
			_flags[y][5],
			_flags[y][6],
			_flags[y][7]);
	}
	printf("  +--------+   +-- -- -- -- -- -- -- --+\n");
	printf("\n");

	return 0;
}

int
reversi_sample::finalize(void)
{
	return 0;
}

