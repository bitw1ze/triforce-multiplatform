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
	if (getY() + 2*grid->getBlockHeight() <= grid->getY()) {
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

	// set cursor type, and see if cursor is even inside the grid
	if (!(x > grid->getX() && x < grid->getX() + (int)ncols*grid->getBlockWidth()))
	{
		glutSetCursor(GLUT_CURSOR_INHERIT);
		return;
	}
	if (y > grid->getY() || y < grid->getY() - (int)(nrows-1)*grid->getBlockHeight())
	{
		glutSetCursor(GLUT_CURSOR_INHERIT);
		return;
	}

	// move cursor inside grid
	glutSetCursor(GLUT_CURSOR_LEFT_SIDE);
	if (getX() + 2*grid->getBlockWidth() - x_threshold*grid->getBlockWidth() < x)
		do
			moveRight();
		while (getX() + 2*grid->getBlockWidth() - x_threshold*grid->getBlockWidth() < x && col < ncols - 2);
	else
		while (x - x_threshold*grid->getBlockWidth() < getX() && col > 0)
			moveLeft();

	if (getY() - grid->getBlockHeight()*y_threshold > y)
		do
			moveUp();
		while (getY() - grid->getBlockHeight()*grid->getBlockHeight() > y && row < grid->getTopRow());
	else
		while (y > getY() + grid->getBlockHeight()*(1+y_threshold) && row > 0) 
			moveDown();
}