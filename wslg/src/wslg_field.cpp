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
#include <libgame/wslg/wslg_field.hpp>

wslg_field::wslg_field()
{
}

wslg_field::wslg_field(int16_t column, int16_t row, int16_t height)
{
	initialize(column, row, height);
}

int
wslg_field::initialize(int16_t column, int16_t row, int16_t height)
{
	int ret;
	ret = __maps.initialize(column, row, height);
	if (ret) {
		return ret;
	}

	wslg_cell	*cellp;
	int16_t x, y, d;
	omap_for_each_entry(cellp, x, y, d, &__maps) {
		cellp->clear(1, 0);
	}
	return 0;
}

int
wslg_field::initialize_cell(int16_t x, int16_t y, int16_t depth, uint16_t cost, uint16_t height)
{
	wslg_cell	*cellp;
	cellp = __maps.get_object(x, y, depth);
	if (!cellp) {
		return -EINVAL;
	}

	cellp->clear(cost, height);
	return 0;
}

wslg_field::wslg_cell*
wslg_field::get_cell(int16_t x, int16_t y, int16_t d)
{
	wslg_cell	*cellp;

	cellp = __maps.get_object(x, y, d);
	if (!cellp) {
		return NULL;
	}
	else {
		return cellp;
	}
}

// 指定座標にpieceを設定する。
wslg_field::wslg_cell*
wslg_field::set_piece(wslg_piece *piecep, int16_t x, int16_t y, int16_t d)
{
	wslg_cell	*cellp;
	cellp = __maps.get_object(x, y, d);
	if (!cellp) {
		return NULL;
	}

	list_add_tail(&piecep->list, &cellp->list);
	return cellp;
}

// 指定座標のpieceを取得する。
wslg_field::wslg_piece*
wslg_field::get_piece(wslg_field::wslg_cell *cellp,
		      wslg_field::wslg_piece *piecep)
{
	if (!cellp) {
		return NULL;
	}

	if (!piecep) {
		return list_first_entry_or_null(&cellp->list, wslg_field::wslg_piece, list);
	}	else {
		return list_next_entry_or_null(&piecep->list, &cellp->list, wslg_field::wslg_piece, list);
	}
}

// 指定座標のpieceを取得する。
wslg_field::wslg_piece*
wslg_field::get_piece(int16_t x, int16_t y, int16_t d)
{
	wslg_cell* cellp;
	wslg_piece* piecep;
	cellp = get_cell(x, y, d);
	piecep = get_piece(cellp, NULL);
	return piecep;
}

// pieceを取り除く。
void
wslg_field::del_piece(wslg_piece *piecep)
{
	if (piecep) {
		list_del_init(&piecep->list);
	}
}
