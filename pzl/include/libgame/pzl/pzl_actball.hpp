/*-
 *
 * MIT License
 * 
 * Copyright (c) 2005, 2018 Abe Takafumi
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

#ifndef _PZL_ACTBALL_H_
#define _PZL_ACTBALL_H_

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
#include <libgame/pzl/pzl_field.hpp>

// 操作玉管理構造体
typedef struct pzl_actball
{
	pzl_actball();
	pzl_actball(pzl_field_t *fieldp, int16_t width, int16_t hight);
	int initialize(pzl_field_t *fieldp, int16_t width, int16_t hight);

	int clear(void);

	// フィールド内の指定座標の玉構造体を取得する。
	pzl_ball_t* get(int16_t x, int16_t y);
	int set(pzl_object_type_t type, int16_t x, int16_t y);
	int set_pos(float x, float y);
	void set_field(pzl_field_t *fieldp) {
		__fieldp = fieldp;
	}

	enum pzl_turn {
		PZL_TURN_LEFT	= 1,
		PZL_TURN_RIGHT	= -1
	};
	int get_turn(pzl_turn turn);


	// 操作玉を90度回転させる。+が左周り、-が右回り。
	// もし、回転した結果、移動不可であればさらに90度回転する。
	// 回転中に落下させるかは、関知しない。
	void fixturn(pzl_turn turn, int turn_cnt);

	// 操作玉を移動させる。
	int move(float vx, float vy);

	// 操作玉をfloat玉へ変換する。
	int actball2floatball(void);

protected:
	obj_maps<pzl_ball>	__maps;
	obj_maps<pzl_ball>	__maps_work;
	obj_maps<pzl_ball>	__maps_rotation_work;

	float	__x;
	float	__y;
	pzl_field_t *__fieldp;

	void __copy(obj_maps<pzl_ball> *dst, obj_maps<pzl_ball> *src);
	int __is_move(obj_maps<pzl_ball> *dst, float _x, float _y);
	int __rotation(obj_maps<pzl_ball> *dst, obj_maps<pzl_ball> *src, pzl_turn turn);
} pzl_actball_t;

#endif // _PZL_FIELD_H_
