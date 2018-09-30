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

#include <libgame/pzl/pzl_reversi_field.hpp>
#include <stdio.h>

int
pzl_reversi_field::__check_chain(pzl_object_type_t type, int16_t raw_x, int16_t raw_y, int vx, int vy, pzl_check_serial_cb_t cb)
{
	pzl_ball_t *ballp;
	int tmp_x = raw_x, tmp_y = raw_y;

	// 設定したものが自分のTYPEになるかを確認し、可能であれば自分の
	// TYPEへ変更する。
	int flag = 0;
	for (;;) {
		tmp_x += vx;
		tmp_y += vy;
		ballp = get_fixed_raw(tmp_x, tmp_y);

		// 範囲外に出たら（NULL）ひっくり返せない。
		if (!ballp) {
			return 0;
		}

		// 空の領域にぶつかったらひっくり返せない。
		if (ballp->type == pzl_ball::PZL_TYPE_NULLBALL ||
		    ballp->type == pzl_ball::PZL_TYPE_NOTPOSSIBLE) {
			return 0;
		}

		if((ballp->type & pzl_ball::PZL_TYPE_GROUP_MASK)
		    != (type & pzl_ball::PZL_TYPE_GROUP_MASK)) {
			// 自分のグループ外であればさらにチェック
			flag = 1;
		} else {
			// 自分のグループかつ、flagが立っていたらひっくり返せる。
			if (flag) {
				goto callback;
			} else {
				// 置いた玉の隣が自身のグループであれば、置けない。
				break;
			}
		}
	}
	return 0;

callback:
	if (!cb) {
		goto out;
	}
	for (tmp_x = raw_x + vx, tmp_y = raw_y + vy;; tmp_x += vx, tmp_y += vy) {
		ballp = get_fixed_raw(tmp_x, tmp_y);
		if((ballp->type & pzl_ball::PZL_TYPE_GROUP_MASK)
		    != (type & pzl_ball::PZL_TYPE_GROUP_MASK)) {
			// 自分のグループ外であればコールバック。
			cb(ballp, type, tmp_x, tmp_y);
		} else {
			break;
		}
	}

out:
	return 1;
}

static void
__pzl_set_reverse_flag(pzl_ball_t *ballp, pzl_object_type_t type, int16_t x, int16_t y)
{
	ballp->set_flags(pzl_ball::PZL_FLG_ERASE);
}

int
pzl_reversi_field::check_chain(pzl_object_type_t type, int16_t x, int16_t y)
{
	int32_t vx, vy, i;
	int32_t raw_x = x + 1, raw_y = y + 1;
	int ret = 0;
	int out = 0;

	omap_for_each_vecter8(vx, vy, i, &__maps) {
		ret = __check_chain(type, raw_x, raw_y, vx, vy, NULL);
		if (ret) {
			out |= 1;
		}
	}
	return out;
}

int
pzl_reversi_field::upd_chain(pzl_object_type_t type, int16_t x, int16_t y)
{
	int32_t vx, vy, i;
	int32_t raw_x = x + 1, raw_y = y + 1;
	int ret = 0;
	int out = 0;

	omap_for_each_vecter8(vx, vy, i, &__maps) {
		ret = __check_chain(type, raw_x, raw_y, vx, vy, __pzl_set_reverse_flag);
		if (ret) {
			out |= 1;
		}
	}
	return out;
}

pzl_ball_t *
pzl_reversi_field::set_piece(pzl_object_type_t type, int16_t x, int16_t y)
{
	pzl_ball_t *ballp;
	int ret;

	// すでに玉があるので置けない。
	ballp = get_fixed(x, y);
	if (ballp->type != pzl_ball::PZL_TYPE_NULLBALL) {
		return NULL;
	}
	ret = check_chain(type, x, y);
	if (!ret) {
		// ひっくり返せないので置けない
		return NULL;
	}
	ballp->set_type(type);

	return ballp;
}

int
pzl_reversi_field::reverse(pzl_object_type_t group)
{
	pzl_ball_t *ballp;
	int32_t raw_x, raw_y, d;

	omap_for_each_entry(ballp, raw_x, raw_y, d, &__maps) {
		if (ballp->is_flags(pzl_ball::PZL_FLG_ERASE)) {
			// 玉の種類は変えずにtypeを変更する。
			ballp->set_group(group);
			ballp->clear_flags(pzl_ball::PZL_FLG_ERASE);
		}
	}
}
