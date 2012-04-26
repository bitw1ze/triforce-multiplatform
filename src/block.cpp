#include "game.h"

bool Block::match(const Block *right) const { 
	return getSprite() == right->getSprite() && (getState() == right->getState()); 
}

void Block::changeState(gameState gs) {
	state = gs;
}

void Block::swap(Block &right) {
	CBaseSprite *temp = getSprite();
	setSprite( right.getSprite() );
	right.setSprite( temp );

	gameState status = getState();
	changeState( right.getState() );
	right.changeState( status );
}