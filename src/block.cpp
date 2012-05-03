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


Block::Block(Grid *g) : CObject() {
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
	grid = g;
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
			detectAndSetFallState();
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
			Block *temp = this;
			for (int i=0; i<n; ++i)
				temp = temp->down;
			temp->setSprite(getSprite());
			temp->changeState(enabled);
			state = disabled;

			temp->detectAndSetComboState();
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
bool Block::match(const Block *right, bool ignoreActive) const { 
	if (right == NULL)
		return false;

	gameState ls = getState();
	gameState rs = right->getState();

	if (getSprite() != right->getSprite())
		return false;

	if (ignoreActive) 
		return (ls == enabled || ls == inactive) && (rs == enabled || rs == inactive);
	else
		return (ls == enabled && rs == enabled);
}

/*	detectAndSetComboState
	This function uses the match<direction> functions to find all possible combos.
	A combo can be stored as a set of Cells, where a Cell is a struct that holds the
	row and column of a block. A combo in one direction (e.g. 3 blocks in a row 
	horizontal) is stored in 2 cells, and a combo in two directions (e.g. 5 blocks 
	vertical and 3 blocks horizontal) is stored in 4 cells.
	
	This function computes the cells by first finding a horizontal match. If a match
	is found, it will then iteratively search for a vertical from each block in the
	horizontal match.  */
bool Block::detectAndSetComboState() {
	Block *horiz = NULL, *vert = NULL;
	int nleft, nright, nup, ndown;

	list<Block *> combos;

	nleft = leftMatch();
	nright = rightMatch();

	if ( nleft + nright >= 2) {
		horiz = offsetCol(-nleft);
		
		for (int i=-nleft; i <= nright; ++i) {
			combos.push_back(horiz);
			nup = horiz->upMatch();
			ndown = horiz->downMatch();

			if (nup + ndown >= 2) {
				vert = offsetRow(-ndown);

				for (int i=-ndown; i <= nup; ++i) {
					combos.push_back(vert);
					vert = vert->up;
				}
			}

			horiz = horiz->right;
		}
	}
	else {
		ndown = downMatch();
		nup = upMatch();

		if ( ndown + nup >= 2) {
			vert = offsetRow(-ndown);

			for (int i=-ndown; i <= nup; ++i) {
					combos.push_back(vert);

				nleft = vert->leftMatch();
				nright = vert->rightMatch();

				if (nleft + nright >= 2) {
					horiz = offsetCol(-nleft);

					for (int i=-nleft; i <= nright; ++i) {
						combos.push_back(horiz);
						horiz = horiz->right;
					}
				}

				vert = vert->up;
			} 
		}
	}

	if (combos.size() == 0)
		return false;

	int interval = combos.size() * Block::interval_combo;
	for (list<Block *>::iterator it = combos.begin(); it != combos.cend(); it++) {
		(*it)->total_combo_interval = interval;
		(*it)->changeState(combo);
	}

	return true;
}

void Block::detectAndSetFallState() {
	Block *temp = NULL;
	int nfall;

	if (state == disabled && up != NULL && up->getState() == enabled) {
		nfall = 1;
		
		temp = down;
		while (temp != NULL && temp->getState() == disabled) {
			++nfall;
			temp = temp->down;
		}

		temp = up;
		while (temp != NULL && temp->getState() == enabled) {
			temp->setFallCount(nfall);
			temp->changeState(fall);
			temp = temp->up;
		}
	}
	else if (state == enabled && down != NULL && down->getState() == disabled) {
		nfall = 0;

		temp = down;
		while (temp != NULL && temp->getState() == disabled) {
			++nfall;
			temp = temp->down;
		}

		changeState(fall);
		setFallCount(nfall);
	}
}

void Block::transferDown() {

}

/*	leftMatch, rightMatch, downMatch, and upMatch
	These functions are all used to detect combos. They return the number of blocks
	that match the block passed to the direction in the function's name.
	e.g. a 3 combo will return 2		*/

int Block::leftMatch(bool ignoreActive) {
	int matches = 0;
	
	if (this->left == NULL)
		return 0;

	Block *temp = this;
	while (match(temp->left, ignoreActive))  {
		temp = temp->left;
		++matches;
	}

	return matches;
}

int Block::rightMatch(bool ignoreActive) {
	int matches = 0;
	
	if (this->right == NULL)
		return 0;

	Block *temp = this;
	while (match(temp->right, ignoreActive))  {
		temp = temp->right;
		++matches;
	}

	return matches;
}


int Block::upMatch(bool ignoreActive) {
	int matches = 0;
	
	if (this->up == NULL)
		return 0;

	Block *temp = this;
	while (match(temp->up, ignoreActive))  {
		temp = temp->up;
		++matches;
	}

	return matches;
}

int Block::downMatch(bool ignoreActive) {
	int matches = 0;
	
	if (this->down == NULL)
		return 0;

	Block *temp = this;
	while (match(temp->down, ignoreActive))  {
		temp = temp->down;
		++matches;
	}

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

Block * Block::offsetRow(int n) {
	Block *temp = this;

	if (n > 0) {
		while (--n >= 0 && temp != NULL) 
			temp = temp->up;
	}
	else {
		n = -n;
		while (--n >= 0 && temp != NULL)
			temp = temp->down;
	}

	return temp ? temp : this;
}

Block * Block::offsetCol(int n) {
	Block *temp = this;

	if (n > 0) {
		while (--n >= 0 && temp != NULL) 
			temp = temp->right;
	}
	else {
		n = -n;
		while (--n >= 0 && temp != NULL)
			temp = temp->left;
	}

	return temp ? temp : this;
}