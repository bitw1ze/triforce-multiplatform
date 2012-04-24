#include "game.h"

/* Grid methods */
Grid::Grid(GamePlay *ge, CBaseSprite *cursorSprite) {
	gamePlay = ge;
	blockSprites = ge->blockSprites;
	block_w = blockSprites[0]->GetWidth();
	block_h = blockSprites[0]->GetHeight();
	grid_yspeed = block_h / 12;
	grid_yoff = 0;
	gridPos.x = ge->getWidth()/2 - (block_w * ncols)/2;
	gridPos.y = ge->getHeight() - (block_h * 2);
	cursor = new Cursor(this, cursorSprite);

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
		CBaseSprite *newBlock = NULL;
		/* Randomize the blocks without generating combos */
		do    ( newBlock = blockSprites[ rand() % nblocktypes ] );
		while ( leftMatch(0, col) >= 3 || upMatch(0, col) >= 3 );
		blocks[0][col]->init(newBlock, gridPos.x + col * block_w, gridPos.y);
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

	//checkMatches(r, c1);
}

void Grid::checkMatches(int r, int c) {
	//int left, right, up, down;
}

int Grid::leftMatch(int row, int col) {
	int matches = 1;

	if (col <= 0)
		return matches;
	
	for (int c=col-1; c >= 0; --c) {
		if (blocks[row][c]->match(*blocks[row][col])) 
			++matches;
		else
			break;
	}

	return matches;
}

int Grid::rightMatch(int row, int col) {
	int matches = 1;

	if (col >= ncols - 1)
		return matches;

	for (int c=col+1; c < ncols; ++c) {
		if (blocks[row][c]->match(*blocks[row][col])) 
			++matches;
		else
			break;
	}

	return matches;
}


int Grid::upMatch(int row, int col) {
	int matches = 1;

	if (row == getTopRow())
		return matches;

	for (int r=row+1; r<=getTopRow(); ++r) {
		if (blocks[r][col]->match(*blocks[row][col]))
			++matches;
		else
			break;
	}

	return matches;
}

int Grid::downMatch(int row, int col) {
	int matches = 1;

	if (row <= 0)
		return matches;

	for (int r=row-1; r>=0; --r) {
		if (blocks[r][col]->match(*blocks[row][col]))
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