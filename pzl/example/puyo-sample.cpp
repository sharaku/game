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
#include <libgame/pzl/pzl_field.hpp>
#include <libgame/pzl/pzl_actball.hpp>

class puyo_sample : public tui {
public:
	puyo_sample();

protected:
	enum {
		DPZL_STAT_READY,
		DPZL_STAT_DO_ACTION,
		DPZL_STAT_DO_FREEDROP,
		DPZL_STAT_GAMEOVER,
	} __pzl_status;
	int __skip_frame;
	int __fall_frame;
	int __chains;
	int __score;

protected:
	int initialize(void);
	int update(int c);
	int disp(void);
	int finalize(void);

protected:
	int get_ball_type2char(uint16_t type);
	void on_stat_ready(int c);
	int actball(int c);
	void on_stat_do_action(int c);
	void on_stat_do_freedrop(int c);
	void on_stat_game(int c);

protected:
	pzl_field_t	__field;
	pzl_actball_t	__actball;
} ui;

int
main(void)
{
	return ui.main(1000 / 60);
}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------

puyo_sample::puyo_sample()
{
	__pzl_status = DPZL_STAT_READY;
	__skip_frame = 0;
	__fall_frame = 0;
	__chains = 0;
	__score = 0;
}

int
puyo_sample::get_ball_type2char(uint16_t type)
{
	if (type == 0) {
		return ' ';
	} else if (type == 0x10) {
		return 'O';
	} else if (type < 0x10) {
		return 'a' + type - 1;
	} else {
		return 'A' + (type & ~pzl_ball::PZL_TYPE_HARDNESS_MASK) - 1;
	}
	return ' ';
}


// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
void
puyo_sample::on_stat_ready(int c)
{
	__score = 0;
	__actball.set((rand() & 0x03) + 1, 2, 2);
	__actball.set((rand() & 0x03) + 1, 2, 3);
	__actball.set_pos(3, 13);
	__pzl_status = DPZL_STAT_DO_ACTION;
}

int
puyo_sample::actball(int c)
{
	int turn = 0;
	int ret = 0;

	switch (c) {
	case 'w':
		__actball.move(0, 1);
		break;
	case 'a':
		__actball.move(-1, 0);
		break;
	case 's':
		ret = __actball.move(0, -1);
		if (ret) {
			__actball.actball2floatball();
			ret = 1;
		}
		break;
	case 'd':
		__actball.move(1, 0);
		break;
	case ' ':
		turn = __actball.get_turn(pzl_actball::PZL_TURN_LEFT);
		__actball.fixturn(pzl_actball::PZL_TURN_LEFT, turn);
		break;
	}

	return ret;
}

void
puyo_sample::on_stat_do_action(int c)
{
	int ret;

	__chains = 0;
	ret = actball(c);
	if (ret) {
		__pzl_status = DPZL_STAT_DO_FREEDROP;
	}
	__fall_frame--;
	if (__fall_frame < 0) {
		__fall_frame = 120;
		ret = __actball.move(0, -1);
		if (ret) {
			__actball.actball2floatball();
			__pzl_status = DPZL_STAT_DO_FREEDROP;
		}
	}
}

void
puyo_sample::on_stat_do_freedrop(int c)
{
	int ret;
	int drop_cnt = 0, erase_cnt = 0;

	__field.upd_chaincnt(4);

	drop_cnt = __field.upd_floatballs(0.5);
	if (!drop_cnt) {
		__field.float2fixed();
		__field.upd_chaincnt(4);
		erase_cnt = __field.erase();
		if (erase_cnt) {
			__chains++;
			__score += erase_cnt * __chains;
			__skip_frame = 30;
			__field.fixed2float();
		} else {
			__actball.set((rand() & 0x03) + 1, 2, 2);
			__actball.set((rand() & 0x03) + 1, 2, 3);
			__actball.set_pos(3, 13);

			__pzl_status = DPZL_STAT_DO_ACTION;
			ret = __actball.move(0, 0);
			if (ret) {
				__pzl_status = DPZL_STAT_GAMEOVER;
			}
		}
	}
}

void
puyo_sample::on_stat_game(int c)
{
	printf("GAME OVER!\n");
	exit(0);
}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
int
puyo_sample::initialize(void)
{
	__field.initialize(8, 24);
	__actball.initialize(&__field, 5, 5);
	return 0;
}

int
puyo_sample::update(int c)
{
	if (__skip_frame) {
		__skip_frame--;
		return 0;
	}

	switch (__pzl_status) {
	case DPZL_STAT_READY:
		on_stat_ready(c);
		break;

	case DPZL_STAT_DO_ACTION:
		on_stat_do_action(c);
		break;

	case DPZL_STAT_DO_FREEDROP:
		on_stat_do_freedrop(c);
		break;

	case DPZL_STAT_GAMEOVER:
		on_stat_game(c);
		break;
	}
	return 0;
}

int
puyo_sample::disp(void)
{
	uint8_t _field[13][6] = {0};
	uint8_t _chains[13][6] = {0};

	int32_t		x, y, d;
	struct pzl_ball *ballp;

	for (x = 0; x < 6; x++) {
		// fixed玉を表示する。
		for (y = 0; y < 13; y++) {
			ballp = __field.get_fixed(x, 12 - y);
			_chains[y][x] = ballp->chain_cnt;
			_field[y][x] = get_ball_type2char(ballp->type);
		}

		// float玉を表示する。
		ballp = __field.get_floatball(NULL, x);
		while (ballp) {
			y = ballp->y;
			if (y < 13) {
				_field[12 - y][x] = get_ball_type2char(ballp->type);
			} else {
				break;
			}
			ballp = __field.get_floatball(ballp, x);
		}
	}
	// 操作玉を表示する。
	for (x = 0; x < 5; x++) {
		for (y = 0; y < 5; y++) {
			ballp = __actball.get(x, y);
			if (ballp->y >= 13) {
				continue;
			}
			if (ballp->type == pzl_ball_t::PZL_TYPE_NULLBALL) {
				continue;
			}

			int tmp_x = ballp->x;
			int tmp_y = ballp->y;
			_field[12 - tmp_y][tmp_x] = get_ball_type2char(ballp->type);
		}
	}

	printf("SCORE: %07u0\n", __score);
	printf("   012345    012345\n");
	printf("  +--- --+  +chains+\n");
	for (y = 0; y < 13; y++) {
		printf("%2d|%c%c%c%c%c%c|%2d|%01d%01d%01d%01d%01d%01d|\n", 12 - y,
			_field[y][0],
			_field[y][1],
			_field[y][2],
			_field[y][3],
			_field[y][4],
			_field[y][5],
			12 - y,
			_chains[y][0],
			_chains[y][1],
			_chains[y][2],
			_chains[y][3],
			_chains[y][4],
			_chains[y][5]);

	}
	printf("  +------+  +------+\n");
	printf("\n");

	// 操作玉を表示する。
	printf("  01234\n");
	printf(" +-----+\n");
	for (y = 0; y < 5; y++) {
		printf("%d|%c%c%c%c%c|\n",y,
			get_ball_type2char(__actball.get(0, y)->type),
			get_ball_type2char(__actball.get(1, y)->type),
			get_ball_type2char(__actball.get(2, y)->type),
			get_ball_type2char(__actball.get(3, y)->type),
			get_ball_type2char(__actball.get(4, y)->type));
	}
	printf(" +-----+\n");

	return 0;
}

int
puyo_sample::finalize(void)
{
	return 0;
}

