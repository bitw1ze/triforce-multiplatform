#include "game.h"
#include "input.h"

Cursor::Cursor(Grid *gr, CBaseSprite *sprite) {
	grid = gr;

	cursor_delta = sprite->GetHeight();
	row = nrows / 2;
	col = ncols / 2;
	init(sprite);
	setPos(0, 0);
}

bool Cursor::moveLeft(bool doDraw) {
	if (col > 0) {
		--col;
		offsetX( -cursor_delta );
		if (doDraw)
			draw(0);
		return true;
	}
	return false;
}

bool Cursor::moveRight(bool doDraw) {
	if (col < ncols - 2) {
		++col;
		offsetX( cursor_delta );
		if (doDraw)
			draw(0);
		return true;
	}
	return false;
}

bool Cursor::moveDown(bool doDraw) {
	if (getY() + 2*grid->getBlockHeight() <= grid->getY()) {
		--row;
		offsetY( cursor_delta );
		if (doDraw)
			draw(0);
		return true;
	}
	return false;
}

bool Cursor::moveUp(bool doDraw) {
	if (row < nrows) {
		++row;
		offsetY( -cursor_delta );
		if (doDraw)
			draw(0);
		return true;
	}
	return false;
}

void Cursor::setPos(int c, int r) {
	row = r;
	col = c;
	Setxy( grid->getX() + col * cursor_delta, grid->getY() - (row * cursor_delta));
}

void Cursor::shiftRow() {
	if (row < nrows - 1)
		++row;
	else if (row == nrows - 1)
		setPos(getCol(), nrows - 1);
	else
		setPos(getCol(), nrows);
}

void Cursor::passiveMouseHover(int x, int y) {
	const float x_threshold = 0.49;

	// move cursor inside grid
	glutSetCursor(GLUT_CURSOR_LEFT_SIDE);
	if (getX() + 2*grid->getBlockWidth() - x_threshold*grid->getBlockWidth() < x && col < ncols - 2)
		moveRight();
	else if (x - x_threshold*grid->getBlockWidth() < getX() && col > 0)
		moveLeft();
	else if (getY() > y && row < nrows)
	    moveUp();
	else if (y > getY() + grid->getBlockHeight() && row > 0)
		moveDown(); 
}