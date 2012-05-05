/*	grid.cpp
	by Gabe Pike
	Changes:
		- added game states to the grid
		- detect combos and break blocks when a combo is found
		- added destructor
		- documented all the methods
*/

#include <algorithm>
#include "game.h"
#include "input.h"

/*	TODOs: 
		- Make the grid manage its own timer and control when rows are pushed.
		- Overload the draw function in blocks to control how they are drawn when
			called based on its state.
		- Inspect code and make sure that all modules have control over their own inner features.
*/

/*	constructor
	Initialize objects and vars such as position, dimensions, and speed.
	Also create the initial rows of the game */
Grid::Grid(GamePlay *gp) {
	gamePlay = gp;
	blockSprites = gp->blockSprites;
	block_w = blockSprites[0]->GetWidth();
	block_h = blockSprites[0]->GetHeight();
	grid_yspeed = block_h / 12;
	grid_yoff = 0;
	gridPos.x = gp->getWidth()/2 - (block_w * ncols)/2;
	gridPos.y = gp->getHeight() - (block_h * 2);
	cursor = new Cursor(this, gp->cursorSprite);
	last_push = 0;
	timer_push = 400;
	last_combo = 0;
	timer_combo = 0;
	last_fall = 0;
	timer_fall = 0;
	state = play;

	int startingRows = nrows / 2 - 1;

	for (int row=0; row < startingRows; ++row)
		addRow();

	for (int row = 0; row < startingRows; ++row) 
		for (int col = 0; col < ncols; ++col) 
			blocks[row][col].offsetY( -1 * block_h * row );
}

void Grid::changeState(gameState gs) {
	switch (gs) {
	case combo:
		if (state != combo) {
			timer_combo = Block::interval_combo;
			last_combo = mainTimer->time();
		}
		else
			timer_combo += Block::interval_combo;
		break;
	case play:
		last_push = mainTimer->time();
		break;
	}
	state = gs;
}

/*	display
	Call the draw function on all blocks, then draw the cursor over it. */
void Grid::display() {
	composeFrame();

	for (int i=0; i<countEnabledRows(); ++i)
		for (int j=0; j<ncols; ++j)
			blocks[i][j].display();
	cursor->draw(0);
}

void Grid::composeFrame() {
	for (int i=0; i<countEnabledRows(); ++i) 
		for (int j=0; j<ncols; ++j)
			blocks[i][j].composeFrame();

	switch (state) {
	case play:
		if (mainTimer->elapsed(last_push, timer_push)) {
			pushRow();
			last_push = mainTimer->time();
		}
		break;
	case combo:
		if (mainTimer->elapsed(last_combo, timer_combo)) {
			changeState(play);
		}
		break;
	}
}
bool Grid::containsPoint(int x, int y) {
	bool containsX = x > gridPos.x && x < gridPos.x + (int)ncols*block_h;
	bool containsY = !(y > gridPos.y - grid_yoff || y < gridPos.y - (int)(nrows)*block_h);
	return containsX && containsY;
}

int Grid::countEnabledRows() const {
	uint32 count;

	for (count=0; count<blocks.size(); ++count) {
		bool disabled = true;
		for (int i=0; i<ncols; ++i) {
			if (blocks[count][i].getState() == Block::enabled || blocks[count][i].getState() == Block::inactive) {
				disabled = false;
				break;
			}
		}
	
		if (disabled == true)
			break;
	}

	return count;
}

/*	pushRow
	Gradually push a new row onto the play area. */
void Grid::pushRow() {
	grid_yoff += grid_yspeed;

	cursor->offsetY(-grid_yspeed);
	for (int i=0; i<countEnabledRows(); ++i) 
		for (int j=0; j<ncols; ++j) 
			blocks[i][j].offsetY(-grid_yspeed);
	
	if (grid_yoff >= block_h) {
		grid_yoff = 0;
		cursor->shiftRow();
		addRow();
	}
}

/*	addRow
	Add a row to the grid. It will be pushed on to the bottom of the block
	matrix (front of deque). It will make sure not to generate combos since
	that is the player's job.	*/
void Grid::addRow() {
	if (blocks.size() > nrows) {
		blocks.pop_back();
	}

	if (blocks.size() > 0)
		for (int col=0; col<ncols; ++col)
			blocks[0][col].changeState(Block::enabled);
	
	vector<Block> newRow;
	for (int i=0; i<ncols; ++i)
		newRow[i].grid = this;
	
	blocks.push_front(newRow);

	for (int col=0; col<ncols; ++col) {
		// Randomize the blocks without generating combos
		do    ( blocks[0][col].init(blockSprites[ rand() % nblocktypes ], gridPos.x + col * block_w, gridPos.y) );
		while ( leftMatch(0, col, true).size() >= 2 || upMatch(0, col, true).size() >= 2 );
	}

	if (blocks.size() > 3) 
		for (int i=0; i<ncols; ++i)  
			;//setComboState(detectCombo(1, i));
}

/*	swapBlocks()
	Called by the input agent (keyboard or mouse) when the player wants to swap two
	adjacent blocks with the cursor. The state and the block sprite must be swapped to
	have the full effect. After the swap, combo detection is done and if there is a combo
	it is handled by the onCombo() subroutine.	*/
void Grid::swapBlocks() {
	int c1, c2, r;
	c1 = cursor->getCol();
	c2 = c1 + 1;
	r = cursor->getRow();

	if (r >= countEnabledRows())
		return;

	int nfalls;
	if (blocks[r][c1].swap(blocks[r][c2])) {
		if (detectCombo(r, c1).size() == 0)
			;//setFallState( detectFalls(r, c1) );
		if (detectCombo(r, c2).size() == 0) 
			;//setFallState( detectFalls(r, c2) );
	}
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
list<Block> & Grid::detectCombo(int r, int c) {
	list<Block> *combo = new list<Block>();
	list<Block> match1 = leftMatch(r, c);
	list<Block> match2 = rightMatch(r, c);
	
	/*

	if (combo->size() >= 2) {
		combo->insert(match1.begin(), match1.end());
		combo->push_back(blocks[r][c]);
		combo->insert(match2.begin(), match2.end());	
		
		int c1 = c - match1.size();
		int c2 = c + match2.size();
		for (int col = c1; col < c2; ++col) {
			match1 = downMatch(r, col);
			match2 = upMatch(r, col);

			if (match1.size() + match2.size() >= 2) {
				combo->insert(match1.begin(), match1.end());
				combo->insert(match2.begin(), match2.end());
				
				break;
			}
		}
	}
	else {
		match1 = downMatch(r, c);
		match2 = upMatch(r, c);

		if ( match1.size() + match2.size() >= 2) {
			combo->insert(match1.begin(), match1.end());
			combo->push_back(blocks[r][c]);
			combo->insert(match2.begin(), match2.end());

			int r1 = r - match1.size();
			int r2 = r + match2.size();
			for (int row=r1; row < r2; ++row) {
				match1 = leftMatch(row, c);
				match2 = rightMatch(row, c);

				if (match1.size() + match2.size() >= 2) {
					combo->insert(match1.begin(), match1.end());
					combo->insert(match2.begin(), match2.end());

				} 
			}
		}
	}
	*/
	return *combo;
}

list<Block> & Grid::detectFalls(int r, int c) {
	list<Block> *falls = new list<Block>();

	if (r >= countEnabledRows())
		return *falls;

	if (blocks[r][c].getState() == Block::disabled 
		&& blocks[r+1][c].getState() == Block::enabled) {
		
		falls->push_back(blocks[r][c]);
		for (int i=r+1; blocks[r][c].getState() == Block::enabled; ++i) 
			falls->push_back(blocks[i][c]);

	}
	else if (blocks[r][c].getState() == Block::enabled 
		&& blocks[r-1][c].getState() == Block::disabled) {
		
		falls->push_back(blocks[r][c]);
		for (int i=r-1; blocks[r][c].getState() == Block::disabled; --i)
			falls->push_back(blocks[i][c]);

	}
	return *falls;
}

list<Block> & setComboState(list<Block> &combo) {
	if (combo.size() < 1)
		return combo;

	int interval = combo.size() * Block::interval_combo;
	for (list<Block>::iterator it = combo.begin(); it != combo.cend(); it++) {
		(*it).setComboInterval(interval);
		(*it).changeState(Block::combo);
	}

	return combo;
}

list<Block> & setFallState(list<Block> &falls) {
	for (list<Block>::iterator it = falls.begin(); it != falls.cend(); ++it)
		(*it).changeState(Block::fall);

	return falls;
}

/*	leftMatch, rightMatch, downMatch, and upMatch
	These functions are all used to detect combos. They return the number of blocks
	that match the block passed to the direction in the function's name.
	e.g. a 3 combo will return 2		*/

list<Block> & Grid::leftMatch(int r, int c, bool ignoreActive) {
	list<Block> *matches = new list<Block>();

	if (c > 0)
		for (int i=c-1; i >= 0; --i)
			if (blocks[r][c].match(blocks[r][i], ignoreActive))
				matches->push_back(blocks[r][i]);

	return *matches;
}

list<Block> & Grid::rightMatch(int r, int c, bool ignoreActive) {
	list<Block> *matches = new list<Block>();

	if (c < ncols - 1)
		for (int i=c+1; i < ncols; --i)
			if (blocks[r][c].match(blocks[r][i], ignoreActive))
				matches->push_back(blocks[r][i]);

	return *matches;
}


list<Block> & Grid::upMatch(int r, int c, bool ignoreActive) {
	list<Block> *matches = new list<Block>();

	if (r < countEnabledRows() - 1)
		for (int i=r+1; i < countEnabledRows(); ++i)
			if (blocks[r][c].match(blocks[i][c], ignoreActive))
				matches->push_back(blocks[i][c]);

	return *matches;
}

list<Block> & Grid::downMatch(int r, int c, bool ignoreActive) {
	list<Block> *matches = new list<Block>();

	if (r > 0)
		for (int i=r-1; i > 0; --i)
			if (blocks[r][c].match(blocks[i][c], ignoreActive))
				matches->push_back(blocks[i][c]);

	return *matches;
}

/* destructor */
Grid::~Grid() {
	delete cursor;
}