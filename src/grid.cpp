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

const string Grid::actionLabels[Grid::_NUMBER_OF_ACTIONS] = {
	"Swap",
	"Push"};

void Grid::declareActions()
{
	Input::Action::ActionScope scope = Input::Action::SCOPE_FIRST_PLAYER;
	// FIXME: I have a hunch that SCOPE_CURRENT_PLAYER stuff is broken, so
    //	      comment it out and use SCOPE_FIRST_PLAYER for now
	//Input::Action::ActionScope scope = Input::Action::SCOPE_CURRENT_PLAYER;
	Triforce::GameState state = Triforce::PLAY;

	Input::declareAction(scope, state, ACT_SWAP, actionLabels[ACT_SWAP]);
	Input::declareAction(scope, state, ACT_PUSH, actionLabels[ACT_PUSH]);
}

void Grid::doAction(void *gridInstance, int actionState, int actionType)
{
	Grid *g = (Grid *)gridInstance;
	switch((enum Input::Action::ActionState)actionState)
	{
	case Input::Action::STATE_PRESS:
		switch((enum Actions)actionType)
		{
		case ACT_SWAP:
			g->swapBlocks();
			break;
		case ACT_PUSH:
			if (g->getState() == Grid::play)
				g->pushRow();
			break;
		}
	/*
	case Input::Action::STATE_HOLD:
	case Input::Action::STATE_RELEASE:
	*/
	}
}


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
	last_cursor_anim = 0;
	timer_cursor_anim = 50;
	state = play;
	current_cursor_frame = 0;

	int startingRows = nrows / 2 - 1;

	for (int row=0; row < startingRows; ++row)
		addRow();

	for (int row = 0; row < startingRows; ++row) 
		for (int col = 0; col < ncols; ++col) 
			blocks[row][col]->offsetY( -1 * block_h * row );

	//Input::defineActions(Input::Action::SCOPE_CURRENT_PLAYER, Triforce::PLAY, this, doAction);
	Input::defineActions(Input::Action::SCOPE_FIRST_PLAYER, Triforce::PLAY, this, doAction);
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

	Block *row = blocks[0][0];
	while (row != NULL) {
		Block *col = row;
		while (col != NULL) {
			col->display();
			col = col->right;
		}
		row = row->up;
	}
	cursor->draw(current_cursor_frame);
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

	if (mainTimer->elapsed(last_cursor_anim, timer_cursor_anim)) {
		if (++current_cursor_frame >= 10)
			current_cursor_frame = 0;
		last_cursor_anim = mainTimer->time();
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
			if (blocks[count][i]->getState() == Block::enabled || blocks[count][i]->getState() == Block::inactive) {
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
			blocks[0][col]->changeState(Block::enabled);
	
	blocks.push_front(new Block *[ncols]);

	for (int col=0; col<ncols; ++col) {
		blocks[0][col] = new Block(this);
		blocks[0][col]->left = (col > 0) ? blocks[0][col - 1] : NULL;
		blocks[0][col]->up = (blocks.size() > 1) ? blocks[1][col] : NULL;
		blocks[0][col]->right = NULL;
		blocks[0][col]->down = NULL;

		/* Randomize the blocks without generating combos */
		do    ( blocks[0][col]->init(blockSprites[ rand() % nblocktypes ], gridPos.x + col * block_w, gridPos.y) );
		while ( blocks[0][col]->leftMatch(true) >= 2 || blocks[0][col]->upMatch(true) >= 2 );
	}

	for (int i = 0; i < ncols - 1; ++i)
		blocks[0][i]->right = blocks[0][i + 1];

	if (blocks.size() > 3) 
		for (int i=0; i<ncols; ++i)  
			blocks[1][i]->detectAndSetComboState();

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
		if (!temp->detectAndSetComboState())
			temp->detectAndSetFallState();
		if (!temp->right->detectAndSetComboState())
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