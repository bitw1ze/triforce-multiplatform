#include "game.h"

/* Grid methods */
Grid::Grid(GameEnv *ge) {
	blockSprites = ge->blockSprites;
	row_top = 0;
	block_w = blockSprites[0]->GetWidth();
	block_h = blockSprites[0]->GetHeight();
	grid_x = ge->getWidth()/2 - (block_w * ncols)/2;
	grid_y = ge->getHeight() - block_h * 2;

	for (int row=0; row<nrows/2; ++row) {
		pushRow();
	}
}

void Grid::pushRow() {
	if (blocks.size() == nrows)
		blocks.pop_back();

	Block *blockRow = new Block[ncols];

	for (int col=0; col<ncols; ++col) {
		blockRow[col].create(grid_x + col * block_w, grid_y - block_h * 2, 0, 0, blockSprites[ rand() % nblocktypes ], mainTimer);
	}

	if (row_top < nrows)
		++row_top;

	blocks.push_front(blockRow);
}

void Grid::display() {
	for (deque<Block *>::iterator it = blocks.begin(); it < blocks.end(); ++it)
		for (int j=0; j<ncols; ++j) 
			if ((*it)[j].enabled)
				(*it)[j].draw(0);
}

void Grid::setCoords() {
	int i = 0;
	for (deque<Block *>::iterator it = blocks.begin(); it < blocks.end(); ++it, ++i)
		for (int j=0; j<ncols; ++j) 
			if ((*it)[j].enabled) 
				(*it)[j].setY(grid_y - i * block_h);
}