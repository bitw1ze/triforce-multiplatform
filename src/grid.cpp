#include "game.h"

/* Grid methods */
Grid::Grid(GamePlay *ge) {
	gamePlay = ge;
	blockSprites = ge->blockSprites;
	block_w = blockSprites[0]->GetWidth();
	block_h = blockSprites[0]->GetHeight();
	grid_yspeed = block_h / 12;
	grid_yoff = 0;
	gridPos.x = ge->getWidth()/2 - (block_w * ncols)/2;
	gridPos.y = ge->getHeight() - (block_h * 2);
	cursor = new Cursor(this, ge->cursorSprite);

	for (int row=0; row< nrows/2 * 12; ++row) {
		pushRow();
	}
}

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

void Grid::display() {
	for (deque<Block **>::iterator it = blocks.begin(); it < blocks.end(); ++it)
		for (int j=0; j<ncols; ++j) 
			if ((*it)[j]->enabled)
				(*it)[j]->draw(0);
	cursor->draw(0);
}

void Grid::setCoords() {
	int i = 0;
	for (deque<Block **>::iterator it = blocks.begin(); it < blocks.end(); ++it, ++i)
		for (int j=0; j<ncols; ++j) 
			if ((*it)[j]->enabled) 
				(*it)[j]->setY(gridPos.y - i * block_h);
}

void Grid::swapBlocks() {
	int c1, c2, r;
	c1 = cursor->getCol();
	c2 = c1 + 1;
	r = cursor->getRow();

	CBaseSprite *temp = blocks[r][c1]->getSprite();
	blocks[r][c1]->setSprite( blocks[r][c2]->getSprite() );
	blocks[r][c2]->setSprite( temp );

	detectCombos(r, c1);
	detectCombos(r, c2);
}

void Grid::detectCombos(int r, int c) {
	int left, right, up, down;
	Cell cells[4];
	for (int i=0; i<4; ++i)
		cells[i].col = cells[i].row = -1;

	if ( (left = leftMatch(r, c)) >= 2) {
		cells[0].row = cells[1].row = r;
		cells[0].col = c - left;
		cells[1].col = c;
	}
	else if ( (right = rightMatch(r, c)) >= 2) {
		cells[0].row = cells[1].row = r;
		cells[0].col = c;
		cells[1].col = c + right;
	}

	if ( left >= 2 || right >= 2) {
		for (int i=cells[0].col; i < cells[1].col; ++i) {
			up = upMatch(cells[0].row, i);
			down = downMatch(cells[0].row, i);
			if (up >= 2 && down > 2) {
				cells[2].col = cells[3].col = i;
				cells[2].row = cells[0].row - down;
				cells[3].row = cells[0].row + up;
				break;
			}
			else if (down >= 2) {
				cells[2].col = cells[3].col = i;
				cells[2].row = cells[0].row - down;
				cells[3].row = cells[0].row;
				break;
			}
			else if (up >= 2) {
				cells[2].col = cells[3].col = i;
				cells[2].row = cells[0].row;
				cells[3].row = cells[0].row + up;
				break;
			}
		}

		for (int i=0; i<4; ++i)
			printf("(%d, %d) ", cells[i].row, cells[i].col);
		printf("\n");

		return;
	}
	
	if ( (down = downMatch(r, c)) >= 2 ) {
		cells[2].col = cells[3].col = c;
		cells[2].row = r - down;
		cells[3].row = r;
	}
	else if ( (up = upMatch(r, c)) >= 2) {
		cells[2].col = cells[3].col = c;
		cells[2].row = r;
		cells[3].row = r + up;
	}

	if ( down >= 2 || up >= 2) {
		for (int i=cells[2].row; i < cells[3].row; ++i) {
			left = leftMatch(i, cells[2].col);
			right = rightMatch(i, cells[2].col);
			if (left >= 2 && right > 2) {
				cells[0].row = cells[1].row = i;
				cells[0].col = cells[2].col - left;
				cells[1].col = cells[2].col + right;
				break;
			}
			else if (left >= 2) {
				cells[0].row = cells[3].row = i;
				cells[0].col = cells[2].col - left;
				cells[1].col = cells[2].col;
				break;
			}
			else if (right >= 2) {
				cells[0].row = cells[3].row = i;
				cells[0].col = cells[2].col;
				cells[1].col = cells[2].col + right;
				break;
			}
		}
		for (int i=0; i<4; ++i) 
			printf("(%d, %d) ", cells[i].row, cells[i].col);
		printf("\n");

		return;
	}
}

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

Grid::~Grid() {
	delete cursor;
	for (uint32 i=0; i<blocks.size(); ++i)
		for (uint32 j=0; j<ncols; ++j)
			if (blocks[i][j] != NULL)
				delete blocks[i][j];
}