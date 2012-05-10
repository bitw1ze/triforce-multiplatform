/*	block.cpp

	Block is an extension of CObject. A Block represents a single Block in a
	Grid. Each Block contains pointers to each adjacent Block to the left,
	right, up, and down. Therefore, one can think of the matrix of blocks as
	a linked data structure where every object has direct access to every
	adjacent Block.

	A Block can be in one of five (5) states, as described below:
		inactive:	The Block has just entered the Grid area on the bottom but
					cannot be interacted with yet.
		enabled:	The Block is in a normal play state. It can be swapped, made
					into a combo, and put into a falling state
		combo:		The Block is current part of a combo of 3 or more Blocks. Upon
					changing state, the Block will change the Grid's state to combo
					(if not already in the combo state) and add time to the Grid's
					combo timer. While in the combo state, the Block will not move
					it will display a different sprite, and it cannot be swapped.
		fall:		The Block is currently falling because a combo was made below
					the current block or it has been swapped to a space with disabled
					block(s) below. While falling, a Block cannot be swapped.
		disabled:	The Block has been destroyed and will no longer be displayed.

	Blocks are responsible for detecting matches, combos and falling states. They 
	can set states for other blocks and for the Grid they reside in.

	Since Blocks are linked, there are functions to make it easier to find offsets
	from a given block and to get the distance between two blocks. */

#include "game.h"

const int Block::fallFactor = 8;

Block::Block() : CObject() {
	timer = new CTimer(); 
	timer->start();

	fallOffset = 0;
	state = inactive; 
}

Block::Block(const Block &block) {
	clone(block);
}

Block & Block::operator =(const Block &block) {
	clone(block);

	return *this;
}

void Block::clone(const Block &src) {
	changeState(src.getState());
	setSprite(src.getSprite());
	timer = src.timer;
	fallOffset = src.fallOffset;
}

void Block::fallDown() {
	int offset = getHeight() / fallFactor;;
	fallOffset += offset;
	offsetY(offset);
	if (fallOffset >= getHeight()) {
		resetFall();
	}
}

void Block::resetFall() {
	fallOffset = 0;
	offsetY(-getHeight());
}

int Block::getFallOffset() const {
	return fallOffset;
}

void Block::setFallOffset(int f) {
	fallOffset = f;
}

/*	changeState
	Changes the state of the block. Much to be done with this function */
void Block::changeState(gameState gs) {
	state = gs;
}

void Block::composeFrame() {
}

void Block::display() {
	switch (state) {
	case enabled:
		draw(0);
		break;
	case combo:
		draw(1);
		break;
	case fall:
		draw(0);
		break;
	case inactive:
		draw(0);
		break;
	case disabled:
		break;
	}
}