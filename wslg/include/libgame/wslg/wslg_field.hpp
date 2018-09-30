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

// 3次元MAPに対して制御を行う。

#ifndef _WSLG_FIELD_H_
#define _WSLG_FIELD_H_

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

typedef struct wslg_field
{
	struct wslg_cell {
		// セル中のピースリスト
		// 同一のセルにピースが重なった際の管理を行う。
		// 将棋やチェスの場合は同一のセルに設定できるピースは1つであるが、
		// SLGの中には同一のセルに複数のピースを設定できるものがある。
		// これを管理するための構造である。
		list_head_t	list;

		uint16_t	cost;	// セルの移動コスト
		uint16_t	hight;	// セルの高さ

		enum {
			WSLG_CELL_FLG_NULL	= 0x0001,	// 透過地形
			WSLG_CELL_FLG_ZOC	= 0x0002,	// 地形ZOC属性あり
		};
		uint16_t	flags;	// セルの状態フラグ

		void clear(uint16_t _cost, uint16_t _height) {
			init_list_head(&list);
			flags = 0;
			cost = _cost;
			hight = _height;
		}
	};

	struct wslg_piece {
		list_head_t	list;
		wslg_piece() {
			init_list_head(&list);
		}
	};

	int32_t get_column() { return __maps.get_width(); }
	int32_t get_row() { return __maps.get_hight(); }
	int32_t get_height() { return __maps.get_depth(); }

	wslg_field();
	wslg_field(int16_t column, int16_t row, int16_t height);
	int initialize(int16_t column, int16_t row, int16_t height);

	// セルのコストと高さを設定する。
	int initialize_cell(int16_t x, int16_t y, int16_t depth, uint16_t cost, uint16_t h);

	wslg_cell* get_cell(int16_t x, int16_t y, int16_t d);

	// 指定座標にpieceを設定する。
	wslg_cell* set_piece(wslg_piece *piecep, int16_t x, int16_t y, int16_t d);

	// 指定座標のpieceを取得する。
	wslg_piece* get_piece(wslg_field::wslg_cell *cellp,
			      wslg_field::wslg_piece *piecep);

	// 指定座標のpieceを取得する。
	wslg_piece* get_piece(int16_t x, int16_t y, int16_t d);

	// pieceを取り除く。
	void del_piece(wslg_piece *piecep);
protected:
	obj_maps<wslg_cell>	__maps;
} wslg_field_t;

#endif // _WSLG_FIELD_H_
