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

// 3次元MAPに対して制御を行う。

#ifndef _WSLG_MOVE_MAP_H_
#define _WSLG_MOVE_MAP_H_

#ifdef __cplusplus
	#ifndef CPP_SRC
		#define CPP_SRC(x) x
	#endif
#else
	#ifndef CPP_SRC
		#define CPP_SRC(x)
	#endif
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libsharaku/container/list.h>
#include <libsharaku/container/obj_maps.hpp>
#include <libsharaku/pool/obj_pool.hpp>

template<uint8_t C, uint8_t R>
struct wslg_move_map
{
	enum {
		// 移動可能
		MVMAP_FLG_MOVE		= 0x01,

		// 相手と接触
		// チェスの場合は相手を取り、かつその場に移動可能を示す
		// タクティクス系であれば相手に攻撃が可能を示す
		MVMAP_FLG_ENCOUNT	= 0x02,

		// 敵グループ
		// 相手グループの駒が存在していることを示す
		MVMAP_FLG_ENEMY		= 0x40,

		// 味方グループ
		// 味方グループの駒が存在していることを示す
		MVMAP_FLG_ALLY		= 0x80,
	};
	void init(void) {
		memset(maps, 0, sizeof maps);
	}

	uint8_t get(int16_t x, int16_t y) {
		if (x < 0 || x >= C || y < 0 || y >= R) {
			return 0;
		}
		return maps[y][x];
	}
	uint8_t is(uint8_t flag, int16_t x, int16_t y) {
		if (x < 0 || x >= C || y < 0 || y >= R) {
			return 0;
		}
		return (maps[y][x] & flag) != 0;
	}
	uint8_t set(uint8_t flag, int16_t x, int16_t y) {
		if (x < 0 || x >= C || y < 0 || y >= R) {
			return 0;
		}
		return maps[y][x] |= flag;
	}
	uint8_t clear(uint8_t flag, int16_t x, int16_t y) {
		if (x < 0 || x >= C || y < 0 || y >= R) {
			return 0;
		}
		return maps[y][x] &= ~flag;
	}

	// ---------------------------------------------------------------------
	// 移動範囲ライブラリ
	// ---------------------------------------------------------------------
	struct position_2d {
		int	add_x;
		int	add_y;
	};
	int get_near_rook(int idx, int32_t *x, int32_t *y) {
		static struct position_2d ___vecter_pos[4] = {
			{ 0, -1},
			{ 0,  1},
			{-1,  0},
			{ 1,  0}
		};
		*x = ___vecter_pos[idx].add_x;
		*y = ___vecter_pos[idx].add_y;
		return idx;
	}
	int get_near_bishop(int idx, int32_t *x, int32_t *y) {
		static struct position_2d ___vecter_pos[4] = {
			{-1, -1},
			{-1,  1},
			{ 1, -1},
			{ 1,  1}
		};
		*x = ___vecter_pos[idx].add_x;
		*y = ___vecter_pos[idx].add_y;
		return idx;
	}
	int get_near8(int idx, int32_t *x, int32_t *y) {
		static struct position_2d ___vecter_pos[8] = {
			{-1,  0},
			{-1,  1},
			{ 0,  1},
			{ 1,  1},
			{ 1,  0},
			{ 1, -1},
			{ 0, -1},
			{-1, -1}
		};
		*x = ___vecter_pos[idx].add_x;
		*y = ___vecter_pos[idx].add_y;
		return idx;
	}

	uint8_t	maps[R][C];
};

#endif // _WSLG_FIELD_H_
