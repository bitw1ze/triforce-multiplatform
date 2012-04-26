/*	grid.cpp
	by Gabe Pike */

#include "game.h"

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

	for (int row=0; row< nrows/2 * 12; ++row) {
		pushRow();
	}
}

/*	display
	Call the draw function on all blocks, then draw the cursor over it. */
void Grid::display() {
	for (deque<Block **>::iterator it = blocks.begin(); it < blocks.end(); ++it)
		for (int j=0; j<ncols; ++j) 
			if ((*it)[j]->getState() == Block::enabled)
				(*it)[j]->draw(0);
	cursor->draw(0);
}

/*	pushRow
	Gradually push a new row onto the play area. */
void Grid::pushRow() {
	grid_yoff += grid_yspeed;

	cursor->offsetY(-grid_yspeed);
	for (uint32 i=0; i<blocks.size(); ++i) {
		for (int j=0; j<ncols; ++j) {
			blocks[i][j]->offsetY(-grid_yspeed);
		}
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
	if (blocks.size() >= nrows)
		blocks.pop_back();

	bool combo;
	blocks.push_front(new Block *[ncols]);

	for (int col=0; col<ncols; ++col) {
		blocks[0][col] = new Block();

		combo = false;
		/* Randomize the blocks without generating combos */
		do    ( blocks[0][col]->init(blockSprites[ rand() % nblocktypes ], gridPos.x + col * block_w, gridPos.y) );
		while ( leftMatch(0, col) >= 2 || upMatch(0, col) >= 2 );
		
	}
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

	blocks[r][c1]->swap(*blocks[r][c2]);

	onCombo( detectCombos(r, c1) );
	onCombo( detectCombos(r, c2) );
}

/*	killRows
	Called when a combo occurs. It will change the state to a 'freeze' state,
	where all the blocks will stop for a certain amount of time before breaking. */
void Grid::onCombo(Cell cells[4]) {
	int _row, _col;

	for (int i=0; i<4; ++i) 
		printf("(%d, %d) ", cells[i].row, cells[i].col);
	printf("\n");

	_row = cells[0].row;
	if (cells[0].col != -1) {
		for (_col = cells[0].col; _col <= cells[1].col; ++_col)
			blocks[_row][_col]->changeState( Block::disabled );
	}
	_col = cells[2].col;
	if (cells[2].col != -1) {
		for (_row = cells[2].row; _row <= cells[3].row; ++_row)
			blocks[_row][_col]->changeState( Block::disabled );
	}
}

/*	leftMatch, rightMatch, downMatch, and upMatch
	These functions are all used to detect combos. They return the number of blocks
	that match the block passed to the direction in the function's name.
	e.g. a 3 combo will return 2		*/

int Grid::leftMatch(int row, int col) {
	int matches = 0;

	if (col <= 0)
		return matches;
	
	for (int c=col-1; c >= 0; --c) {
		if (blocks[row][c]->match(blocks[row][col])) 
			++matches;
		else
			break;
	}

	return matches;
}

int Grid::rightMatch(int row, int col) {
	int matches = 0;

	if (col >= ncols - 1)
		return matches;

	for (int c=col+1; c < ncols; ++c) {
		if (blocks[row][c]->match(blocks[row][col])) 
			++matches;
		else
			break;
	}

	return matches;
}


int Grid::upMatch(int row, int col) {
	int matches = 0;

	if (row == getTopRow())
		return matches;

	for (int r=row+1; r<=getTopRow(); ++r) {
		if (blocks[r][col]->match(blocks[row][col]))
			++matches;
		else
			break;
	}

	return matches;
}

int Grid::downMatch(int row, int col) {
	int matches = 0;

	if (row <= 0)
		return matches;

	for (int r=row-1; r>=0; --r) {
		if (blocks[r][col]->match(blocks[row][col]))
			++matches;
		else
			break;
	}

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
	horizontal match. 

	The final set of cells is returned at the end. It will be useful to pass this array
	of cells to onCombo(), which handles the event of a combo. */
Cell *Grid::detectCombos(int r, int c) {
	int left, right, up, down;
	Cell *cells = new Cell[4];
	for (int i=0; i<4; ++i)
		cells[i].col = cells[i].row = -1;

	left = leftMatch(r, c);
	right = rightMatch(r, c);

	if ( left + right >= 2) {
		cells[0].row = cells[1].row = r;
		cells[0].col = c - left;
		cells[1].col = c + right;

		for (int i=cells[0].col; i <= cells[1].col; ++i) {
			up = upMatch(cells[0].row, i);
			down = downMatch(cells[0].row, i);
			if (up + down >= 2) {
				cells[2].col = cells[3].col = i;
				cells[2].row = cells[0].row - down;
				cells[3].row = cells[0].row + up;
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

	return cells;
}

/* destructor
	Delete all the blocks and the cursor */
Grid::~Grid() {
	delete cursor;
	for (uint32 i=0; i<blocks.size(); ++i)
		for (uint32 j=0; j<ncols; ++j)
			if (blocks[i][j] != NULL)
				delete blocks[i][j];
}