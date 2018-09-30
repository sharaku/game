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

#ifndef _PZL_BALL_H_
#define _PZL_BALL_H_

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
#include <libsharaku/container/list.h>

typedef uint16_t pzl_object_type_t;

// 玉の管理
//  玉は FIXED 状態の玉である。位置が固定されており、移動しないものを指す。
//  玉は2次元配列で管理する。
typedef struct pzl_ball
{
	enum pzl_ball_type {
		// 空白玉
		PZL_TYPE_NULLBALL	= 0x0000,
		// 破壊不可オブジェクト
		PZL_TYPE_NOTPOSSIBLE	= 0xFFFF,

		// 硬度マスク
		// 邪魔玉の硬度を示す。
		// ぷよぷよのお邪魔ぷよは0x10のように空(0x0000)の
		// 硬度 1を指定することで表現可能。
		PZL_TYPE_GROUP_MASK	= 0xF000,
		PZL_TYPE_HARDNESS_MASK	= 0x00F0,
		PZL_TYPE_BALL_MASK	= 0x000F,

		// 硬度の基本値
		DPZL_HARDNESS_NUM	= 0x0010,
	};
	enum pzl_ball_flags {
		PZL_FLG_ERASE		= 0x01,
		PZL_FLG_HARDNESS_CLR	= 0x02,
		PZL_FLG_NOTMOVE		= 0x04,
		// これ以上落ちない状態
		PZL_FLG_FIXED		= 0x08,
	};

	list_head_t		list;
	pzl_object_type_t	type;
	uint8_t			chain_cnt;
	uint8_t			flags;
	float			x;
	float			y;

	// NULL玉の設定
	void clear(void) {
		init_list_head(&list);
		type = PZL_TYPE_NULLBALL;
		chain_cnt = 0;
		flags = 0;
		x = 0.0f;
		y = 0.0f;
	}

	// 破壊・移動不可玉の設定
	void not_posible(void) {
		init_list_head(&list);
		type = PZL_TYPE_NOTPOSSIBLE;
		flags = PZL_FLG_NOTMOVE;
		chain_cnt = 0;
		x = 0.0f;
		y = 0.0f;
	}

	// typeの設定
	void set_type(pzl_object_type_t t) {
		type = t;
	}

	// flag設定
	void set_flags(uint8_t f) {
		flags |= f;
	}
	void clear_flags(uint8_t f) {
		flags &= ~f;
	}
	int is_flags(uint8_t f) {
		return (flags & f) != 0;
	}
	void dec_hardness(void) {
		pzl_object_type_t hardness = (type) & PZL_TYPE_HARDNESS_MASK;
		if (!hardness) {
			// 減算できない
			return;
		}
		hardness = hardness -  DPZL_HARDNESS_NUM;
		pzl_object_type_t type     = (type) & ~PZL_TYPE_HARDNESS_MASK;
		type = hardness | type;
	}
	void inc_hardness(struct pzl_object *obj) {
		pzl_object_type_t hardness = (type) & PZL_TYPE_HARDNESS_MASK;
		if (hardness == PZL_TYPE_HARDNESS_MASK) {
			// 加算できない
			return;
		}
		hardness = hardness +  DPZL_HARDNESS_NUM;
		pzl_object_type_t type     = (type) & ~PZL_TYPE_HARDNESS_MASK;
		type = hardness | type;
	}
	pzl_object_type_t get_group(void) {
		pzl_object_type_t group = (type) & PZL_TYPE_GROUP_MASK;
		return group;
	}
	void set_group(pzl_object_type_t group) {
		pzl_object_type_t new_type = (type & ~PZL_TYPE_GROUP_MASK) |
						 (group & PZL_TYPE_GROUP_MASK);
		type = new_type;
	}
} pzl_ball_t;

#endif // _PZL_FIELD_H_
