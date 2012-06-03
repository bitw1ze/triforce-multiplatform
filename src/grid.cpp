/*	grid.cpp

A Grid represents a single player. It is responsible displaying all the
objects inside the Grid (Blocks and Cursor). It contains functions to
display and compose the frame, swap and match Blocks, and push/add new 
Blocks onto the Grid.

The Grid also controls events that occur. It detects combos and falls,
and maintains a list of combo, fall, and bonus events. It calls the
methods in Fall, Combo, and Bonus to interact with them.
*/

#include "game.h"
#include "input.h"

const int Grid::forcedPushSpeed = 1;
const int Grid::forcedPushinterval = 8;

/*	constructor
	Initialize objects and vars such as position, dimensions, and speed.
	Also create the initial rows of the game */
Grid::Grid() {
	set();
	init();
}

void Grid::set() {
	blockSprites = GamePlay::blockSprites;
	
	pushSpeed = 2;
	pushInterval = 300;
	pushOffset = 0;
	lastPush = 0;
	lastForcedPush = 0;

	pushAccel = 0.9; // reduces the time it takes to push a row
	pushAccelInterval = 25 * 1000; // seconds
	lastPushAccel = 0;

	comboInterval = 0; // FIXME: put in Combo

	gridPos.x = GamePlay::getWidth()/2 - (GamePlay::blockLength * ncols)/2;
	gridPos.y = GamePlay::getHeight() - (GamePlay::blockLength * 2);

	cursor = new Cursor(this, GamePlay::cursorSprite);

	state = play;

	// FIXME: put in cursor
	last_cursor_anim = 0;
	timer_cursor_anim = 50;
	current_cursor_frame = 0;
}

void Grid::init() {
	int startingRows = nrows / 2 - 1;

	for (int row=0; row < startingRows; ++row)
		addRow();

	for (int row = 0; row < startingRows; ++row) 
		for (int col = 0; col < ncols; ++col) 
			blocks[row][col].offsetY( -1 * GamePlay::blockLength * row );

	defineActions();
}

Grid::Grid(const Grid &g) {
	clone(g);
}

Grid & Grid::operator =(const Grid &g) {
	clone(g);
	return *this;
}


void Grid::clone(const Grid &g) {
	// TODO
}

void Grid::declareActions()
{
	using namespace Input;
	using namespace PlayState;

	Action::ActionScope scope = Action::SCOPE_FIRST_PLAYER;
	Triforce::GameState state = Triforce::PLAY;

	declareAction(scope, state, SWAP, actionLabels[SWAP]);
	declareAction(scope, state, PUSH, actionLabels[PUSH]);
	declareAction(scope, state, PAUSE_TOGGLE, actionLabels[PAUSE_TOGGLE]);
}

void Grid::defineActions()
{
	using namespace Input;
	using namespace PlayState;

	Action::ActionScope scope = Action::SCOPE_FIRST_PLAYER;
	Triforce::GameState state = Triforce::PLAY;

	defineAction(scope, state, SWAP, this, doAction);
	defineAction(scope, state, PUSH, this, doAction);
	defineAction(scope, state, PAUSE_TOGGLE, this, doAction); // duplicate definition (OK)
}

void Grid::doAction(void *gridInstance, int actionState, int actionType)
{
	using namespace PlayState;

	Grid *g = (Grid *)gridInstance;
	switch((enum Input::Action::ActionState)actionState)
	{
	case Input::Action::STATE_PRESS:
		switch((enum Actions)actionType)
		{
		case SWAP:
			g->swapBlocks();
			break;
		case PUSH:
			if (g->getState() == Grid::play)
				g->changeState(push);
			break;
		case PAUSE_TOGGLE:
			g->printDebug();
			break;
		}
		break;
	case Input::Action::STATE_HOLD:
		switch((enum Actions)actionType)
		{
		case PUSH:
			if (g->getState() == Grid::push && mainTimer->elapsed(g->lastForcedPush, forcedPushinterval)) {
				g->pushRow(Grid::forcedPushSpeed);
				g->lastForcedPush = mainTimer->time();
			}
			break;
		}
		break;
	case Input::Action::STATE_RELEASE:
		switch((enum Actions)actionType)
		{
		case PUSH:
			if (g->getState() == Grid::push)
				g->changeState(play);
			break;
		}
	}
	glutPostRedisplay();
}

void Grid::changeState(gameState gs) {
	switch (gs) {
	case combo:
		break;
	case play:
		lastPush = mainTimer->time();
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
	cursor->draw(current_cursor_frame);
	cursor->alignToMouse();
}

void Grid::displayBonus() {
	for (list<Bonus>::iterator it = bonuses.begin(); it != bonuses.end(); ++it)
		it->display();
}

void Grid::composeFrame() {
	updateEvents();

	for (uint32 i=0; i<blocks.size(); ++i) 
		for (uint32 j=0; j<ncols; ++j)
			blocks[i][j].composeFrame();

	if (mainTimer->elapsed(last_cursor_anim, timer_cursor_anim)) {
		if (++current_cursor_frame >= 10)
			current_cursor_frame = 0;
		last_cursor_anim = mainTimer->time();
	}

	switch (state) {
	case play:
		// accelerate the speed that blocks are pushed
		if (mainTimer->elapsed(lastPushAccel, pushAccelInterval)) {
			pushInterval = (int)( (float)pushInterval * pushAccel );
			lastPushAccel = mainTimer->time();
		}

		if (mainTimer->elapsed(lastPush, pushInterval)) {
			pushRow(pushSpeed);
			lastPush = mainTimer->time();
		}
		break;

	}
}

bool Grid::containsPoint(int x, int y) {
	bool containsX = x > gridPos.x && x < gridPos.x + (int)ncols*GamePlay::blockLength;
	bool containsY = !(y > gridPos.y - pushOffset || y < gridPos.y - (int)(nrows)*GamePlay::blockLength);
	return containsX && containsY;
}

bool Grid::containsPoint(Point point) {
	return containsPoint(point.x, point.y);
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
void Grid::pushRow(int speed) {
	pushOffset += speed;

	cursor->offsetY(-speed);
	for (uint32 i=0; i<blocks.size(); ++i) 
		for (uint32 j=0; j<ncols; ++j) 
			blocks[i][j].offsetY(-speed);
	
	if (pushOffset >= GamePlay::blockLength) {
		pushOffset %= GamePlay::blockLength;
		cursor->shiftRow();
		addRow();
	}
}

/*	addRow
	Add a row to the grid. It will be pushed on to the bottom of the block
	matrix (front of deque). It will make sure not to generate combos since
	that is the player's job.	*/
void Grid::addRow() {
	if (blocks.size() > nrows - 1) 
		blocks.pop_back();

	if (blocks.size() > 0)
		for (vector<Block>::iterator it = blocks[0].begin(); it != blocks[0].cend(); ++it)
			(*it).changeState(Block::enabled);
	
	vector<Block> newRow(ncols);
	blocks.push_front(newRow);

	for (int col=0; col<ncols; ++col) {
		// Randomize the blocks without generating combos
		do {
			blocks[0][col].init(blockSprites[rand() % nblocktypes], gridPos.x + col * GamePlay::blockLength, gridPos.y);
		} while ( leftMatch(0, col, true) >= 2 || upMatch(0, col, true) >= 2 );
	}

	// increment row for all falls
	for (list<Fall>::iterator it = fallEvents.begin(); it != fallEvents.end(); ++it)
		(*it).adjustRow();

	if (blocks.size() > 3) {
		for (int i=0; i<ncols; ++i) {
			detectCombo(Cell(1, i), 0, true);
		}
	}
}

void Grid::incComboInterval(int interval) {
	comboInterval += interval;
}

/*	swapBlocks()
	Called by the input agent (keyboard or mouse) when the player wants to swap two
	adjacent blocks with the cursor. The state and the block sprite must be swapped to
	have the full effect. After the swap, combo detection is done and if there is a combo
	it is handled by the onCombo() subroutine.	*/
void Grid::swapBlocks() {
	unsigned int c1, c2, r;
	c1 = cursor->getCol();
	c2 = c1 + 1;
	r = cursor->getRow();

	if (r >= blocks.size())
		return;

	bool belowFall;
	if (r < blocks.size() - 1) {
		belowFall = blocks[r+1][c1].getState() == Block::fall 
			|| blocks[r+1][c2].getState() == Block::fall;
	}
	else
		belowFall = false;

	Combo combo1, combo2;

	if (!belowFall && swap(blocks[r][c1], blocks[r][c2])) {
		if (detectFall(r, c1)) 
			fallEvents.push_back(FallNode(r, c1));
		else if (detectFall(r+1, c1))
			fallEvents.push_back(FallNode(r+1, c1));
		else 
			combo1 = detectCombo(Cell(r, c1), 0, false);

		if (detectFall(r, c2))
			fallEvents.push_back(FallNode(r, c2));
		else if (detectFall(r+1, c2))
			fallEvents.push_back(FallNode(r+1, c2));
		else 
			combo2 = detectCombo(Cell(r, c2), 0, false);

		if (combo1 && combo2) {
			Cell cell = combo1.getState() == Combo::VERT ? *combo1.up() : *combo1.left();
			if (combo1.getChainCount() >= 2)
				++cell.row;

			bonuses.push_back(Bonus(cell, combo1.count() + combo2.count(), Bonus::COMBO, *this));
		}
		else if (combo1) {
			initBonus(combo1);
		}
		else if (combo2) {
			initBonus(combo2);
		}
	}
}

void Grid::initBonus(const Combo &combo) {
	if (combo.count() >= 4) {
		Cell cell = combo.getState() == Combo::VERT ? *combo.up() : *combo.left();
		if (combo.getChainCount() >= 2)
			++cell.row;

		bonuses.push_back(Bonus(cell, combo.count(), Bonus::COMBO, *this));
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
	Block::gameState ls = left.getState();
	Block::gameState rs = right.getState();

	//cout << "left yoff: " << left.getFallOffset() << endl;
	//cout << "right yoff: " << right.getFallOffset() << endl;

	if ( ls == Block::combo || rs == Block::combo ||
		ls == Block::fall || rs == Block::fall)
		return false;

	if (left.getFallOffset() != 0 || right.getFallOffset() != 0) {
		return false;
	}
	
	//printf("states: (%d, %d)\n", left.getState(), right.getState());
	Block temp = left;
	left = right;
	right = temp;
	//printf("states: (%d, %d)\n\n", left.getState(), right.getState());

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

void Grid::printDebug() {
	for (int i = blocks.size() - 1; i >= 0; --i) {
		for (int j = 0; j < ncols; ++j) {
			printf("%d ", blocks[i][j].getState());
		}
		printf("\n");
	}
	printf("\n");
}

void Grid::setBlockStates(list<Cell> & ev, Block::gameState gs) {
	for (list<Cell>::iterator it = ev.begin(); it != ev.end(); ++it)
		blocks[(*it).row][(*it).col].changeState(gs);
}

void Grid::updateEvents() {
	if (comboEvents.size() > 0) {
		for (list<Combo>::iterator ev = comboEvents.begin(); ev != comboEvents.end(); ) {
			if (!ev->update(*this)) 
				comboEvents.erase(ev++);
			else
				++ev;
		}

		if (comboEvents.size() == 0)
			changeState(Grid::play);
	}

	if (fallEvents.size() > 0) {
		for (list<Fall>::iterator fl = fallEvents.begin(); fl != fallEvents.end(); ) {
			if (!fl->update(*this)) 
				fallEvents.erase(fl++);
			else 
				++fl;
		}
	}

	if (bonuses.size() > 0) {
		for (list<Bonus>::iterator it = bonuses.begin(); it != bonuses.end(); ) {
			if (!it->update()) 
				bonuses.erase(it++);
			else
				++it;
		}
	}
}

/*	detectCombo
	This function uses the match<direction> functions to find all possible combos.
	A combo can be stored as a set of Cells, where a Cell is a Class that holds the
	row and column of a block. A combo in one direction (e.g. 3 blocks in a row 
	horizontal) is stored in 2 cells, and a combo in two directions (e.g. 5 blocks 
	vertical and 3 blocks horizontal) is stored in 4 cells.
	
	This function computes the cells by first finding a horizontal match. If a match
	is found, it will then iteratively search for a vertical from each block in the
	horizontal match.  */
Combo Grid::detectCombo(Cell &cell, int chains, bool doBonus) {
	int r = cell.row;
	int c = cell.col;

	Combo combo(chains);

	int match1 = leftMatch(r, c);
	int match2 = rightMatch(r, c);

	if (match1 + match2 >= 2) {
		combo.mid(r, c);
		combo.left(r, c - match1);
		combo.right(r, c + match2);

		for (int col = combo.left()->col; col <= combo.right()->col; ++col) {
			match1 = downMatch(r, col);
			match2 = upMatch(r, col);

			if (match1 + match2 >= 2) {
				combo.down(r - match1, col);
				combo.up(r + match2, col);

				combo.changeState(Combo::MULTI);
				initCombo(combo);
				if (doBonus) 
					initBonus(combo);
				return combo;
			}
		}

		combo.changeState(Combo::HORI);
		initCombo(combo);
		if (doBonus) 
			initBonus(combo);
		return combo;
	}
	else {
		match1 = downMatch(r, c);
		match2 = upMatch(r, c);

		if (match1 + match2 >= 2) {
			combo.mid(r, c);
			combo.down(r - match1, c);
			combo.up(r + match2, c);

			for (int row=combo.down()->row; row <= combo.up()->row; ++row) {
				match1 = leftMatch(row, c);
				match2 = rightMatch(row, c);

				if (match1 + match2 >= 2) {
					combo.left(row, c - match1);
					combo.right(row, c + match2);

					combo.changeState(Combo::MULTI);
					initCombo(combo);
					if (doBonus) 
						initBonus(combo);
					return combo;
				} 
			}
			combo.changeState(Combo::VERT);
			initCombo(combo);
			if (doBonus) 
				initBonus(combo);
			return combo;
		}
		else {
			combo.changeState(Combo::NONE);
			return combo;
		}
	}
}

void Grid::initCombo(Combo &combo) {
	if (combo.getState() == Combo::NONE)
		return;

	if (combo.getChainCount() >= 2) {
		Cell *cell = combo.up() ? combo.up() : combo.left();
		bonuses.push_back( Bonus(*cell, combo.getChainCount(), Bonus::CHAIN, *this) );
	}

	combo.init(*this);
	comboEvents.push_back(combo);
}

/*	detectFall(const Combo &)
	This fall detection subroutine passes the highest block for each row in the combo */
bool Grid::detectFall(const Combo &combo) {
	int r, c;

	Fall fall(combo.getChainCount());

	switch (combo.getState()) {
	
	case Combo::HORI:
		r = combo.left()->row + 1;

		if (r < (int)blocks.size())
			for (c = combo.left()->col; c <= combo.right()->col; ++c) 
				if (detectFall(r, c, false))
					fall.push_back(FallNode(r, c));

		break;

	case Combo::VERT:
		r = combo.up()->row + 1;
		c = combo.up()->col;
		if (r < (int)blocks.size())
			if (detectFall(r, c, false))
				fall.push_back(FallNode(r, c));
		
		break;

	case Combo::MULTI:
		r = combo.left()->row + 1;
		if (r < (int)blocks.size()) {
			for (c = combo.left()->col; c <= combo.right()->col; ++c) {
				if (c != combo.mid()->col && detectFall(r, c, false))
					fall.push_back(FallNode(r, c));
				else if (detectFall(combo.up()->row+1, c, false))
					fall.push_back(FallNode(combo.up()->row+1, c));
			}
		}
						
		break;

	default:
		return false;

	}

	fall.possibleChain = true;
	fall.init(*this);
	fallEvents.push_back(fall);
	
	return true;
}

bool Grid::detectFall(int r, int c, bool initialize) {
	if (r >= (int)blocks.size() || r <= 0)
		return false;

	Block::gameState midState, downState;
	downState = blocks[r-1][c].getState();
	midState = blocks[r][c].getState();

	if ((downState == Block::disabled || downState == Block::fall) && midState == Block::enabled) {
		//initFallState(cell);
		//fallEvents.push_back(cell);
		if (initialize) {
			Fall fall(FallNode(r, c));
			fall.init(*this);
			fallEvents.push_back(fall);
		}
		return true;
	}
	else return false;
}
