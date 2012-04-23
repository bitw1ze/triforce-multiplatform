#include "game.h"

/* Grid methods */
Grid::Grid(GameEnv *ge, CBaseSprite *cursorSprite) {
	gameEnv = ge;
	blockSprites = ge->blockSprites;
	block_w = blockSprites[0]->GetWidth();
	block_h = blockSprites[0]->GetHeight();
	grid_yspeed = block_h / 12;
	grid_yoff = 0;
	grid_x = ge->getWidth()/2 - (block_w * ncols)/2;
	grid_y = ge->getHeight() - (block_h * 2);
	cursor = new Cursor(this, cursorSprite);

	for (int row=0; row< nrows/2 * 12; ++row) {
		pushRow();
	}
}

void Grid::pushRow() {
	grid_yoff += grid_yspeed;

	cursor->offsetY(-grid_yspeed);
	for (int i=0; i<blocks.size(); ++i) {
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
	Block **blockRow = new Block *[ncols];

	for (int col=0; col<ncols; ++col) {
		blockRow[col] = new Block();

		combo = false;
		CBaseSprite *newBlock = NULL;

		/* Randomize the blocks without generating combos */
		do {
			newBlock = blockSprites[ rand() % nblocktypes ];
			/* Really ugly but basically it is checking that there are no combos of three to the
			   left or above. This would best be moved to another function later on. */
			combo =  ( (col >= 2 && blockRow[col-1]->match(newBlock) && blockRow[col-2]->match(newBlock)) 
				|| (blocks.size() >= 2 && blocks[0][col]->match(newBlock) && blocks[1][col]->match(newBlock)) );
		} while (combo);
		blockRow[col]->create(grid_x + col * block_w, grid_y, 0, 0, newBlock);
	}
	printf("grid_y: %d\nscreen_h: %d\n", getY(), gameEnv->getHeight());

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