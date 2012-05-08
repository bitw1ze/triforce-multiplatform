/*	grid.cpp
	by Gabe Pike
	Changes:
		- added game states to the grid
		- detect combos and break blocks when a combo is found
		- added destructor
		- documented all the methods
*/

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
	for (uint32 i=0; i<blocks.size(); ++i)
		for (uint32 j=0; j<ncols; ++j)
			blocks[i][j].display();
	cursor->draw(0);
}

void Grid::composeFrame() {
	GridEvent::composeFrame(this);

	for (uint32 i=0; i<blocks.size(); ++i) 
		for (uint32 j=0; j<ncols; ++j)
			blocks[i][j].composeFrame();

	switch (state) {
	case play:
		if (mainTimer->elapsed(last_push, timer_push)) {
			pushRow();
			last_push = mainTimer->time();
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
		bool isDisabled = true;
		for (int i=0; i<ncols; ++i) {
			if (blocks[count][i].getState() == Block::enabled || blocks[count][i].getState() == Block::inactive) {
				isDisabled = false;
				break;
			}
		}
	
		if (isDisabled == true)
			break;
	}

	return count;
}

/*	pushRow
	Gradually push a new row onto the play area. */
void Grid::pushRow() {
	grid_yoff += grid_yspeed;

	cursor->offsetY(-grid_yspeed);
	for (uint32 i=0; i<blocks.size(); ++i) 
		for (uint32 j=0; j<ncols; ++j) 
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
		for (vector<Block>::iterator it = blocks[0].begin(); it != blocks[0].cend(); ++it)
			(*it).changeState(Block::enabled);
	
	vector<Block> newRow(ncols);
	blocks.push_front(newRow);

	for (int col=0; col<ncols; ++col) {
		// Randomize the blocks without generating combos
		do {
			blocks[0][col].init(blockSprites[rand() % nblocktypes], gridPos.x + col * block_w, gridPos.y);
		} while ( leftMatch(0, col, true) >= 2 || upMatch(0, col, true) >= 2 );
		newRow[col].grid = this;
	}

	GridEvent combo(this);
	if (blocks.size() > 3) {
		for (int i=0; i<ncols; ++i) { 
			GridEvent::detectCombo(this, Cell(1, i));
		}
	}
}

void Grid::incComboInterval(int interval) {
	timer_combo += interval;
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

	if (r >= (int)blocks.size())
		return;

	if (swap(blocks[r][c1], blocks[r][c2])) {
		if (!GridEvent::detectCombo(this, Cell(r, c1)))
			GridEvent::detectFall(this, Fall(r, c1));

		if (!GridEvent::detectCombo(this, Cell(r, c2)))
			GridEvent::detectFall(this, Fall(r, c2));
	}
}

/*	leftMatch, rightMatch, downMatch, and upMatch
	These functions are all used to detect combos. They return the number of blocks
	that match the block passed to the direction in the function's name.
	e.g. a 3 combo will return 2		*/

int Grid::leftMatch(int r, int c, bool ignoreActive) {
	int matches = 0;

	if (c > 0 && c < ncols && r >= 0 && r < (int)blocks.size()) {
		for (int i=c-1; i >= 0; --i) {
			if (match(blocks[r][c], blocks[r][i], ignoreActive)) 
				++matches;
			else
				break;
		}
	}

	return matches;
}

int Grid::rightMatch(int r, int c, bool ignoreActive) {
	int matches = 0;

	if (c < ncols - 1 && c >= 0 && r < (int)blocks.size() && r >= 0) {
		for (int i=c+1; i < ncols; ++i) {
			if (match(blocks[r][c], blocks[r][i], ignoreActive)) 
				++matches;
			else
				break;
		}
	}

	return matches;
}

int Grid::upMatch(int r, int c, bool ignoreActive) {
	int matches = 0;

	if (r < (int)blocks.size() - 1 && r >= 0 && c >= 0 && c < ncols) {
		for (int i=r+1; i < (int)blocks.size(); ++i) {
			if (match(blocks[r][c], blocks[i][c], ignoreActive)) 
				++matches;
			else
				break;
		}
	}

	return matches;
}

int Grid::downMatch(int r, int c, bool ignoreActive) {
	int matches = 0;

	if (r > 0) {
		for (int i=r-1; i > 0; --i) {
			if (match(blocks[r][c], blocks[i][c], ignoreActive)) 
				++matches;
			else
				break;
		}
	}

	return matches;
}

/* destructor */
Grid::~Grid() {
	delete cursor;
}

/*	swap
	Swap two adjacent blocks that the cursor has selected. */
bool swap(Block &left, Block &right) {
	if ( left.getState() == Block::combo || left.getState() == Block::fall ||
		right.getState() == Block::combo || right.getState() == Block::fall)
		return false;

	Block temp = left;
	left = right;
	right = temp;

	return true;
}

/*	match
	Detect a single match of one block to another block based on the sprite.
	ignoreActive should be enabled when only testing if block states and sprites
	match, such as when generating blocks */
bool match(const Block &left, const Block &right, bool ignoreActive) { 

	Block::gameState ls = left.getState();
	Block::gameState rs = right.getState();

	if (left.getSprite() != right.getSprite())
		return false;

	if (ignoreActive) 
		return (ls == Block::enabled || ls == Block::inactive) && (rs == Block::enabled || rs == Block::inactive);
	else
		return (ls == Block::enabled && rs == Block::enabled);

	return false;
}
