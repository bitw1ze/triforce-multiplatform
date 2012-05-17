#include "game.h"

const uint64 Bonus::moveInterval = 50;
const int Bonus::totalMove = 24;
const int Bonus::moveSpeed = 1;

Bonus::Bonus(const Cell &cell, int cnt, BonusType bt, Grid &grid) : CObject() {
	set(cell, cnt, bt, grid);
}

Bonus::Bonus(const Bonus &src) {
	clone(src);
}

void Bonus::set(const Cell &cell, int cnt, BonusType bt, Grid &grid) {
	row = cell.row;
	col = cell.col;
	count = cnt;
	bonusType = bt;
	setSprite(new CBaseSprite(*GamePlay::bonusSprite));
	Setxy(grid.blocks[row][col].getX(), grid.blocks[row][col].getY() - GamePlay::blockLength/2);

	offset = 0;
	lastMove = 0;
	if (bonusType == Bonus::CHAIN)
		sprintf_s(str, "x%d", count);
	else
		sprintf_s(str, "%d", count);

	lastMove = mainTimer->time();
}

void Bonus::clone(const Bonus &src) {
	row = src.row;
	col = src.col;
	count = src.count;
	bonusType = src.bonusType;
	setSprite(new CBaseSprite(*src.getSprite()));
	text.setSprite(src.text.getSprite());
	Setxy(src.getX(), src.getY());
	text.Setxy(src.text.getX(), src.text.getY());

	offset = src.offset;
	lastMove = src.lastMove;
}

bool Bonus::update() {
	if (mainTimer->elapsed(lastMove, moveInterval)) {
		lastMove = mainTimer->time();
		offset += moveSpeed;
		offsetY(-moveSpeed);
	}

	return offset < totalMove;
}

void Bonus::display() {
	float x = (getX() + getWidth()/2) / (float)screen_w;
	float y = 1.0 - (float)(getY() + getHeight()/2) / ((float)screen_h);
	draw(0);
	textPrintf(x, y, GamePlay::font1, str, GamePlay::fcolor2);
}

Bonus::~Bonus() {
//	setSprite(NULL);
}