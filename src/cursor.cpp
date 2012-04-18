#include "game.h"

const string Cursor::spriteFile = "cursor.bmp";

void Cursor::moveUp() {
	++row;
	grid->getY();
}

void Cursor::moveDown() {
}

void Cursor::moveLeft() {
}

void Cursor::moveRight() {
}

Cursor::Cursor(Grid *grid) {
	this->grid = grid;
	cursor_w = grid->getBlockWidth() * 2;
	cursor_h = grid->getBlockHeight();
	cursor_x = grid->getX();
	cursor_y = grid->getY();
	CBaseSprite *sprite = new CBaseSprite(0, cursor_w, cursor_h);
	
	create(cursor_x, cursor_y, cursor_w * 2, cursor_h, sprite, mainTimer);

	int r=254, g=0, b=254, frameCount=1, frame=0;
	sprite->loadFrame(frame, themeDirectory + spriteFile, r, g, b);
}