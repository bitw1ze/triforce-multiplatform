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

Cursor::Cursor(Grid *gr, CBaseSprite *sprite) {
	grid = gr;

	row = nrows / 2;
	col = ncols / 2;
	create(cursor_x, cursor_y, cursor_w * 2, cursor_h, sprite, mainTimer);
	printf("dim: (%d, %d)\npos: (%d, %d)\n", cursor_w, cursor_h, cursor_x, cursor_y);
}