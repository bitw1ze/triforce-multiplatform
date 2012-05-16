#include "game.h"

Chain::Chain(int r, int c, int cnt) {
	set(r, c, cnt);
}

Chain::Chain(const Chain &src) {
	clone(src);
}

void Chain::set(int r, int c, int cnt) {
	row = r;
	col = c;
	count = cnt;
	lastActivation = 0;
	displayDuration = cnt;
}

void Chain::clone(const Chain &src) {
	row = src.row;
	col = src.col;
	count = src.count;
	lastActivation = src.lastActivation;
	displayDuration = src.displayDuration;
}

bool Chain::update() {
	return mainTimer->elapsed(lastActivation, displayDuration);
}

void Chain::activate() {
	lastActivation = mainTimer->time();
}

void Chain::display(Grid &grid) {
	
}