/*	block.cpp

	Block is an extension of CObject. A Block represents a single Block in a
	Grid. Each Block contains pointers to each adjacent Block to the left,
	right, up, and down. Therefore, one can think of the matrix of blocks as
	a linked data structure where every object has direct access to every
	adjacent Block.

	A Block can be in one of four (5) states, as described below:
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

int Block::interval_combo;


Block::Block() : CObject() {
	timer = new CTimer(); 
	timer->start();

	last_combo = -1;
	interval_combo = 500;

	last_fall = -1;
	interval_fall = 20;
	total_falls = 0;
	fall_factor = 6;
	count_falls = 0;

	state = inactive; 
}

/*	changeState
	Changes the state of the block. Much to be done with this function */
void Block::changeState(gameState gs) {
	switch (gs) {
	case combo:
		if (state != combo) {
			last_combo = timer->time();
			grid->changeState(Grid::combo);
		}
		break;
	case fall:
		last_fall = timer->time();
		count_falls = 0;
		break;
	case disabled:
		break;
	case enabled:
		break;
	}
	state = gs;
}

void Block::composeFrame() {
	switch (state) {
	case enabled:
		break;

	case combo:
		if (timer->elapsed(last_combo, total_combo_interval)) {
			changeState(disabled);
			//detectAndSetFallState();
		}
		break;

	case fall:
		if (timer->elapsed(last_fall, interval_fall)) {
			last_fall = timer->time();
			offsetY(getHeight() / fall_factor);
			++count_falls;
		}
		if (count_falls >= total_falls) {
			int n = (total_falls / fall_factor);
			offsetY(n * -getHeight());
			state = enabled;
			/*
			FIXME
			Block *temp = this;
			for (int i=0; i<n; ++i)
				temp = temp->down;
			temp->setSprite(getSprite());
			temp->changeState(enabled);
			state = disabled;

			temp->detectAndSetComboState();
			*/
		}

		break;
	}
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

/*	swap
	Swap two adjacent blocks that the cursor has selected. */
bool Block::swap(Block &right) {
	if ( getState() == combo || getState() == fall ||
		right.getState() == combo || right.getState() == fall)
		return false;

	CBaseSprite *temp = getSprite();
	setSprite( right.getSprite() );
	right.setSprite( temp );

	gameState st = state;
	state = right.state;
	right.state = st;

	return true;
}

/*	match
	Detect a single match of one block to another block based on the sprite.
	ignoreActive should be enabled when only testing if block states and sprites
	match, such as when generating blocks */
bool Block::match(const Block &right, bool ignoreActive) const { 

	gameState ls = getState();
	gameState rs = right.getState();

	if (getSprite() != right.getSprite())
		return false;

	if (ignoreActive) 
		return (ls == enabled || ls == inactive) && (rs == enabled || rs == inactive);
	else
		return (ls == enabled && rs == enabled);
}

