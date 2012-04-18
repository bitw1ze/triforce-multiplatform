#include "game.h"

const string Cursor::spriteFile = "cursor.bmp";

void Cursor::moveUp() {
}

void Cursor::moveDown() {
}

void Cursor::moveLeft() {
}

void Cursor::moveRight() {
}

Cursor::Cursor(Grid *g) {
	grid = g;
	cursor = new CObject();
}