#include "game.h"

void Cursor::moveLeft() {
	if (col > 0) {
		--col;
		cursor_x -= cursor_delta;
		setX(cursor_y);
		draw(0);
	}
}

void Cursor::moveRight() {
	if (col < ncols - 1) {
		++col;
		cursor_x += cursor_delta;
		setX(cursor_y);
		draw(0);
	}
}

void Cursor::moveDown() {
	if (row > 0) {
		--row;
		cursor_y += cursor_delta;
		setY(cursor_y);
		draw(0);
	}
}


void Cursor::moveUp() {
	if (row < nrows - 1) {
		++row;
		cursor_y -= cursor_delta;
		setY(cursor_y);
		draw(0);
	}
}

void Cursor::setPos(int c, int r) {
	row = r;
	col = c;
	cursor_x = grid->getX() + col * grid->getBlockWidth();
	cursor_y = grid->getY() - row * grid->getBlockHeight();
	Setxy(cursor_x, cursor_y);
}

Cursor::Cursor(Grid *gr, CBaseSprite *sprite) {
	grid = gr;

	row = nrows / 2;
	col = ncols / 2;
	create(0, 0, 0, 0, sprite, mainTimer);
	setPos(4, 4);
	printf("dim: (%d, %d)\npos: (%d, %d)\n", cursor_w, cursor_h, cursor_x, cursor_y);
}