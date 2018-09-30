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
#include <libgame/pzl/pzl_actball.hpp>


pzl_actball::pzl_actball()
{
	__x = 0.0f;
	__y = 0.0f;
	__fieldp = NULL;
}

pzl_actball::pzl_actball(pzl_field_t *fieldp, int16_t width, int16_t hight)
{
	initialize(fieldp, width, hight);
}

int
pzl_actball::initialize(pzl_field_t *fieldp, int16_t width, int16_t hight)
{
	int ret;

	__x = 0.0f;
	__y = 0.0f;
	__fieldp = fieldp;

	ret = __maps.initialize(width, hight, 1);
	if (ret) {
		return ret;
	}
	ret = __maps_work.initialize(width, hight, 1);
	if (ret) {
		return ret;
	}
	ret = __maps_rotation_work.initialize(width, hight, 1);
	if (ret) {
		return ret;
	}
}

int
pzl_actball::clear(void)
{
	int w, h;
	pzl_ball *ballp;

	for (h = 0; h < __maps.get_hight(); h++) {
		for (w = 0; w < __maps.get_width(); w++) {
			ballp = __maps.get_object(w, h, 0);
			ballp->clear();
		}
	}
}

pzl_ball_t*
pzl_actball::get(int16_t x, int16_t y)
{
	int cx = __maps.get_width() / 2;
	int cy = __maps.get_hight() / 2;

	pzl_ball_t *ballp = __maps.get_object(x, y, 0);
	ballp->x = __x  - cx + x;
	ballp->y = __y  - cy + y;
	return ballp;
}

int
pzl_actball::set(pzl_object_type_t type, int16_t x, int16_t y)
{
	pzl_ball *ballp;

	ballp = __maps.get_object(x, y, 0);
	ballp->set_type(type);
	return 0;
}

int
pzl_actball::set_pos(float x, float y)
{
	__x = x;
	__y = y;
	return 0;
}

void
pzl_actball::__copy(obj_maps<pzl_ball> *dst, obj_maps<pzl_ball> *src)
{
	int w, h;
	pzl_ball *src_ball, *dst_ball;

	for (h = 0; h < src->get_hight(); h++) {
		for (w = 0; w < src->get_width(); w++) {
			src_ball = src->get_object(w, h, 0);
			dst_ball = dst->get_object(w, h, 0);
			*dst_ball = *src_ball;
		}
	}
}

int
pzl_actball::__is_move(obj_maps<pzl_ball> *mapp, float x, float y)
{
	int w, h;
	int raw_x, raw_y;
	int is_moveable;
	pzl_ball *ballp;
	int cx = mapp->get_width() / 2;
	int cy = mapp->get_hight() / 2;

	for (h = 0; h < mapp->get_hight(); h++) {
		for (w = 0; w < mapp->get_width(); w++) {
			ballp = mapp->get_object(w, h, 0);
			if (ballp->type == pzl_ball_t::PZL_TYPE_NULLBALL) {
				continue;
			}

			raw_x = x + w - cx + 1;
			raw_y = y + h - cy + 1;

			// 領域範囲内で移動できるかをチェックする。
			// フィールドの周りには破壊不可オブジェクトを配置
			// しているため、範囲外に出るかをチェックしなくてもよい。
			is_moveable = __fieldp->is_moveable(raw_x, raw_y);
			if (!is_moveable) {
				// 移動できなければ終了。
				return 0;
			}
		}
	}

	return 1;
}

int
pzl_actball::__rotation(obj_maps<pzl_ball> *dst, obj_maps<pzl_ball> *src, pzl_turn turn)
{
	int w, h;
	pzl_ball *src_ball, *dst_ball;

	if (turn == PZL_TURN_LEFT) {
		for (h = 0; h < src->get_hight(); h++) {
			for (w = 0; w < src->get_width(); w++) {
				src_ball = src->get_object(src->get_hight() - 1 - h, w, 0);
				dst_ball = dst->get_object(w, h, 0);
				*dst_ball = *src_ball;
			}
		}
	} else {
		for (h = 0; h < src->get_hight(); h++) {
			for (w = 0; w < src->get_width(); w++) {
				src_ball = src->get_object(h, src->get_width() - 1 - w, 0);
				dst_ball = dst->get_object(w, h, 0);
				*dst_ball = *src_ball;
			}
		}
	}
}

int
pzl_actball::get_turn(pzl_turn turn)
{
	int i;
	int turn_cnt;
	int is_moveable;

	// 0番のworkを使用する。
	__copy(&__maps_work, &__maps);
	turn_cnt = 0;

	// 4回回れば今の位置に来る。つまり、4回回れば必ず確定できる。
	for (i = 0; i < 4; i++) {
		turn_cnt++;
		__rotation(&__maps_rotation_work, &__maps_work, turn);
		is_moveable = __is_move(&__maps_rotation_work, __x, __y);
		if (is_moveable) {
			break;
		}
		__copy(&__maps_work, &__maps_rotation_work);
	}

	return turn_cnt;
}

void
pzl_actball::fixturn(pzl_turn turn, int turn_cnt)
{
	int i;
	for (i = 0; i < turn_cnt; i++) {
		__rotation(&__maps_rotation_work, &__maps, turn);
		__copy(&__maps, &__maps_rotation_work);
	}
}

int
pzl_actball::move(float vx, float vy)
{
	int is_moveable;

	// 0番のworkを使用する。
	__copy(&__maps_work, &__maps);

	is_moveable = __is_move(&__maps_work, __x + vx, __y + vy);
	if (is_moveable) {
		// 全てが移動可能である。
		__x = __x + vx;
		__y = __y + vy;
		return 0;
	} else {
		return -1;
	}
}

int
pzl_actball::actball2floatball(void)
{
	int w, h, x, y;
	pzl_ball *ballp;

	for (h = 0; h < __maps.get_hight(); h++) {
		for (w = 0; w < __maps.get_width(); w++) {
			ballp = get(w, h);
			if (ballp->type == pzl_ball_t::PZL_TYPE_NULLBALL) {
				continue;
			}

			__fieldp->add_floatball(ballp->type, ballp->x, ballp->y);
			set(pzl_ball_t::PZL_TYPE_NULLBALL, h, w);
		}
	}
	clear();
	return 0;
}
