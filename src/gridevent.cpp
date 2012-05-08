#include "game.h"

int GridEvent::comboInterval = 500;
int GridEvent::fallInterval = 50;

GridEvent::GridEvent(Grid *g) {
	grid = g;
	_left = _right = _up = _down = _mid = NULL;
	interval = 0;
	state = NONE;
}

GridEvent::GridEvent(const GridEvent &src) {
	clone(src);
}

GridEvent & GridEvent::operator =(const GridEvent &src) {
	clone(src);

	return *this;
}

void GridEvent::clone(const GridEvent &src) {
	grid = src.grid;

	_left = src._left;
	_right = src._right;
	_up = src._up;
	_down = src._down;
	_mid = src._mid;

	interval = src.interval;
	startTime = src.startTime;
	timer.start();
	lastFall = src.lastFall;

	state = src.state;
	comboType = src.comboType;
	fallType = src.fallType;

	combo = src.combo;
	falls = src.falls;
}

Cell * GridEvent::left(int r, int c) { 
	if (!_left) 
		_left = new Cell; 
	_left->row = r; 
	_left->col = c; 

	return _left;
}

Cell * GridEvent::right(int r, int c) { 
	if (!_right) 
		_right = new Cell; 
	_right->row = r;
	_right->col = c;

	return _right;
}

Cell * GridEvent::down(int r, int c) { 
	if (!_down) 
		_down = new Cell; 
	_down->row = r;
	_down->col = c; 

	return _down;
}

Cell * GridEvent::up(int r, int c) { 
	if (!_up) 
		_up = new Cell; 
	_up->row = r; 
	_up->col = c; 

	return _up;
}

Cell * GridEvent::mid(int r, int c) { 
	if (!_mid) 
		_mid = new Cell; 
	_mid->row = r; 
	_mid->col = c; 

	return _mid;
}

int GridEvent::count() const {
	switch (comboType) {
	case HORI:
		return _right->col - _left->col + 1;

	case VERT:
		return _up->row - _down->row + 1;

	case MULTI:
		return (_up->row - _down->row) + (_right->col - _left->col);

	default:
		return 0;

	}
}

bool GridEvent::checkComboFinished() {
	if (state == COMBO && timer.elapsed(startTime, interval)) {
		setBlockStates(Block::disabled);
		detectFallAfterCombo();
		return true;
	}

	return false;
}

void GridEvent::initComboState() {
	if (state != COMBO)
		return;

	interval = count() * GridEvent::comboInterval;
	timer.start();
	startTime = timer.time();

	int r, c;

	switch (comboType) {
	case VERT:
		r = down()->row;
		c = down()->col;
		for (r; r <= up()->row; ++r) {
			combo.push_back(Cell(r, c));
		}
		break;

	case HORI:
		r = left()->row;
		c = left()->col;
		for (c; c <= right()->col; ++c) {
			combo.push_back(Cell(r, c));
		}
		break;

	case MULTI:
		r = left()->row;
		c = left()->col;
		for (c; c <= right()->col; ++c) {
			combo.push_back(Cell(r, c));
		}
		
		c = down()->col;
		r = down()->row;
		for (r; r <= up()->row; ++r) {
			if (r != mid()->row)
				combo.push_back(Cell(r, c));
		}
	}

	//printDebug();
	grid->incComboInterval(interval);
	grid->changeState(Grid::combo);
	setBlockStates(Block::combo);
}

void GridEvent::setBlockStates(Block::gameState gs) {
	for (list<Cell>::iterator it = combo.begin(); it != combo.cend(); ++it)
		grid->blocks[(*it).row][(*it).col].changeState(gs);
}

bool GridEvent::areCombos(list<GridEvent> &combos) {
	for (list<GridEvent>::iterator it = combos.begin(); it != combos.cend(); ++it)
		if (!(*it).checkComboFinished())
			return false;
	
	return true;
}

void GridEvent::checkFallResult(Grid *grid, list<GridEvent> &events) {
}

void GridEvent::composeFrame(Grid *g, list<GridEvent> &events) {
	Grid *grid = g;
	if (g == NULL)
		return;

	if (!GridEvent::areCombos(events)) {
		grid->changeState(Grid::play);

		if (!GridEvent::areFalling(events))
			events.clear();

		return;
	}

	/* detect combos after fall
	while (row < (int)grid->blocks.size() && grid->blocks[row][c].getState() == Block::enabled) {
		detectCombo(Cell(row, c));
		++row;
	}
	*/

	for (list<GridEvent>::iterator ev = events.begin(); ev != events.end(); ++ev) {
		switch ((*ev).getState()) {
		case COMBO:
			(*ev).checkComboFinished();

			break;
			
		case FALL:
			for (list<Cell>::iterator fl = (*ev).falls.begin(); fl != (*ev).falls.cend(); ++fl) {
				if ( (*fl).enabled )
					(*ev).doFall(*fl);
			}

			break;
		}
	}
}

void GridEvent::doFall(Cell &cell) {
	// row and column begin at the block immediately above a broken combo block
	int r = cell.row;
	int c = cell.col;
	int row;

	if (state == FALL && timer.elapsed(lastFall, fallInterval)) 
		lastFall = timer.time();
	else
		return;
	
	// check if finished with fall 
	if (r <= 1 || grid->blocks[r-1][c].getState() == Block::enabled) {
		row = r;
		// re-enable all the blocks that were falling
		while (row < (int)grid->blocks.size() && grid->blocks[row][c].getState() == Block::fall) {
			grid->blocks[row][c].changeState(Block::enabled);
			++row;
		}

		cell.enabled = false;

		return;
	}

	row = r;
	while (row < (int)grid->blocks.size() && grid->blocks[row][c].getState() == Block::fall) {
		grid->blocks[row][c].fallDown();
		++row;
	}

	bool swapBlocks = (grid->blocks[r][c].getFallOffset() == 0);
	if (swapBlocks) {
		row = r;
		while (row < (int)grid->blocks.size() && grid->blocks[row][c].getState() == Block::fall) {
			grid->blocks[row-1][c] = grid->blocks[row][c];		
			++row;
		}
		// disable the top row of the follow blocks that has just been cleared
		grid->blocks[row-1][c].changeState(Block::disabled);

		// decrement the current row that we begin from.
		r = --cell.row;
	}
}



bool GridEvent::hasFalling() {
	if (state != FALL)
		return false;

	for (list<Cell>::iterator fl = falls.begin(); fl != falls.cend(); ++fl) {
		if ( (*fl).enabled )
			return true;
	}

	changeState(DONE_FALLING);
	return false;
}

bool GridEvent::areFalling(list<GridEvent> &events) {
	for (list<GridEvent>::iterator ev = events.begin(); ev != events.cend(); ++ev) {
		if ( (*ev).hasFalling() )
			return true;
	}

	return false;
}

/*	detectCombo
	This function uses the match<direction> functions to find all possible combos.
	A combo can be stored as a set of Cells, where a Cell is a struct that holds the
	row and column of a block. A combo in one direction (e.g. 3 blocks in a row 
	horizontal) is stored in 2 cells, and a combo in two directions (e.g. 5 blocks 
	vertical and 3 blocks horizontal) is stored in 4 cells.
	
	This function computes the cells by first finding a horizontal match. If a match
	is found, it will then iteratively search for a vertical from each block in the
	horizontal match.  */
bool GridEvent::detectCombo(Cell &cell) {
	int r = cell.row;
	int c = cell.col;

	int match1 = grid->leftMatch(r, c);
	int match2 = grid->rightMatch(r, c);

	if (match1 + match2 >= 2) {
		mid(r, c);
		left(r, c - match1);
		right(r, c + match2);

		for (int col = left()->col; col <= right()->col; ++col) {
			match1 = grid->downMatch(r, col);
			match2 = grid->upMatch(r, col);

			if (match1 + match2 >= 2) {
				down(r - match1, col);
				up(r + match2, col);

				state = COMBO;
				comboType = MULTI;
				initComboState();
				return true;
			}
		}

		state = COMBO;
		comboType = HORI;
		initComboState();
		return true;
	}
	else {
		match1 = grid->downMatch(r, c);
		match2 = grid->upMatch(r, c);

		if ( match1 + match2 >= 2) {
			mid(r, c);
			down(r - match1, c);
			up(r + match2, c);

			for (int row=down()->row; row <= up()->row; ++row) {
				match1 = grid->leftMatch(row, c);
				match2 = grid->rightMatch(row, c);

				if (match1 + match2 >= 2) {
					left(row, c - match1);
					right(row, c + match2);

					state = COMBO;
					comboType = MULTI;
					initComboState();
					return true;
				} 
			}

			state = COMBO;
			comboType = VERT;
			initComboState();
			return true;
		}

		state = NONE;
		return false;
	}
}

/*	detectFallAfterCombo
	This fall detection subroutine passes the highest block for each row in the combo */

bool GridEvent::detectFallAfterCombo() {
	if (state != COMBO)
		return false;

	int r, c;

	switch (comboType) {
	
	case HORI:
		r = left()->row + 1;
		if (r >= (int)grid->blocks.size())
			return false;

		for (c = left()->col; c <= right()->col; ++c) {
			if (grid->blocks[r][c].getState() == Block::enabled)
				falls.push_back(Cell(r, c));
		}

		break;

	case VERT:
		r = up()->row + 1;
		c = up()->col;
		if (r >= (int)grid->blocks.size())
			return false;
		else {
			if (grid->blocks[r][c].getState() == Block::enabled)
				falls.push_back(Cell(r, c));
		}
		
		break;

	case MULTI:
		r = left()->row + 1;
		if (r >= (int)grid->blocks.size())
			return false;
		for (c = left()->col; c <= right()->col; ++c) {
			if (grid->blocks[r][c].getState() == Block::enabled && r != mid()->row)
				falls.push_back(Cell(r, c));
		}
		r = up()->row + 1;
		c = up()->col;
		if (r < (int)grid->blocks.size()) {
			if (grid->blocks[r][c].getState() == Block::enabled)
				falls.push_back(Cell(r, c));
		}

		break;

	default:
		state = NONE;
		return false;
	}
	
	bool isFall = falls.size() > 0;
	if (isFall) {
		changeState(FALL);
		fallType = AFTERCOMBO;
		initFallState();
	}

	return isFall;
}

bool GridEvent::detectFallAfterSwap(const Cell &cell) {
	int r = cell.row;
	int c = cell.col;

	if (r >= (int)grid->blocks.size())
		return false;

	if (grid->blocks[r-1][c].getState() == Block::disabled || grid->blocks[r-1][c].getState() == Block::fall) 
		falls.push_back(cell);

	bool isFall = falls.size() > 0;
	if (falls.size() > 0) {
		changeState(FALL);
		fallType = AFTERSWAP;
		initFallState();
	}

	return isFall;
}

void GridEvent::initFallState() {
	for (list<Cell>::iterator fl = falls.begin(); fl != falls.cend(); ++fl) {
		int r = (*fl).row;
		int c = (*fl).col;

		while (r < (int)grid->blocks.size() && grid->blocks[r][c].getState() == Block::enabled) {
			grid->blocks[r][c].changeState(Block::fall);
			++r;
		}
	}

	lastFall = timer.time();
}

void GridEvent::printDebug() {
	if (state != COMBO)
		return;

	switch (comboType) {
	case VERT:
		printf("count = %d: (%d, %d)->(%d, %d)\n", count(), down()->row, down()->col, up()->row, up()->col);
		break;

	case HORI:
		printf("count = %d: (%d, %d)->(%d, %d)\n", count(), left()->row, left()->col, right()->row, right()->col);
		break;

	case MULTI:
		printf("count = %d: (%d, %d)->(%d, %d) (%d, %d)->(%d, %d)\n", count(),
			left()->row, left()->col, right()->row, right()->col,
			down()->row, down()->col, up()->row, up()->col);
		break;
	}
}

void GridEvent::printStates() {
	for (list<Cell>::iterator it = combo.begin(); it != combo.cend(); ++it) {
		if (grid->blocks[(*it).row][(*it).col].getState() == Block::combo)
			cout << "state == combo\n";
		else
			cout << "state == not combo\n";
	}
}
