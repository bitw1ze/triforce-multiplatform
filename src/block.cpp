/*	block.cpp
	by Gabe Pike
	Changes:
		- Blocks now keep track of their own state
		- Put the swap function in Block
		- Block now handles its own display function based on states
*/

#include "game.h"

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

void Block::setStateTimer(gameState newState, gameState nextst, int t) {
	last_state = mainTimer->time();
	timer_state = t;
	changeState(newState);
	nextState = nextst;
}

void Block::composeFrame() {
	if (last_state > 0 && mainTimer->elapsed(last_state, timer_state)) {
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
			getState() == right->getState()
		&&	(ignoreActive ? true : (isActive() == true && right->isActive() == true) )
		&&	getState() == enabled
		&&	getSprite() == right->getSprite();
}

/*	leftMatch, rightMatch, downMatch, and upMatch
	These functions are all used to detect combos. They return the number of blocks
	that match the block passed to the direction in the function's name.
	e.g. a 3 combo will return 2		*/

Block * Block::leftMatch(bool ignoreActive) {
	if (left == NULL)
		return NULL;
	
	Block *temp = this->left;
	while (match(temp, ignoreActive)) 
		temp = temp->left;

	return temp;
}

Block * Block::rightMatch(bool ignoreActive) {
	if (right == NULL)
		return NULL;
	
	Block *temp = this->right;
	while (match(temp, ignoreActive)) 
		temp = temp->right;

	return temp;
}


Block * Block::upMatch(bool ignoreActive) {
	if (up == NULL)
		return NULL;
	
	Block *temp = this->up;
	while (match(temp, ignoreActive)) 
		temp = temp->up;

	return temp;
}

Block * Block::downMatch(bool ignoreActive) {
	if (down == NULL)
		return NULL;
	
	Block *temp = this->down;
	while (match(temp, ignoreActive)) 
		temp = temp->down;

	return temp;
}

/*	detectCombos
	This function uses the match<direction> functions to find all possible combos.
	A combo can be stored as a set of Cells, where a Cell is a struct that holds the
	row and column of a block. A combo in one direction (e.g. 3 blocks in a row 
	horizontal) is stored in 2 cells, and a combo in two directions (e.g. 5 blocks 
	vertical and 3 blocks horizontal) is stored in 4 cells.
	
	This function computes the cells by first finding a horizontal match. If a match
	is found, it will then iteratively search for a vertical from each block in the
	horizontal match. 

	The final set of cells is returned at the end. It will be useful to pass this array
	of cells to onCombo(), which handles the event of a combo. */
bool Block::detectCombos() {
	Block *mleft, *mright, *mup, *mdown;
	Cell *cells = new Cell[4];
	for (int i=0; i<4; ++i)
		cells[i].col = cells[i].row = -1;

	mleft = leftMatch();
	mright = rightMatch();

	if ( mleft + mright >= 2) {
		Block *temp = this - mleft;
		while (--left)
			temp = temp->left;

		while (temp <= right) {
			temp->changeState(combo);

			mup = upMatch();
			mdown = downMatch();
			if (mup + mdown >= 2) {
				Block *temp2 = mdown;
				while (temp2 <= 
				break;
			}
		}
	}
	else {
		down = downMatch(r, c);
		up = upMatch(r, c);
		if ( up + down >= 2) {
			cells[2].col = cells[3].col = c;
			cells[2].row = r - down;
			cells[3].row = r + up;

			for (int i=cells[2].row; i <= cells[3].row; ++i) {
				left = leftMatch(i, cells[2].col);
				right = rightMatch(i, cells[2].col);
				if (left + right >= 2) {
					cells[0].row = cells[1].row = i;
					cells[0].col = cells[2].col - left;
					cells[1].col = cells[2].col + right;
					break;
				}
			}
		}
	}

	if (cells[0].row != -1 || cells[2].row != -1) {
		currentCombo = cells;
		return true;
	}
	else {
		currentCombo = NULL;
		return false;
	}
}

void Block::setFallStates() {
	int row, col;

	row = currentCombo[0].row;
	if (row != -1) {
		for (col=currentCombo[0].col; col <= currentCombo[1].col; ++col) {
			int num_falls = upMatch(row, col) + downMatch(row, col) + 1;
			for (int r = row + 1; r< blocks.size(); ++ r) {
				if (blocks[r][col]->getState() == Block::enabled) {
					blocks[r][col]->setFalls(num_falls);
					blocks[r][col]->changeState(Block::fall);
				}
				else if (blocks[r][col]->getState() == Block::combo && row <= currentCombo[3].row)
					continue;
				else
					break;
			}
		}
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