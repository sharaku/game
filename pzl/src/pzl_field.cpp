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

#include <stdio.h>
#include <libgame/pzl/pzl_field.hpp>


pzl_field::pzl_field()
{
}

pzl_field::pzl_field(int16_t width, int16_t hight)
{
	initialize(width, hight);
}

int
pzl_field::initialize(int16_t width, int16_t hight)
{
	int ret;

	// メモリを獲得して初期化する。
	ret = __maps.initialize(width, hight, 1);
	if (ret) {
		return ret;
	}
	ret = __pool.initialize(width * hight * 1);
	if (ret) {
		return ret;
	}

	// ballpは2次元配列分の領域を割り当てる。
	// また、指定範囲の周りには破壊不可オブジェクトを格納する。
	int x, y, d, i;
	pzl_ball_t *ballp;
	omap_for_each_entry(ballp, x, y, d, &__maps) {
		ballp->clear();
	}

	omap_for_each_width(x, &__maps) {
		ballp = __maps.get_object(x, 0, 0);
		ballp->not_posible();

		ballp = __maps.get_object(x, hight - 1, 0);
		ballp->not_posible();
	}

	omap_for_each_hignt(y, &__maps) {
		ballp = __maps.get_object(0, y, 0);
		ballp->not_posible();

		ballp = __maps.get_object(width - 1, y, 0);
		ballp->not_posible();
	}

	__floatballsp = (hlist_head_t*)malloc(sizeof(hlist_head_t) * width);
	for (i = 0; i < width; i++) {
		INIT_HLIST_HEAD(&(__floatballsp[i]));
	}

	return 0;
}

// rawレベルの玉構造体を取得する。
// フィールドの周りにある、空白玉, 破壊不可オブジェクトも含めて取得する。
pzl_ball_t*
pzl_field::get_fixed_raw(int16_t x, int16_t y)
{
	return __maps.get_object(x, y, 0);
}

// フィールド内の指定座標の玉構造体を取得する。
pzl_ball_t*
pzl_field::get_fixed(int16_t x, int16_t y)
{
	return get_fixed_raw(x + 1, y + 1);
}

pzl_ball_t*
pzl_field::set_fixed(pzl_object_type_t type, int16_t x, int16_t y)
{
	pzl_ball_t *ballp;
	ballp = get_fixed(x, y);
	ballp->clear();
	ballp->set_type(type);
	return ballp;
}

void
pzl_field::refresh(void)
{
	int x, y, d;
	pzl_ball_t *ballp;

	omap_for_each_entry(ballp, x, y, d, &__maps) {
		ballp->chain_cnt = 0;
		ballp->clear_flags(pzl_ball_t::PZL_FLG_ERASE |
				   pzl_ball_t::PZL_FLG_HARDNESS_CLR);
	}
}

void
pzl_field::__upd_chaincnt(int16_t cx, int16_t cy, int ecnt)
{
	int x, y, i;
	pzl_ball_t *ballp, *ball_nearp;

	ballp = get_fixed_raw(cx, cy);

	// 空白玉, 破壊不可オブジェクトもしくは高度が0以上はスキップ
	if (ballp->type == pzl_ball_t::PZL_TYPE_NULLBALL ||
	    ballp->type == pzl_ball_t::PZL_TYPE_NOTPOSSIBLE ||
	    (ballp->type & pzl_ball_t::PZL_TYPE_HARDNESS_MASK) != 0) {
		return;
	}

	ballp->chain_cnt++;
	if (ballp->chain_cnt >= ecnt) {
		ballp->set_flags(pzl_ball_t::PZL_FLG_ERASE);
	}

	omap_for_each_near4_entry(ball_nearp, x, y, i, cx, cy, 0, &__maps) {
		if (ballp->type == ball_nearp->type &&
		    ball_nearp->chain_cnt < ballp->chain_cnt) {
			__upd_chaincnt(x, y, ecnt);
		}
	}
}

void
pzl_field::upd_chaincnt(int ecnt)
{
	int x, y, d, i, cx, cy;
	pzl_ball_t *ballp, *ball_nearp;

	// 情報をれフレッシュする
	refresh();

	// チェインの集計と、ecnt以上の場合はPZL_FLG_ERASEを設定。
	omap_for_each_entry(ballp, x, y, d, &__maps) {
		__upd_chaincnt(x, y, ecnt);
	}

	// 削除対象の周りに硬度減算フラグを設定する。
	omap_for_each_entry(ballp, cx, cy, d, &__maps) {
		if (!(ballp->is_flags(pzl_ball_t::PZL_FLG_ERASE))) {
			continue;
		}
		omap_for_each_near4_entry(ball_nearp, x, y, i, cx, cy, d, &__maps) {
			if ((ball_nearp->type & pzl_ball_t::PZL_TYPE_HARDNESS_MASK) != 0 &&
			    ball_nearp->type != pzl_ball_t::PZL_TYPE_NOTPOSSIBLE && 
			    ball_nearp->is_flags(pzl_ball_t::PZL_FLG_ERASE)) {
				ball_nearp->set_flags(pzl_ball_t::PZL_FLG_HARDNESS_CLR);
			}
		}
	}
}

// -----------------------------------------------------
// float玉
// -----------------------------------------------------
void
pzl_field::add_floatball(pzl_object_type_t type, int16_t x, int16_t y)
{
	pzl_ball_t	*new_ballp, *ballp;
	hlist_head_t	*hlistp;

	// poolから空きのfloat玉を獲得する
	new_ballp = __pool.alloc();
	if (!new_ballp) {
		return;
	}

	new_ballp->clear();
	new_ballp->set_type(type);
	new_ballp->y = y;
	new_ballp->x = x;

	hlistp = &(__floatballsp[x + 1]);
	hlist_for_each_entry(ballp, hlistp, pzl_ball, list) {
		// fballpは昇順に並んでいるので該当箇所までスキップする。
		// 指定位置を見つけたらそこに挿入する
		if (ballp->y >= y) {

			list_add(&(new_ballp->list), &(ballp->list));
			return;
		}
	}
	// 見つからなかった（最初の登録を含む）場合は、リストの末尾に登録する。
	hlist_add_tail(&(new_ballp->list), hlistp);
}

pzl_ball_t*
pzl_field::get_floatball(pzl_ball_t *fballp, uint8_t x)
{
	hlist_head_t	*hlistp = &(__floatballsp[x + 1]);
	if (fballp) {
		return hlist_next_entry_or_null(fballp, hlistp,
						pzl_ball_t, list);
	} else {
		return hlist_first_entry_or_null(fballp, hlistp,
						 pzl_ball_t, list);
	}
}

void
pzl_field::fixed2float(void)
{
	int raw_x, raw_y;
	pzl_ball_t *ballp;
	int null_flg = 0;

	omap_for_each_width(raw_x, &__maps) {
		null_flg = 0;
		omap_for_each_hignt_entry(ballp, raw_y, raw_x, 0, &__maps) {
			if (ballp->type == pzl_ball_t::PZL_TYPE_NULLBALL) {
				null_flg = 1;
				continue;
			}
			if (ballp->is_flags(pzl_ball_t::PZL_FLG_NOTMOVE)) {
				null_flg = 0;
				continue;
			}
			if (null_flg) {
				add_floatball(ballp->type, raw_x - 1, raw_y - 1);
				ballp->clear();
			}
		}
	}
}

void
pzl_field::float2fixed(void)
{
	int32_t			raw_x, y;
	hlist_head_t		*hlistp;
	pzl_ball_t		*fballp, *n, *ballp;

	// 全玉を落下させる。
	omap_for_each_width(raw_x, &__maps) {
		hlistp = &(__floatballsp[raw_x]);
		hlist_for_each_entry_safe(fballp, n, hlistp,
					  pzl_ball_t, list) {
			y = fballp->y;
			ballp = get_fixed(raw_x - 1, y);
			set_fixed(fballp->type, raw_x - 1, y);
			hlist_del_init(&(fballp->list), hlistp);
			__pool.free(fballp);
		}
	}
	return;
}

int
pzl_field::upd_floatballs(float drop_speed)
{
	int32_t			raw_x, raw_y, raw_y_prev;
	int			drop_cnt = 0;
	hlist_head_t		*hlistp;
	pzl_ball_t		*fballp, *ballp_prevp, *ballp;

	// 全玉を落下させる。
	omap_for_each_width(raw_x, &__maps) {
		hlistp = &(__floatballsp[raw_x]);
		raw_y_prev = 0;
		hlist_for_each_entry(fballp, hlistp, pzl_ball, list) {
			// fixedの座標をraw座標へ変更する。
			raw_y = fballp->y - drop_speed + 1;

			ballp = get_fixed(raw_x - 1, raw_y - 1);
			if (ballp->type == pzl_ball_t::PZL_TYPE_NULLBALL &&
			    raw_y_prev < raw_y) {
				// 移動できるなら移動させる。
				fballp->y -= drop_speed;
				drop_cnt++;
				raw_y_prev = raw_y;
			} else {
				fballp->set_flags(pzl_ball_t::PZL_FLG_FIXED);
				raw_y_prev = fballp->y + 1;
			}
		}
	}

	// 移動した玉の数を応答する。
	return drop_cnt;
}

int
pzl_field::erase(void)
{
	int32_t		raw_x, raw_y, d;
	pzl_ball_t	*ballp;
	int		erase_cnt = 0;

	// FIXED状態の玉を検索し、連結数が一定数以上のものを削除する。
	// また、隣接している玉の硬度を下げる。
	// ただし、破壊不可のブロックは硬度を変化させない。
	omap_for_each_entry(ballp, raw_x, raw_y, d, &__maps) {
		if (ballp->is_flags(pzl_ball_t::PZL_FLG_ERASE)) {
			ballp->clear();
			erase_cnt++;
			continue;
		}
		if (ballp->is_flags(pzl_ball_t::PZL_FLG_HARDNESS_CLR)) {
			ballp->dec_hardness();
			continue;
		}
	}
	return erase_cnt;
}

int
pzl_field::is_moveable(int16_t raw_x, int16_t raw_y)
{
	if (!(__maps.is_range(raw_x, raw_y, 0))) {
		return 0;
	}

	pzl_ball_t *ballp = get_fixed_raw(raw_x, raw_y);
	return (ballp->type == pzl_ball_t::PZL_TYPE_NULLBALL);
}
