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
	if (bonusType == Bonus::CHAIN) 
		text.setSprite(new CBaseSprite(*GamePlay::chainFontSprite));
	else 
		text.setSprite(new CBaseSprite(*GamePlay::chainFontSprite));

	Setxy(grid.blocks[row][col].getX(), grid.blocks[row][col].getY() - GamePlay::blockLength/2);
	text.Setxy(getX(), getY());

	offset = 0;
	lastMove = 0;


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
		text.offsetY(-moveSpeed);
	}

	return offset < totalMove;
}

void Bonus::display() {
	draw(0);
	if (bonusType == Bonus::CHAIN)
		text.draw(count - 2);
	else if (bonusType == Bonus::COMBO)
		text.draw(count - 4);
}

Bonus::~Bonus() {
}