/*	block.cpp
	by Gabe Pike
	Changes:
		- Blocks now keep track of their own state
		- Put the swap function in Block
		- Block now handles its own display function based on states
*/

#include "game.h"

int Block::interval_combo;

Block::Block() : CObject() {
	active = false;
	state = enabled; 
	timer = new CTimer(); 
	timer->start();
	last_state = -1;
	last_fall = -1;
	interval_fall = 100;
	total_falls = 0;
	fall_offset = 6;
	count_falls = 0;
}

/*	changeState
	Changes the state of the block. Much to be done with this function */
void Block::changeState(gameState gs) {
	switch (gs) {
	case combo:
	//	onCombo();
		break;
	case fall:
		last_fall = mainTimer->time();
		count_falls = 0;
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

void Block::setStateTimer(gameState newState, gameState nextst) {
	last_state = mainTimer->time();
	if (newState == combo)
		interval_state = Block::interval_combo;
	changeState(newState);
	nextState = nextst;
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

void Block::composeFrame() {
	if (last_state > 0 && mainTimer->elapsed(last_state, interval_state)) {
		changeState(nextState);
		last_state = -1;
	}

	switch (state) {
	case fall:
		cout << "Falling\n";
		if (mainTimer->elapsed(last_fall, interval_fall)) {
			offsetY(getHeight() / fall_offset);
			++count_falls;
		}
		if (count_falls >= total_falls) 
			changeState(enabled);

		break;
	}
}

void Block::display() {
	composeFrame();

	switch (state) {
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

/*	match
	Detect a single match of one block to another block based on the sprite.
	ignoreActive should be enabled when only testing if block states and sprites
	match, such as when generating blocks */
bool Block::match(const Block *right, bool ignoreActive) const { 
	return 
			right != NULL
		&&	getState() == right->getState()
		&&	(ignoreActive ? true : (isActive() == true && right->isActive() == true) )
		&&	getState() == enabled
		&&	getSprite() == right->getSprite();
}

/*	leftMatch, rightMatch, downMatch, and upMatch
	These functions are all used to detect combos. They return the number of blocks
	that match the block passed to the direction in the function's name.
	e.g. a 3 combo will return 2		*/

int Block::leftMatch(Block **matched, bool ignoreActive) {
	int matches = 0;
	
	Block *temp = this->left;
	while (match(temp, ignoreActive))  {
		temp = temp->left;
		++matches;
	}

	if (matched != NULL)
		*matched = temp;

	return matches;
}

int Block::rightMatch(Block **matched, bool ignoreActive) {
	int matches = 0;
	
	Block *temp = this->right;
	while (match(temp, ignoreActive))  {
		temp = temp->right;
		++matches;
	}

	if (matched != NULL)
		*matched = temp;

	return matches;
}


int Block::upMatch(Block **matched, bool ignoreActive) {
	int matches = 0;
	
	Block *temp = this->up;
	while (match(temp, ignoreActive))  {
		temp = temp->up;
		++matches;
	}

	if (matched != NULL)
		*matched = temp;

	return matches;
}

int Block::downMatch(Block **matched, bool ignoreActive) {
	int matches = 0;
	
	Block *temp = this->down;
	while (match(temp, ignoreActive))  {
		temp = temp->down;
		++matches;
	}

	if (matched != NULL)
		*matched = temp;

	return matches;
}

/*	detectCombos
	This function uses the match<direction> functions to find all possible combos.
	A combo can be stored as a set of Cells, where a Cell is a struct that holds the
	row and column of a block. A combo in one direction (e.g. 3 blocks in a row 
	horizontal) is stored in 2 cells, and a combo in two directions (e.g. 5 blocks 
	vertical and 3 blocks horizontal) is stored in 4 cells.
	
	This function computes the cells by first finding a horizontal match. If a match
	is found, it will then iteratively search for a vertical from each block in the
	horizontal match.  */
bool Block::detectCombos() {
	Block *bleft, *bright, *bup, *bdown;
	Block *leftright = NULL, *downup = NULL;
	int nleft, nright, nup, ndown;

	nleft = leftMatch(&bleft);
	nright = rightMatch(&bright);

	if ( nleft + nright >= 2) {
		leftright = bleft;

		do {
			leftright->changeState(combo);

			nup = leftright->upMatch(&bup);
			ndown = leftright->downMatch(&bdown);

			if (nup + ndown >= 2) {
				downup = bdown;
				do {
					downup->changeState(combo);
					downup = downup->up;
				} while (downup != bup);
			}

			leftright = leftright->right;
		} while (leftright != right);

		return true;
	}
	else {
		ndown = downMatch(&bdown);
		nup = upMatch(&bup);

		if ( nleft + nright >= 2) {
			downup = bdown;

			do {
				downup->setStateTimer(combo, disabled);

				nleft = downup->leftMatch(&bleft);
				nright = downup->rightMatch(&bright);

				if (nup + ndown >= 2) {
					leftright = bleft;
					do {
						downup->setStateTimer(combo, disabled);
						downup = downup->up;
					} while (bleft != bright);
				}

				downup = downup->up;
			} while (downup != down);

			return true;
		}
	}

	return false;
}

void Block::setFallStates() {
	Block *temp = up;
	int nfall = 1;

	while (temp != NULL && temp->getState() == enabled) {
		temp->setFalls(nfall);
		temp->changeState(fall);

		temp = temp->up;
		++nfall;
	}
}

int Block::downDistance(Block *block) const {
	int count = 1;
	Block *temp = down;
	while (temp != block && temp != NULL) {
		temp = temp->down;
		++count;
	}

	return (temp != NULL) ? count : 0;
}

int Block::upDistance(Block *block) const {
	int count = 1;
	Block *temp = up;
	while (temp != block && temp != NULL) {
		temp = temp->up;
		++count;
	}

	return (temp != NULL) ? count : 0;
}

int Block::leftDistance(Block *block) const {
	int count = 1;
	Block *temp = left;
	while (temp != block && temp != NULL) {
		temp = temp->left;
		++count;
	}

	return (temp != NULL) ? count : 0;
}

int Block::rightDistance(Block *block) const {
	int count = 1;
	Block *temp = right;
	while (temp != block && temp != NULL) {
		temp = temp->right;
		++count;
	}

	return (temp != NULL) ? count : 0;
}