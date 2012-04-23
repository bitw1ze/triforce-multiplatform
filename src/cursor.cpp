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
}

void Cursor::moveRight(bool doDraw) {
	if (col < ncols - 2) {
		++col;
		offsetX( cursor_delta );
		if (doDraw)
			draw(0);
	}
}

void Cursor::moveDown(bool doDraw) {
	if (row > 0) {
		--row;
		offsetY( cursor_delta );
		if (doDraw)
			draw(0);
	}
}


void Cursor::moveUp(bool doDraw) {
	if (row < grid->getTopRow()) {
		++row;
		offsetY( -cursor_delta );
		if (doDraw)
			draw(0);
	}
	printf("row: %d\ntoprow: %d\n", row, grid->getTopRow());
}

void Cursor::setPos(int c, int r) {
	row = r;
	col = c;
	Setxy( grid->getX() + col * cursor_delta, grid->getY() - (row * cursor_delta));
}

void Cursor::shiftRow() {
	if (row < grid->getTopRow())
		++row;
	else if (row == nrows - 1)
		this->setPos(getCol(), grid->getTopRow());
	else
		this->setPos(getCol(), grid->getTopRow()+1);
	
	printf("row: %d\ntoprow: %d\n", row, grid->getTopRow());
}