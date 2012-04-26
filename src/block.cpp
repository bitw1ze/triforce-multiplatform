#include "game.h"

/*	match
	Detect a single match of one block to another block based on the sprite. */
bool Block::match(const Block *right) const { 
	return getSprite() == right->getSprite() && (getState() == right->getState()); 
}

/*	changeState
	Changes the state of the block. Much to be done with this function */
void Block::changeState(gameState gs) {
	state = gs;
}

/*	swap
	Swap two adjacent blocks that the cursor has selected. */
void Block::swap(Block &right) {
	CBaseSprite *temp = getSprite();
	setSprite( right.getSprite() );
	right.setSprite( temp );

	gameState status = getState();
	changeState( right.getState() );
	right.changeState( status );
}