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

#include <stdio.h>
#include <libsharaku/container/obj_maps.hpp>
#include <libgame/wslg/chess_field.hpp>

int
chess_field::initialize(void)
{
	wslg_field::initialize(8, 8, 1);
	return 0;
}

// 初期配置の設定
int
chess_field::init_placement(void)
{
	int group, i;

	for (group = 0; group < chess_piece::CHES_GROUP_MAX; group++) {
		__piece[0 + group * 16].set((chess_piece::piece_group)group,
						chess_piece::CHES_TYPE_ROOK);
		__piece[1 + group * 16].set((chess_piece::piece_group)group,
						chess_piece::CHES_TYPE_KNIGHT);
		__piece[2 + group * 16].set((chess_piece::piece_group)group,
						chess_piece::CHES_TYPE_BISHOP);
		__piece[3 + group * 16].set((chess_piece::piece_group)group,
						chess_piece::CHES_TYPE_KING);
		__piece[4 + group * 16].set((chess_piece::piece_group)group,
						chess_piece::CHES_TYPE_QUEEN);
		__piece[5 + group * 16].set((chess_piece::piece_group)group,
						chess_piece::CHES_TYPE_BISHOP);
		__piece[6 + group * 16].set((chess_piece::piece_group)group,
						chess_piece::CHES_TYPE_KNIGHT);
		__piece[7 + group * 16].set((chess_piece::piece_group)group,
						chess_piece::CHES_TYPE_ROOK);
		for (i = 0; i < 8; i++) {
			__piece[8 + i + group * 16]
					.set((chess_piece::piece_group)group,
					     chess_piece::CHES_TYPE_PAWN);
		}
	}

	// 駒を置く
	// 白
	for (i = 0; i < 8; i++) {
		set_piece(&(__piece[i + 0]), i, 0, 0);
	}
	for (i = 0; i < 8; i++) {
		set_piece(&(__piece[i + 8]), i, 1, 0);
	}

	// 黒
	for (i = 0; i < 8; i++) {
		set_piece(&(__piece[i + 24]), i, 6, 0);
	}
	for (i = 0; i < 8; i++) {
		set_piece(&(__piece[i + 16]), i, 7, 0);
	}
	return 0;
}

// ---------------------------------------------------------------------
// 各コマの移動範囲を取得する。
// ---------------------------------------------------------------------
void
chess_field::__mvmap_king(chess_move_map_t *mvmapp, int16_t x, int16_t y,
			  chess_piece::piece_group grp, int useful)
{
	int	vx, vy, i;
	int	tmpx, tmpy;

	for (i = 0; i < 8; i++) {
		mvmapp->get_near8(i, &vx, &vy);
		tmpx = x + vx;
		tmpy = y + vy;

		// 移動もしくは駒取得範囲を作成
		__take_or_move(mvmapp, tmpx, tmpy);
	}

	if (useful) {
		return;
	}

	// 敵の効果範囲を取得、その範囲を作成した移動範囲から削除する。
	chess_piece::piece_group enemy_grp;
	chess_move_map_t enemy_useful;
	int32_t column, row;
	enemy_useful.init();
	if (grp == chess_piece::CHES_GROUP_BLACK) {
		enemy_grp = chess_piece::CHES_GROUP_WHITE;
	} else {
		enemy_grp = chess_piece::CHES_GROUP_BLACK;
	}

	wslg_field::wslg_cell	*cellp;
	chess_piece		*piecep;
	for (column = 0; column < 8; column++) {
		for (row = 0; row < 8; row++) {
			cellp  = get_cell(column, row, 0);
			piecep = (chess_piece*)get_piece(cellp, NULL);

			// 指定されたgrpと異なる場合は効果範囲を作る。
			if (!piecep) {
				continue;
			}
			if (piecep->group == enemy_grp) {
				__get_mvmap(piecep->type, &enemy_useful,
					    column, row, piecep->group, 1);
			}
		}
	}

	for (column = 0; column < 8; column++) {
		for (row = 0; row < 8; row++) {
			if (enemy_useful.is(chess_move_map_t::MVMAP_FLG_ENCOUNT |
					    chess_move_map_t::MVMAP_FLG_MOVE,
					    column, row)) {
				mvmapp->clear(chess_move_map_t::MVMAP_FLG_ENCOUNT |
						chess_move_map_t::MVMAP_FLG_MOVE,
						column, row);
			}
		}
	}
}

void
chess_field::__mvmap_queen(chess_move_map_t *mvmapp, int16_t x, int16_t y,
			   chess_piece::piece_group grp)
{
	int	vx, vy, i;
	int	tmpx, tmpy;
	int	ret;

	for (i = 0; i < 8; i++) {
		mvmapp->get_near8(i, &vx, &vy);
		tmpx = x;
		tmpy = y;
		for (tmpx += vx, tmpy += vy;
		     tmpx >= 0 && tmpx < 8 && tmpy >= 0 && tmpy < 8;
		     tmpx += vx, tmpy += vy) {
			ret = __take_or_move(mvmapp, tmpx, tmpy);
			if (ret) {
				break;
			}
		}
	}
}

void
chess_field::__mvmap_rook(chess_move_map_t *mvmapp, int16_t x, int16_t y,
			  chess_piece::piece_group grp)
{
	int	vx, vy, i;
	int	tmpx, tmpy;
	int	ret;

	for (i = 0; i < 4; i++) {
		mvmapp->get_near_rook(i, &vx, &vy);
		tmpx = x;
		tmpy = y;
		for (tmpx += vx, tmpy += vy;
		     tmpx >= 0 && tmpx < 8 && tmpy >= 0 && tmpy < 8;
		     tmpx += vx, tmpy += vy) {
			ret = __take_or_move(mvmapp, tmpx, tmpy);
			if (ret) {
				break;
			}
		}
	}
}

void
chess_field::__mvmap_bishop(chess_move_map_t *mvmapp, int16_t x, int16_t y,
			    chess_piece::piece_group grp)
{
	int	vx, vy, i;
	int	tmpx, tmpy;
	int	ret;

	for (i = 0; i < 4; i++) {
		mvmapp->get_near_bishop(i, &vx, &vy);
		tmpx = x;
		tmpy = y;
		for (tmpx += vx, tmpy += vy;
		     tmpx >= 0 && tmpx < 8 && tmpy >= 0 && tmpy < 8;
		     tmpx += vx, tmpy += vy) {
			ret = __take_or_move(mvmapp, tmpx, tmpy);
			if (ret) {
				break;
			}
		}
	}
}

void
chess_field::__mvmap_knight(chess_move_map_t *mvmapp, int16_t x, int16_t y,
			    chess_piece::piece_group grp)
{
	static chess_move_map_t::position_2d	___vecter_pos[8] = {
		{+1, -2},
		{+2, -1},
		{+2, +1},
		{+1, +2},
		{-1, -2},
		{-2, -1},
		{-2, +1},
		{-1, +2}
	};
	int	vx, vy, i;
	int	tmpx, tmpy;

	for (i = 0; i < 8; i++) {
		vx = ___vecter_pos[i].add_x;
		vy = ___vecter_pos[i].add_y;
		tmpx = x + vx;
		tmpy = y + vy;
		__take_or_move(mvmapp, tmpx, tmpy);
	}
}

void
chess_field::__mvmap_pawn(chess_move_map_t *mvmapp, int16_t x, int16_t y,
			  chess_piece::piece_group grp, int useful)
{
	int vy;
	int cnt = 1;
	int init_y;

	if (grp == chess_piece::CHES_GROUP_BLACK) {
		vy = 1;
		init_y = 1;
	} else {
		vy = -1;
		init_y = 6;
	}

	if (y == init_y) {
		// 初期位置の場合、2マス移動可能
		cnt = 2;
	}

	if (useful) {
		// 最大効果範囲を取得する場合は、無条件に斜めを設定する。
		mvmapp->set(chess_move_map_t::MVMAP_FLG_ENCOUNT,
			    x + 1, y + vy);
		mvmapp->set(chess_move_map_t::MVMAP_FLG_ENCOUNT,
			    x - 1, y + vy);
	} else {
		// 移動先に駒がいる場合は移動できない。（敵もしくは味方）
		// 何もないときは移動可能
		int i;
		for (i = 1; i < cnt + 1; i++) {
			if (!mvmapp->is(chess_move_map_t::MVMAP_FLG_ALLY |
			                chess_move_map_t::MVMAP_FLG_ENEMY,
				        x, y + (vy * i))) {
				mvmapp->set(chess_move_map_t::MVMAP_FLG_MOVE,
					    x, y + (vy * i));
			}
		}

		// 敵駒が現在地の斜め前にある場合は取れる。
		if (mvmapp->is(chess_move_map_t::MVMAP_FLG_ENEMY,
			       x + 1, y + vy)) {
			mvmapp->set(chess_move_map_t::MVMAP_FLG_ENCOUNT,
				    x + 1, y + vy);
		}
		if (mvmapp->is(chess_move_map_t::MVMAP_FLG_ENEMY,
			       x - 1, y + vy)) {
			mvmapp->set(chess_move_map_t::MVMAP_FLG_ENCOUNT,
				    x - 1, y + vy);
		}
	}
}

int
chess_field::get_mvmap(chess_move_map_t *mvmapp, int16_t x, int16_t y)
{
	mvmapp->init();

	wslg_field::wslg_cell	*cellp;
	chess_piece		*piecep;
	cellp  = get_cell(x, y, 0);
	piecep = (chess_piece*)get_piece(cellp, NULL);

	if (!piecep) {
		return 0;
	}

	return __get_mvmap(piecep->type, mvmapp, x, y, piecep->group);
}

int
chess_field::__get_mvmap(chess_piece::piece_type type, chess_move_map_t *mvmapp,
			 int16_t x, int16_t y, chess_piece::piece_group grp,
			 int useful)
{
	__set_enemy_ally(mvmapp, grp);

	switch (type) {
	case chess_piece::CHES_TYPE_KING:
		__mvmap_king(mvmapp, x, y, grp, useful);
		break;

	case chess_piece::CHES_TYPE_QUEEN:
		__mvmap_queen(mvmapp, x, y, grp);
		break;

	case chess_piece::CHES_TYPE_ROOK:
		__mvmap_rook(mvmapp, x, y, grp);
		break;

	case chess_piece::CHES_TYPE_BISHOP:
		__mvmap_bishop(mvmapp, x, y, grp);
		break;

	case chess_piece::CHES_TYPE_KNIGHT:
		__mvmap_knight(mvmapp, x, y, grp);
		break;

	case chess_piece::CHES_TYPE_PAWN:
		__mvmap_pawn(mvmapp, x, y, grp, useful);
		break;

	default:
		break;
	}

	__clear_enemy_ally(mvmapp);

	int16_t _x, _y;
	int ret = 0;
	for (_y = 0; _y < 8; _y++) {
		for (_x = 0; _x < 8; _x++) {
			if (mvmapp->is(chess_move_map_t::MVMAP_FLG_MOVE |
					chess_move_map_t::MVMAP_FLG_ENCOUNT, _x, _y)) {
				ret++;
			}
		}
	}
	return ret;
}

int
chess_field::__take_or_move(chess_move_map_t *mvmapp, int16_t x, int16_t y)
{
	int ret = 0;

	// 移動先に敵駒がある場合はとれる。
	// 移動先に味方駒がある場合は移動できない
	if (mvmapp->is(chess_move_map_t::MVMAP_FLG_ENEMY, x, y)) {
		mvmapp->set(chess_move_map_t::MVMAP_FLG_ENCOUNT, x, y);
		ret = 1;
	} else if (mvmapp->is(chess_move_map_t::MVMAP_FLG_ALLY, x, y)) {
		ret = 1;
	} else {
		mvmapp->set(chess_move_map_t::MVMAP_FLG_MOVE, x, y);
	}
	return ret;
}

void
chess_field::__set_enemy_ally(chess_move_map_t *mvmapp,
			      chess_piece::piece_group grp)
{
	int16_t column, row;
	wslg_cell	*cellp;
	chess_piece	*piecep;

	for (column = 0; column < 8; column++) {
		for (row = 0; row < 8; row++) {
			cellp  = get_cell(column, row, 0);
			piecep = (chess_piece*)get_piece(cellp, NULL);
			if (!piecep) {
				continue;
			}
			if (piecep->group == grp) {
				mvmapp->set(chess_move_map_t::MVMAP_FLG_ALLY, column, row);
			} else {
				mvmapp->set(chess_move_map_t::MVMAP_FLG_ENEMY, column, row);
			}
		}
	}
}

void
chess_field::__clear_enemy_ally(chess_move_map_t *mvmapp)
{
	int16_t column, row;
	wslg_cell	*cellp;
	chess_piece	*piecep;

	for (column = 0; column < 8; column++) {
		for (row = 0; row < 8; row++) {
			cellp  = get_cell(column, row, 0);
			piecep = (chess_piece*)get_piece(cellp, NULL);
			if (!piecep) {
				continue;
			}
			mvmapp->clear(chess_move_map_t::MVMAP_FLG_ALLY |
				      chess_move_map_t::MVMAP_FLG_ENEMY, column, row);
		}
	}
}
