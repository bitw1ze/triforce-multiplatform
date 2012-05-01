/*	block.cpp
	by Gabe Pike
	Changes:
		- Blocks now keep track of their own state
		- Put the swap function in Block
		- Block now handles its own display function based on states
*/

#include "game.h"

//int Block::interval_combo;

Block::Block() : CObject() {
	active = false;
	state = enabled; 
	timer = new CTimer(); 
	timer->start();

	last_combo = -1;
	interval_combo = 1500;

	last_fall = -1;
	interval_fall = 20;
	total_falls = 0;
	fall_factor = 6;
	count_falls = 0;
}

/*	changeState
	Changes the state of the block. Much to be done with this function */
void Block::changeState(gameState gs) {
	switch (gs) {
	case combo:
		last_combo = timer->time();
	//	onCombo();
		break;
	case fall:
		last_fall = timer->time();
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

void Block::composeFrame() {
	
	switch (state) {
	case enabled:
		break;

	case combo:
		if (timer->elapsed(last_combo, 1500)) {
			changeState(disabled);
			setFallStates();
		}
		break;

	case fall:
		if (timer->elapsed(last_fall, interval_fall)) {
			last_fall = timer->time();
			offsetY(getHeight() / fall_factor);
			++count_falls;

			if (total_falls % count_falls == 0) {
				if (down->state == disabled)
					down->setSprite(getSprite());
			}
		}
		if (count_falls >= total_falls) {
			changeState(enabled);
		}

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
bool Block::match(const Block *right, bool ignoreActive) const { 
	return 
			right != NULL
		&&	getState() == right->getState()
		&&	(ignoreActive ? true : (isActive() == true && right->isActive() == true) )
		&&	getState() == enabled
		&&	getSprite() == right->getSprite();
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
	Block *bleft = NULL, *bright = NULL, *bup = NULL, *bdown = NULL;
	Block *leftright = NULL, *downup = NULL;
	int nleft, nright, nup, ndown;

	nleft = leftMatch(&bleft);
	nright = rightMatch(&bright);

	if ( nleft + nright >= 2) {
		leftright = this;
		for (int i=0; i < nleft; i++) 
			leftright = leftright->left;
		
		for (int i=0; i <= nright; ++i) {
			leftright->changeState(combo);
			nup = leftright->upMatch(&bup);
			ndown = leftright->downMatch(&bdown);

			if (nup + ndown >= 2) {
				downup = this;
				for (int i=0; i < ndown; ++i)
					downup = downup->down;

				for (int i=0; i <= nup; ++i) {
					downup->changeState(combo);
					downup = downup->up;
				}
			}

			leftright = leftright->right;
		}

		return true;
	}
	else {
		ndown = downMatch(&bdown);
		nup = upMatch(&bup);

		if ( ndown + nup >= 2) {
			downup = this;
			for (int i=0; i < ndown; ++i)
				downup = downup->down;

			for (int i=0; i <= nup; ++i) {
				downup->changeState(combo);

				nleft = downup->leftMatch(&bleft);
				nright = downup->rightMatch(&bright);

				if (nleft + nright >= 2) {
					leftright = this;
					for (int i=0; i < nleft; ++i) 
						leftright = leftright->left;

					for (int i=0; i <= nright; ++i) {
						leftright->changeState(combo);
						leftright = leftright->right;
					}
				}

				downup = downup->up;
			} 

			return true;
		}
	}

	return false;
}

void Block::setFallStates() {
	Block *temp = NULL;
	int nfall;

	if (up != NULL && up->getState() == enabled)
		nfall = 1;
	else return;

	temp = down;
	while (temp != NULL && temp->getState() != enabled) {
		++nfall;
		temp = temp->down;
	}

	temp = up;
	while (temp != NULL && temp->getState() == enabled) {
		temp->setFallCount(nfall);
		temp->changeState(fall);
		temp = temp->up;
	}

	state = disabled;
}

void Block::transferDown() {

}

/*	leftMatch, rightMatch, downMatch, and upMatch
	These functions are all used to detect combos. They return the number of blocks
	that match the block passed to the direction in the function's name.
	e.g. a 3 combo will return 2		*/

int Block::leftMatch(Block **matched, bool ignoreActive) {
	int matches = 0;
	
	if (this->left == NULL)
		return 0;

	Block *temp = this;
	while (match(temp->left, ignoreActive))  {
		temp = temp->left;
		++matches;
	}

	if (temp == NULL)
		temp = this;

	if (matched != NULL)
		*matched = temp;

	return matches;
}

int Block::rightMatch(Block **matched, bool ignoreActive) {
	int matches = 0;
	
	if (this->right == NULL)
		return 0;

	Block *temp = this;
	while (match(temp->right, ignoreActive))  {
		temp = temp->right;
		++matches;
	}

	if (temp == NULL)
		temp = this;

	if (matched != NULL)
		*matched = temp;

	return matches;
}


int Block::upMatch(Block **matched, bool ignoreActive) {
	int matches = 0;
	
	if (this->up == NULL)
		return 0;

	Block *temp = this;
	while (match(temp->up, ignoreActive))  {
		temp = temp->up;
		++matches;
	}

	if (temp == NULL)
		temp = this;

	if (matched != NULL)
		*matched = temp;

	return matches;
}

int Block::downMatch(Block **matched, bool ignoreActive) {
	int matches = 0;
	
	if (this->down == NULL)
		return 0;

	Block *temp = this;
	while (match(temp->down, ignoreActive))  {
		temp = temp->down;
		++matches;
	}

	if (temp == NULL)
		temp = this;

	if (matched != NULL)
		*matched = temp;

	return matches;
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