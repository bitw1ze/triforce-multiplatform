/*	block.cpp
	by Gabe Pike
	Changes:
		- Blocks now keep track of their own state
		- Put the swap function in Block
		- Block now handles its own display function based on states
*/

#include "game.h"

//int Block::interval_combo;

Block::Block(Grid *g) : CObject() {
	timer = new CTimer(); 
	timer->start();

	last_combo = -1;
	interval_combo = 1500;

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
		if (state != combo)
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

			if (temp->detectAndSetComboState())
				if (grid->getState() != Grid::combo)
					grid->changeState(Grid::combo);
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
		draw(0);
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
	return 
			right != NULL
		&&	getState() != disabled && right->getState() != disabled
		&&	getSprite() == right->getSprite()
		&&  (ignoreActive ? true : getState() != inactive && right->getState() != inactive);
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

	nleft = leftMatch();
	nright = rightMatch();

	if ( nleft + nright >= 2) {
		horiz = offsetCol(-nleft);
		
		cout << "hori: ";
		for (int i=-nleft; i <= nright; ++i) {
			cout << i << " ";
			horiz->changeState(combo);
			nup = horiz->upMatch();
			ndown = horiz->downMatch();

			if (nup + ndown >= 2) {
				vert = offsetRow(-ndown);

				for (int i=-ndown; i <= nup; ++i) {
					vert->changeState(combo);
					vert = vert->up;
				}
			}

			horiz = horiz->right;
		}

		cout << endl;
		return true;
	}
	else {
		ndown = downMatch();
		nup = upMatch();

		if ( ndown + nup >= 2) {
			vert = offsetRow(-ndown);

			for (int i=-ndown; i <= nup; ++i) {
				vert->changeState(combo);

				nleft = vert->leftMatch();
				nright = vert->rightMatch();

				if (nleft + nright >= 2) {
					horiz = offsetCol(-nleft);

					for (int i=-nleft; i <= nright; ++i) {
						horiz->changeState(combo);
						horiz = horiz->right;
					}
				}

				vert = vert->up;
			} 

			return true;
		}
	}

	return false;
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