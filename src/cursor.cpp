#include "game.h"

Cursor::Cursor(Grid *gr, CBaseSprite *sprite) {
	grid = gr;

	cursor_delta = sprite->GetHeight();
	row = nrows / 2;
	col = ncols / 2;
	create(0, 0, 0, 0, sprite);
	setPos(0, 0);
}

void Cursor::moveLeft(bool doDraw) {
	if (col > 0) {
		--col;
		offsetX( -cursor_delta );
		if (doDraw)
			draw(0);
	}
	printf("grid: (%d, %d)\ncursor: (%d, %d)\nrow: %d\n", grid->getX(), grid->getY(), getX(), getY(), row);
}

void Cursor::moveRight(bool doDraw) {
	if (col < ncols - 2) {
		++col;
		offsetX( cursor_delta );
		if (doDraw)
			draw(0);
	}
	printf("grid: (%d, %d)\ncursor: (%d, %d)\nrow: %d\n", grid->getX(), grid->getY(), getX(), getY(), row);
}

void Cursor::moveDown(bool doDraw) {
	if (row > 0) {
		--row;
		offsetY( cursor_delta );
		if (doDraw)
			draw(0);
	}
	printf("grid: (%d, %d)\ncursor: (%d, %d)\nrow: %d\n", grid->getX(), grid->getY(), getX(), getY(), row);
}


void Cursor::moveUp(bool doDraw) {
	if (row < grid->getTopRow()) {
		++row;
		offsetY( -cursor_delta );
		if (doDraw)
			draw(0);
	}
	printf("grid: (%d, %d)\ncursor: (%d, %d)\nrow: %d\n", grid->getX(), grid->getY(), getX(), getY(), row);
}

void Cursor::setPos(int c, int r) {
	row = r;
	col = c;
	Setxy( grid->getX() + col * cursor_delta, grid->getY() - (row * cursor_delta) - (2 * cursor_delta));
	printf("row: %d\n", row);
}

void Cursor::shiftRow() {
	if (row <= grid->getTopRow())
		++row;
	else
		offsetY(cursor_delta);
}