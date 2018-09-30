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


#ifndef _CHES_FIELD_H_
#define _CHES_FIELD_H_

#ifdef __cplusplus
	#ifndef CPP_SRC
		#define CPP_SRC(x) x
	#endif
#else
	#ifndef CPP_SRC
		#define CPP_SRC(x)
	#endif
#endif

#include <libgame/wslg/wslg_field.hpp>
#include <libgame/wslg/wslg_move_map.hpp>

typedef wslg_move_map<8, 8>	chess_move_map_t;

typedef struct chess_field : public wslg_field
{
	struct chess_piece : public wslg_piece {
		enum piece_type {
			CHES_TYPE_KING,
			CHES_TYPE_QUEEN,
			CHES_TYPE_ROOK,
			CHES_TYPE_BISHOP,
			CHES_TYPE_KNIGHT,
			CHES_TYPE_PAWN,
			CHES_TYPE_MAX,
		} type;
		enum piece_group {
			CHES_GROUP_BLACK,
			CHES_GROUP_WHITE,
			CHES_GROUP_MAX,
		} group;

		chess_piece() {
			init_list_head(&list);
			type = CHES_TYPE_PAWN;
			group = CHES_GROUP_BLACK;
		}

		void set(piece_group _group, piece_type _type) {
			group = _group;
			type = _type;
		}
	};

	int initialize(void);
	int init_placement(void);
	int get_mvmap(chess_move_map_t *mvmapp, int16_t x, int16_t y);

protected:
	int __get_mvmap(chess_piece::piece_type type, chess_move_map_t *mvmapp,
			int16_t x, int16_t y, chess_piece::piece_group grp, int useful = 0);
	void __mvmap_king(chess_move_map_t *mvmapp, int16_t x, int16_t y, chess_piece::piece_group grp, int useful);
	void __mvmap_queen(chess_move_map_t *mvmapp, int16_t x, int16_t y, chess_piece::piece_group grp);
	void __mvmap_rook(chess_move_map_t *mvmapp, int16_t x, int16_t y, chess_piece::piece_group grp);
	void __mvmap_bishop(chess_move_map_t *mvmapp, int16_t x, int16_t y, chess_piece::piece_group grp);
	void __mvmap_knight(chess_move_map_t *mvmapp, int16_t x, int16_t y, chess_piece::piece_group grp);
	void __mvmap_pawn(chess_move_map_t *mvmapp, int16_t x, int16_t y, chess_piece::piece_group grp, int useful);
	void __set_enemy_ally(chess_move_map_t *mvmapp, chess_piece::piece_group grp);
	void __clear_enemy_ally(chess_move_map_t *mvmapp);
	int __take_or_move(chess_move_map_t *mvmapp, int16_t x, int16_t y);

	chess_piece	__piece[8 * 4];
} chess_field_t;

#endif // _CHES_FIELD_H_
