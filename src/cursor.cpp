#include "game.h"

void Cursor::moveLeft() {
	if (col > 0) {
		--col;
		offsetX( -cursor_delta );
		draw(0);
	}
}

void Cursor::moveRight() {
	if (col < ncols - 2) {
		++col;
		offsetX( cursor_delta );
		draw(0);
	}
}

void Cursor::moveDown() {
	if (row > 0) {
		--row;
		offsetY( cursor_delta );
		draw(0);
	}
}


void Cursor::moveUp() {
	if (row < grid->getTopRow() - 1) {
		++row;
		offsetY( -cursor_delta );
		draw(0);
	}
}

void Cursor::setPos(int c, int r) {
	row = r;
	col = c;
	offsetXY( grid->getX() + col * cursor_delta, grid->getY() - row * cursor_delta );
}

Cursor::Cursor(Grid *gr, CBaseSprite *sprite) {
	grid = gr;

	cursor_delta = sprite->GetHeight();
	row = nrows / 2;
	col = ncols / 2;
	create(0, 0, 0, 0, sprite);
	setPos(3, 3);
}