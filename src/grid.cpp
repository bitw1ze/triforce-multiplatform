/*	grid.cpp
	by Gabe Pike
	Changes:
		- added game states to the grid
		- detect combos and break blocks when a combo is found
		- added destructor
		- documented all the methods
*/

#include "game.h"
#include <algorithm>

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
	//Block::setComboInterval(1500);
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
			blocks[row][col]->offsetY( -1 * block_h * row );
	
}

void Grid::changeState(gameState gs) {
	switch (gs) {
	case combo:
		if (state != combo) {
			timer_combo = 1500;
			last_combo = mainTimer->time();
		}
		else
			timer_combo += 1500;
		state = gs;
		
		break;
	case play:
		state = gs;
		last_push = mainTimer->time();
		break;
	}
}

/*	display
	Call the draw function on all blocks, then draw the cursor over it. */
void Grid::display() {
	composeFrame();

	Block *row = blocks[0][0];
	while (row != NULL) {
		Block *col = row;
		while (col != NULL) {
			col->display();
			col = col->right;
		}
		row = row->up;
	}
	cursor->draw(0);
}

void Grid::composeFrame() {
	Block *row = getBlock(0, 0);
	Block *col = NULL;
	while (row != NULL) {
		col = row;
		while (col != NULL) {
			col->composeFrame();
			col = col->right;
		}
		row = row->up;
	}

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

void Grid::passiveMouseHover(int x, int y) {
	// see if cursor is even inside playable part of grid, and set cursor type
	if (!(x > gridPos.x && x < gridPos.x + (int)ncols*block_h))
		glutSetCursor(GLUT_CURSOR_INHERIT);
	else if (y > gridPos.y - grid_yoff ||
	    y < gridPos.y - (int)(nrows)*block_h)
		glutSetCursor(GLUT_CURSOR_INHERIT);
	else
		cursor->passiveMouseHover(x, y);
}

int Grid::countEnabledRows() const {
	uint32 count;

	for (count=0; count<blocks.size(); ++count) {
		bool disabled = true;
		for (int i=0; i<ncols; ++i) {
			if (blocks[count][i]->getState() == Block::enabled) {
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
	Block *row = blocks[0][0];
	Block *col = NULL;
	while (row != NULL) {
		col = row;
		while (col != NULL) {
			col->offsetY(-grid_yspeed);
			col = col->right;
		}
		row = row->up;
	}
	
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
		delete [] blocks[nrows];
		blocks.pop_back();
	}

	if (blocks.size() > 0)
		for (int col=0; col<ncols; ++col)
			blocks[0][col]->setActive(true);
	
	blocks.push_front(new Block *[ncols]);

	for (int col=0; col<ncols; ++col) {
		blocks[0][col] = new Block();
		blocks[0][col]->left = (col > 0) ? blocks[0][col - 1] : NULL;
		blocks[0][col]->up = (blocks.size() > 1) ? blocks[1][col] : NULL;
		blocks[0][col]->right = NULL;
		blocks[0][col]->down = NULL;

		/* Randomize the blocks without generating combos */
		do    ( blocks[0][col]->init(blockSprites[ rand() % nblocktypes ], gridPos.x + col * block_w, gridPos.y) );
		while ( blocks[0][col]->leftMatch(NULL, true) >= 2 || blocks[0][col]->leftMatch(NULL, true) >= 2 );
	}

	for (int i = 0; i < ncols - 1; ++i)
		blocks[0][i]->right = blocks[0][i + 1];

	if (blocks.size() > 3) {
		for (int i=0; i<ncols; ++i)  {
			if (blocks[1][i]->detectAndSetComboState()) {
				changeState(combo);
			}
		}
	}

	if (blocks.size() > 1)
		for (int i=0; i<ncols; ++i) 
			blocks[1][i]->down = blocks[0][i];
}

/*	setCoords
	This function sets the coordinates of the entire block matrix. It was used early in 
	development and is obsolete as of now, but it is still here in case it is needed. */
void Grid::setCoords() {
	int i = 0;
	for (deque<Block **>::iterator it = blocks.begin(); it < blocks.end(); ++it, ++i)
		for (int j=0; j<ncols; ++j) 
			if ((*it)[j]->getState() == Block::enabled) 
				(*it)[j]->setY(gridPos.y - i * block_h);
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

	Block *temp = getBlock(r, c1);
	if (temp != NULL && temp->swap(*temp->right)) {
		if (temp->detectAndSetComboState())
			changeState(combo);
		else
			temp->detectAndSetFallState();
		
		if (temp->right->detectAndSetComboState())
			changeState(combo);
		else
			temp->right->detectAndSetFallState();
	}
}

Block * Grid::getBlock(int r, int c) {
	Block *row = blocks[0][0];
	Block *col = NULL;
	for (int i=0; i <= r && row != NULL; ++i, row = row->up) {
		col = row;
		for (int j=0; j < c && col != NULL; ++j, col = col->right) {
			//printf("(%d, %d) ", i, j);
		}
		//printf("\n");
	}

	return col;
}

/* destructor
	Delete all the blocks and the cursor */
Grid::~Grid() {
	delete cursor;
	/*
	for (uint32 i=0; i<blocks.size(); ++i)
		for (uint32 j=0; j<ncols; ++j)
			if (blocks[i][j] != NULL)
				delete blocks[i][j];
				*/
}