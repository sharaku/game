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

#ifndef _PZL_FIELD_H_
#define _PZL_FIELD_H_

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
#include <libgame/pzl/pzl_ball.hpp>

typedef struct pzl_field
{
	int32_t get_width() { return __maps.get_width(); }
	int32_t get_hight() { return __maps.get_hight(); }

	pzl_field();

	// フィールドの初期化
	// ぷよぷよ系のばあい、邪魔玉の管理のためにhightを表示フィールドの
	// 2倍用意しておくとよい
	pzl_field(int16_t width, int16_t hight);
	int initialize(int16_t width, int16_t hight);

	// 情報をリフレッシュする。
	void refresh(void);

	// rawレベルの玉構造体を取得する。
	// フィールドの周りにある、空白玉, 破壊不可オブジェクトも含めて取得する。
	pzl_ball_t* get_fixed_raw(int16_t x, int16_t y);

	// フィールド内の指定座標の玉構造体を取得する。
	pzl_ball_t* get_fixed(int16_t x, int16_t y);

	// フィールド内の指定座標に玉を設定する
	pzl_ball_t* set_fixed(pzl_object_type_t type, int16_t x, int16_t y);

	// 連結カウント数のアップデートを行う
	void __upd_chaincnt(int16_t x, int16_t y, int ecnt);
	void upd_chaincnt(int ecnt);

	// -----------------------------------------------------
	// float玉
	// -----------------------------------------------------
	// float玉を追加する。
	void add_floatball(pzl_object_type_t type, int16_t x, int16_t y);

	// float玉を取得する。
	//  fballpがNULLの場合はxで示される列の先頭を取得する。
	//  fballpがNULL以外の場合はfballpの次を応答する。
	//  戻り値がNULLの場合は、該当のfloat玉がない。
	pzl_ball_t* get_floatball(pzl_ball_t *fballp, uint8_t x);

	// 消滅後の落下させる場合に使用する。固定玉をfloat玉へ変換する。
	void fixed2float(void);

	// float玉を固定玉へ変換する。
	void float2fixed(void);

	// float玉の更新を行う。更新を行った数を応答する。
	// 戻り値が0の場合はfloat玉がすべて落ち切ったということである。
	int upd_floatballs(float drop_speed);

	// 固定玉のうち、指定数以上連結されているものを削除する。
	int erase(void);
	
	// 指定座標に移動可能であるかをチェックする。
	// チェックは固定玉のみ。よって、float2fixed()で
	// 確定させたのちチェックすること。
	int is_moveable(int16_t raw_x, int16_t raw_y);
protected:
	obj_maps<pzl_ball>	__maps;
	obj_pool<pzl_ball>	__pool;
	hlist_head_t		*__floatballsp;	// float玉管理領域
} pzl_field_t;

#endif // _PZL_FIELD_H_
