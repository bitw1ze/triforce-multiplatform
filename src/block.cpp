/*	block.cpp
	by Gabe Pike
	Changes:
		- Blocks now keep track of their own state
		- Put the swap function in Block
		- Block now handles its own display function based on states
*/

#include "game.h"

Block::Block() : CObject() {
	state = displayed; 
	timer = new CTimer(); 
	timer->start();
	last_time = timer->time();
}

/*	match
	Detect a single match of one block to another block based on the sprite. */
bool Block::match(const Block *right) const { 
	return 
		getState() == right->getState()
		&& (getState() == enabled || getState() == displayed)
		&&	getSprite() == right->getSprite();
}

/*	changeState
	Changes the state of the block. Much to be done with this function */
void Block::changeState(gameState gs) {
	
	switch (state) {
	case combo:
		onCombo();
		break;
	case fall:
		//onFall();
		break;
	case disabled:
		//setSprite(NULL);
		break;
	case enabled:
		break;
	}
	state = gs;
}

/*	swap
	Swap two adjacent blocks that the cursor has selected. */
bool Block::swap(Block &right) {
	if ( getState() == combo || getState() == fall ||
		right.getState() == combo || right.getState() == fall)
		return false;

	CBaseSprite *temp = getSprite();
	setSprite( right.getSprite() );
	right.setSprite( temp );

	gameState status = getState();
	changeState( right.getState() );
	right.changeState( status );

	return true;
}

void Block::onCombo() {
	
}

void Block::display() {
	switch (state) {
	case displayed:
	case enabled:
		draw(0);
		break;
	case combo:
		draw(0);
		break;
	case fall:
		draw(0);
		break;
	case disabled:
		break;
	}
}