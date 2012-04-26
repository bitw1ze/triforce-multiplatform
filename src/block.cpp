#include "game.h"

Block::Block() : CObject() {
	state = enabled; 
	timer = new CTimer(); 
	timer->start();
	last_time = timer->time();
}

/*	match
	Detect a single match of one block to another block based on the sprite. */
bool Block::match(const Block *right) const { 
	return getSprite() == right->getSprite() && (getState() == right->getState()); 
}

/*	changeState
	Changes the state of the block. Much to be done with this function */
void Block::changeState(gameState gs) {
	state = gs;
	switch (state) {
	case combo:
		onCombo();
		break;
	}
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

void Block::onCombo() {
	
}

void Block::display() {
	switch (state) {
	case enabled:
		draw(0);
		break;
	case combo:
		draw(0);
		if (timer->elapsed(last_time, 200))
			changeState(disabled);
		break;
	case disabled:
		break;
	}
}