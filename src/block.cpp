#include "game.h"

bool Block::match(const Block *right) const { 
	return getSprite() == right->getSprite() && (getState() == right->getState()); 
}

void Block::changeState(gameState gs) {
	state = gs;
}