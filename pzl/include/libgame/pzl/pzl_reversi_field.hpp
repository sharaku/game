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

//  ライブラリの座標系
//  ライブラリ内の座標は0がフィールドの底で、+ されるごとに上がっていく。
//  ぷよぷよ系の場合、表示時にY軸を逆転させる必要がある。

#ifndef _PZL_REVERSI_FIELD_H_
#define _PZL_REVERSI_FIELD_H_

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
#include <errno.h>
#include <libgame/pzl/pzl_field.hpp>

typedef struct pzl_reversi_field : public pzl_field
{
	int check_chain(pzl_object_type_t type, int16_t x, int16_t y);
	int upd_chain(pzl_object_type_t type, int16_t x, int16_t y);

	// フィールド内の指定座標に玉を置く。
	// ただし、既に置いてある、ひっくり返せない場合は置かない。
	// 戻り値がNULLの場合はおけなかったということ。
	pzl_ball_t* set_piece(pzl_object_type_t type, int16_t x, int16_t y);

	// フラグが立っているものをひっくり返す
	int reverse(pzl_object_type_t group);

protected:
	typedef void (*pzl_check_serial_cb_t)(pzl_ball_t *ballp, pzl_object_type_t type, int16_t x, int16_t y);
	int __check_chain(pzl_object_type_t type, int16_t x, int16_t y, int vx, int vy, pzl_check_serial_cb_t cb);

} pzl_reversi_field_t;

#endif // _PZL_REVERSI_FIELD_H_
