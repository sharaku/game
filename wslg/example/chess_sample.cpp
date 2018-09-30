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

// g++ reversi_sample.cpp -I../include -L../libs/pzl -lgame.pzl.linux.x86

#include "tui.hpp"
#include <libsharaku/container/obj_maps.hpp>
#include <libgame/wslg/chess_field.hpp>

class chess_sample : public tui {
public:
	chess_sample();
	int16_t	__mvtgt_x;
	int16_t	__mvtgt_y;
	int16_t	__cursol_x;
	int16_t	__cursol_y;
	int16_t	__set_ball_type;
	enum {
		MODE_CURSOL,
		MODE_MOVE,
	} __mode;

protected:

protected:
	int initialize(void);
	int update(int c);
	int disp(void);
	int finalize(void);

protected:
	int __cursol(int c);
	int get_type2char(chess_field::chess_piece *piece, char *str);

protected:
	chess_field_t	__field;
} ui;

int
main(void)
{
	return ui.main(1000 / 60);
}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
chess_sample::chess_sample()
{
	__set_ball_type = chess_field::chess_piece::CHES_GROUP_WHITE;
	__mode = MODE_CURSOL;

}

int
chess_sample::get_type2char(chess_field::chess_piece *piece, char *str)
{
	switch (piece->group) {
	case chess_field::chess_piece::CHES_GROUP_BLACK:
		str[0] = 'b';
		break;

	case chess_field::chess_piece::CHES_GROUP_WHITE:
		str[0] = 'w';
		break;
	}

	switch (piece->type) {
	case chess_field::chess_piece::CHES_TYPE_KING:
		str[1] = 'K';
		break;

	case chess_field::chess_piece::CHES_TYPE_QUEEN:
		str[1] = 'Q';
		break;

	case chess_field::chess_piece::CHES_TYPE_ROOK:
		str[1] = 'R';
		break;

	case chess_field::chess_piece::CHES_TYPE_BISHOP:
		str[1] = 'B';
		break;

	case chess_field::chess_piece::CHES_TYPE_KNIGHT:
		str[1] = 'N';
		break;

	case chess_field::chess_piece::CHES_TYPE_PAWN:
		str[1] = 'P';
		break;
	}
}

int
chess_sample::__cursol(int c)
{
	chess_move_map_t mvmap;
	int ret;

	// カーソルを移動する。
	switch (c) {
	case 'w':
		__cursol_y--;
		break;
	case 'a':
		__cursol_x--;
		break;
	case 's':
		__cursol_y++;
		break;
	case 'd':
		__cursol_x++;
		break;
	}

	__cursol_x = __cursol_x & 7;
	__cursol_y = __cursol_y & 7;
	if (__mode == MODE_CURSOL) {
		__mvtgt_x = __cursol_x;
		__mvtgt_y = __cursol_y;
	}

	switch (c) {
	case ' ':	// 決定
		ret = __field.get_mvmap(&mvmap, __mvtgt_x, __mvtgt_y);
		if (__mode == MODE_CURSOL) {
			chess_field::chess_piece	*piecep;
			piecep = (chess_field::chess_piece*)__field.get_piece(__cursol_x, __cursol_y, 0);
			if (piecep) {
				if (piecep->group == __set_ball_type && ret) {
					__mode = MODE_MOVE;
				}
			}
		} else {
			if (mvmap.is(chess_move_map_t::MVMAP_FLG_MOVE |
				     chess_move_map_t::MVMAP_FLG_ENCOUNT, __cursol_x, __cursol_y)) {
				chess_field::chess_piece	*piecep;

				piecep = (chess_field::chess_piece*)__field.get_piece(__cursol_x, __cursol_y, 0);
				__field.del_piece(piecep);
				piecep = (chess_field::chess_piece*)__field.get_piece(__mvtgt_x, __mvtgt_y, 0);
				__field.del_piece(piecep);
				__field.set_piece(piecep, __cursol_x, __cursol_y, 0);

				if (__set_ball_type == chess_field::chess_piece::CHES_GROUP_WHITE) {
					__set_ball_type = chess_field::chess_piece::CHES_GROUP_BLACK;
				} else {
					__set_ball_type = chess_field::chess_piece::CHES_GROUP_WHITE;
				}
				__mode = MODE_CURSOL;
			}
		}
		break;
	case 'B':	// キャンセル
		if (__mode == MODE_MOVE) {
			__mode = MODE_CURSOL;
		}
		break;
	}
	return 0;
}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
int
chess_sample::initialize(void)
{
	__field.initialize();
#if 1
	__field.init_placement();
#else
	static chess_field::chess_piece piece_p;
	static chess_field::chess_piece piece_p_a;
	static chess_field::chess_piece piece_p_e;

	piece_p_a.set(chess_field::chess_piece::CHES_GROUP_WHITE, chess_field::chess_piece::CHES_TYPE_PAWN);
	piece_p_e.set(chess_field::chess_piece::CHES_GROUP_BLACK, chess_field::chess_piece::CHES_TYPE_PAWN);

	piece_p.set(chess_field::chess_piece::CHES_GROUP_WHITE, chess_field::chess_piece::CHES_TYPE_KING);

	__field.set_piece(&piece_p, 4, 3, 0);
	__field.set_piece(&piece_p_a, 3, 2, 0);
	__field.set_piece(&piece_p_e, 5, 1, 0);
#endif
	return 0;
}

int
chess_sample::update(int c)
{
	__cursol(c);
	return 0;
}

int
chess_sample::disp(void)
{
	char _field[8][8][4] = {0};
	int y, x, d;

	wslg_field::wslg_cell		*cellp;
	chess_field::chess_piece	*piecep;
	for (y = 0; y < 8; y++) {
		for (x = 0; x < 8; x++) {
			_field[y][x][0] = ' ';
			_field[y][x][1] = ' ';
			_field[y][x][2] = ' ';

			cellp = __field.get_cell(x, y, 0);
			piecep = (chess_field::chess_piece*)__field.get_piece(cellp, NULL);
			if (piecep) {
				get_type2char(piecep, (char*)&(_field[y][x]));
			}
		}
	}

	if (__set_ball_type == chess_field::chess_piece::CHES_GROUP_WHITE) {
		printf("type=%c x=%d y=%d\n", 'w', __cursol_x, __cursol_y);
	} else {
		printf("type=%c x=%d y=%d\n", 'b', __cursol_x, __cursol_y);
	}

	_field[__cursol_y][__cursol_x][0] = '[';
	_field[__cursol_y][__cursol_x][1] = '*';
	_field[__cursol_y][__cursol_x][2] = ']';
	_field[__cursol_y][__cursol_x][3] = '\0';

	// 移動範囲を表示する
	chess_move_map_t mvmap;
	__field.get_mvmap(&mvmap, __mvtgt_x, __mvtgt_y);
	for (y = 0; y < 8; y++) {
		for (x = 0; x < 8; x++) {
			if (mvmap.is(chess_move_map_t::MVMAP_FLG_MOVE, x, y)) {
				_field[y][x][2] = '#';
			}
			if (mvmap.is(chess_move_map_t::MVMAP_FLG_ENCOUNT, x, y)) {
				_field[y][x][2] = '$';
			}
		}
	}

	printf("    a   b   c   d   e   f   g   h\n");
	printf("  +---+---+---+---+---+---+---+---+\n");
	for (y = 0; y < 8; y++) {
		printf("%2d|%-3s|%-3s|%-3s|%-3s|%-3s|%-3s|%-3s|%-3s|\n",
			y,
			_field[y][0],
			_field[y][1],
			_field[y][2],
			_field[y][3],
			_field[y][4],
			_field[y][5],
			_field[y][6],
			_field[y][7]);
		printf("  +---+---+---+---+---+---+---+---+\n");
	}
	printf("\n");

	return 0;
}

int
chess_sample::finalize(void)
{
	return 0;
}

