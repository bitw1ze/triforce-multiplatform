#include "game.h"

/* Grid methods */
Grid::Grid(GameEnv *ge, CBaseSprite *cursorSprite) {
	blockSprites = ge->blockSprites;
	block_w = blockSprites[0]->GetWidth();
	block_h = blockSprites[0]->GetHeight();
	speed = block_h / 4;
	grid_x = ge->getWidth()/2 - (block_w * ncols)/2;
	grid_y = ge->getHeight() - block_h * 2;

	for (int row=0; row<nrows/2; ++row) {
		pushRow();
	}

	cursor = new Cursor(this, cursorSprite);
}

void Grid::pushRow() {
	if (blocks.size() == nrows)
		blocks.pop_back();

	Block **blockRow = new Block *[ncols];

	for (int col=0; col<ncols; ++col) {
		blockRow[col] = new Block();
		blockRow[col]->create(grid_x + col * block_w, grid_y - block_h * 2, 0, 0, blockSprites[ rand() % nblocktypes ]);
	}

	blocks.push_front(blockRow);
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
				(*it)[j]->setY(grid_y - i * block_h);
}

void Grid::swapBlocks() {
	int c1, c2, r;
	c1 = cursor->getCol();
	c2 = c1 + 1;
	r = cursor->getRow();

	Block *temp = blocks[r][c1];
	blocks[r][c1] = blocks[r][c2];
	blocks[r][c2] = temp;

	blocks[r][c1]->setX(getX() + c1 * 48);
	blocks[r][c2]->setX(getX() + c2 * 48);
	blocks[r][c1]->draw(0);
	blocks[r][c2]->draw(0);
}