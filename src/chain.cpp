#include "game.h"

const uint64 Chain::displayDuration = 1500;

Chain::Chain(int r, int c, int cnt) {
	set(r, c, cnt);
}

Chain::Chain(const Cell &cell, int cnt) {
	set(cell.row, cell.col, cnt);
	init();
}

Chain::Chain(const Chain &src) {
	clone(src);
	init();
}

void Chain::set(int r, int c, int cnt) {
	row = r;
	col = c;
	count = cnt;
	lastActivation = 0;
}

void Chain::clone(const Chain &src) {
	row = src.row;
	col = src.col;
	count = src.count;
	lastActivation = src.lastActivation;
}

bool Chain::update() {
	cout << "update chain\n";
	return !mainTimer->elapsed(lastActivation, displayDuration);
}

void Chain::init() {
	lastActivation = mainTimer->time();
}

void Chain::display(Grid &grid) {
	char str[5];
	float x = grid.blocks[row][col].getX() / (float)screen_w;
	float y = 1.0 - (float)(grid.blocks[row][col].getY()) / ((float)screen_h);
	sprintf_s(str, "x%d", count);
	textPrintf(x, y, GamePlay::font1, str, GamePlay::fcolor1);
}