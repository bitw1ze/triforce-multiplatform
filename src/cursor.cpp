#include "game.h"

Cursor::Cursor(Grid *gr, CBaseSprite *sprite) {
	grid = gr;

	cursor_delta = sprite->GetHeight();
	row = nrows / 2;
	col = ncols / 2;
	init(sprite);
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
		setPos(getCol(), grid->getTopRow());
	else
		setPos(getCol(), grid->getTopRow()+1);
}

void Cursor::passiveMouseHover(int x, int y) {
	static const float y_threshold = 0,
	                   x_threshold = 0.48;

	// hide/set cursor type
	static const int margin = 1; // number of blocks away from cursor to hide mouse
	if (getX() - margin*cursor_delta < x && getX() + (margin+2)*cursor_delta > x
		&& getY() - margin*cursor_delta < y && getY() + (margin+2)*cursor_delta > y)
		glutSetCursor(GLUT_CURSOR_NONE);
	else
		glutSetCursor(GLUT_CURSOR_INHERIT);

	if (getX() + 2*cursor_delta - x_threshold*cursor_delta < x)
		do
			moveRight();
		while (getX() + 2*cursor_delta - x_threshold*cursor_delta < x && col < ncols - 2);
	else
		while (x - x_threshold*cursor_delta < getX() && col > 0)
			moveLeft();

	if (getY() - cursor_delta*y_threshold > y)
		do
			moveUp();
		while (getY() - cursor_delta*y_threshold > y && row < grid->getTopRow());
	else
		while (y > getY() + cursor_delta*(1+y_threshold) && row > 0) 
			moveDown();
}