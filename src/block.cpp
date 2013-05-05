/*	block.cpp

	Block is an extension of CObject. A Block represents a single Block in a
	Grid. The state and position of Blocks are controlled by the Grid class.

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
		*/

#include "game.h"

const int Block::fallFactor = 8;
const uint64 Block::frameTransitionInterval = 10;

Block::Block() : CObject() {
	set();
}

Block::Block(const Block &src) {
	clone(src);
}

Block & Block::operator =(const Block &src) {
	clone(src);

	return *this;
}

void Block::set() {
	state = inactive;
	fallOffset = 0;
	lastFrameTransition = 0;
	currentFrame = 0;
}

void Block::clone(const Block &src) {
	currentFrame = src.currentFrame;
	lastFrameTransition = src.lastFrameTransition;
	changeState(src.getState());
	setSprite(src.getSprite());
	fallOffset = src.fallOffset;
}

void Block::fallDown() {
	int offset = GamePlay::blockLength / fallFactor;;
	fallOffset += offset;
	offsetY(offset);
	if (fallOffset >= GamePlay::blockLength) {
		resetFall();
	}
}

void Block::resetFall() {
	fallOffset = 0;
	offsetY(-GamePlay::blockLength);
}

int Block::getFallOffset() const {
	return fallOffset;
}

void Block::setFallOffset(int f) {
	fallOffset = f;
}

void Block::changeState(gameState gs) {
	state = gs;
	switch (state) {
	case enabled:
	case combo:
		currentFrame = 0;
		break;
	case inactive:
		currentFrame = 4;
		break;
  case disabled:
    break;

  default:
    break;
	}
}

void Block::composeFrame() {
	if (state == combo && mainTimer->elapsed(lastFrameTransition, frameTransitionInterval)) {
		lastFrameTransition = mainTimer->time();
		currentFrame = ((currentFrame + 1) % 4);
	}
}

void Block::display() {
	if (state != disabled)
		draw(currentFrame);
}
