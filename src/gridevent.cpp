#include "game.h"

int GridEvent::comboInterval = 500;
int GridEvent::fallInterval = 10;

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
	lastFall = src.lastFall;

	state = src.state;
	comboType = src.comboType;

	combo = src.combo;
}

bool GridEvent::operator ==(const GridEvent &ev) {
	return 
		left() == ev.left() && right() == ev.right() &&
		up() == ev.up() && down() == ev.down() &&
		mid() == ev.mid();
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

bool GridEvent::checkComboFinished(Grid *grid, GridEvent &ev) {
	if (ev.state == COMBO && mainTimer->elapsed(ev.startTime, ev.interval)) {
		ev.setBlockStates(Block::disabled);
		detectFallAfterCombo(grid, ev);
		return true;
	}

	return false;
}

void GridEvent::initComboState() {
	if (state != COMBO)
		return;

	interval = count() * GridEvent::comboInterval;
	startTime = mainTimer->time();

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

void GridEvent::composeFrame(Grid *grid) {
	if (grid == NULL)
		return;

	list<GridEvent> eventRemovals;
	list<Fall> fallRemovals;

	for (list<GridEvent>::iterator ev = grid->events.begin(); ev != grid->events.cend(); ++ev) {
		if ((*ev).checkComboFinished(grid, *ev)) {
			eventRemovals.push_back(*ev);
		}
	}

	if (eventRemovals.size() > 0) {
		for (list<GridEvent>::iterator ev = eventRemovals.begin(); ev != eventRemovals.cend(); ++ev) 
			grid->events.remove(*ev);

		eventRemovals.clear();

		if (grid->events.size() == 0)
			grid->changeState(Grid::play);
	}

	for (list<Fall>::iterator fl = grid->falls.begin(); fl != grid->falls.cend(); ++fl) {
		doFall(grid, *fl);
		if (!(*fl).enabled) {
			cleanupFall(grid, *fl);
			fallRemovals.push_back(*fl);
		}
	}

	if (fallRemovals.size() > 0) {
		for (list<Fall>::iterator fl = fallRemovals.begin(); fl != fallRemovals.cend(); ++fl) 
			grid->falls.remove(*fl);

		fallRemovals.clear();
	}


	/* detect combos after fall
	while (row < (int)grid->blocks.size() && grid->blocks[row][c].getState() == Block::enabled) {
		detectCombo(Cell(row, c));
		++row;
	}
	*/

}

void GridEvent::cleanupFall(Grid *grid, Fall &fall) {
	int r = fall.row;
	int c = fall.col;

	int row;
	// check if finished with fall 
	row = r;
		// re-enable all the blocks that were falling
	while (row < (int)grid->blocks.size() && grid->blocks[row][c].getState() == Block::fall) {
		grid->blocks[row][c].changeState(Block::enabled);
		detectCombo(grid, fall);
		++row;
	}

	fall.enabled = false;

}

void GridEvent::doFall(Grid *grid, Fall &cell) {
	// row and column begin at the block immediately above a broken combo block
	int r = cell.row;
	int c = cell.col;
	int row;

	if (cell.enabled == true && mainTimer->elapsed(cell.lastFall, fallInterval)) 
		cell.lastFall = mainTimer->time();
	else
		return;
	
	if (r <= 1 || grid->blocks[r-1][c].getState() == Block::enabled) {
		cleanupFall(grid, cell);
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

/*	detectCombo
	This function uses the match<direction> functions to find all possible combos.
	A combo can be stored as a set of Cells, where a Cell is a struct that holds the
	row and column of a block. A combo in one direction (e.g. 3 blocks in a row 
	horizontal) is stored in 2 cells, and a combo in two directions (e.g. 5 blocks 
	vertical and 3 blocks horizontal) is stored in 4 cells.
	
	This function computes the cells by first finding a horizontal match. If a match
	is found, it will then iteratively search for a vertical from each block in the
	horizontal match.  */
bool GridEvent::detectCombo(Grid *grid, Cell &cell) {
	int r = cell.row;
	int c = cell.col;

	GridEvent combo(grid);

	int match1 = grid->leftMatch(r, c);
	int match2 = grid->rightMatch(r, c);

	if (match1 + match2 >= 2) {
		combo.mid(r, c);
		combo.left(r, c - match1);
		combo.right(r, c + match2);

		for (int col = combo.left()->col; col <= combo.right()->col; ++col) {
			match1 = grid->downMatch(r, col);
			match2 = grid->upMatch(r, col);

			if (match1 + match2 >= 2) {
				combo.down(r - match1, col);
				combo.up(r + match2, col);

				combo.state = COMBO;
				combo.comboType = MULTI;
				combo.initComboState();
				grid->events.push_back(combo);
				return true;
			}
		}

		combo.state = COMBO;
		combo.comboType = HORI;
		combo.initComboState();
		grid->events.push_back(combo);
		return true;
	}
	else {
		match1 = grid->downMatch(r, c);
		match2 = grid->upMatch(r, c);

		if ( match1 + match2 >= 2) {
			combo.mid(r, c);
			combo.down(r - match1, c);
			combo.up(r + match2, c);

			for (int row=combo.down()->row; row <= combo.up()->row; ++row) {
				match1 = grid->leftMatch(row, c);
				match2 = grid->rightMatch(row, c);

				if (match1 + match2 >= 2) {
					combo.left(row, c - match1);
					combo.right(row, c + match2);

					combo.state = COMBO;
					combo.comboType = MULTI;
					combo.initComboState();
					grid->events.push_back(combo);
					return true;
				} 
			}

			combo.state = COMBO;
			combo.comboType = VERT;
			combo.initComboState();
			grid->events.push_back(combo);
			return true;
		}

		combo.state = NONE;
		return false;
	}
}

/*	detectFallAfterCombo
	This fall detection subroutine passes the highest block for each row in the combo */

bool GridEvent::detectFallAfterCombo(Grid *grid, GridEvent &ev) {
	if (ev.state != COMBO)
		return false;

	list<Fall> falls;

	int r, c;

	switch (ev.comboType) {
	
	case HORI:
		r = ev.left()->row + 1;
		if (r >= (int)grid->blocks.size())
			return false;

		for (c = ev.left()->col; c <= ev.right()->col; ++c) {
			detectFall(grid, Fall(r, c));
		}

		break;

	case VERT:
		r = ev.up()->row + 1;
		c = ev.up()->col;
		if (r >= (int)grid->blocks.size())
			return false;
		else {
			detectFall(grid, Fall(r, c));
		}
		
		break;

	case MULTI:
		r = ev.left()->row + 1;
		if (r >= (int)grid->blocks.size())
			return false;
		for (c = ev.left()->col; c <= ev.right()->col; ++c) {
			if (r != ev.mid()->row)
				detectFall(grid, Fall(r, c));
		}
		r = ev.up()->row + 1;
		c = ev.up()->col;
		detectFall(grid, Fall(r, c));

		break;

	default:
		ev.changeState(NONE);
		return false;
	}
	
	ev.changeState(NONE);

	return true;
}

bool GridEvent::detectFall(Grid *grid, Fall &cell) {
	int r = cell.row;
	int c = cell.col;

	if (r >= (int)grid->blocks.size())
		return false;

	Block::gameState downState;
	Block::gameState midState;
	Block::gameState upState; 

	downState = r > 1 ? grid->blocks[r-1][c].getState() : (Block::gameState)-1;
	midState = grid->blocks[r][c].getState();
	upState = r < (int)grid->blocks.size() - 1 ? grid->blocks[r+1][c].getState() : (Block::gameState)-1;

	if (( downState == Block::disabled || downState == Block::fall ) && midState == Block::enabled) {
		initFallState(grid, cell);
		grid->falls.push_back(cell);
		
		return true;
	}
	else if ( midState == Block::disabled && upState == Block::enabled ) {
		Fall fall(r+1, c);
		initFallState(grid, fall);
		grid->falls.push_back(fall);

		return true;
	}

	return false;
}

void GridEvent::initFallState(Grid *grid, Fall &cell) {
	int r = cell.row;
	int c = cell.col;

	cell.enabled = true;
	while (r < (int)grid->blocks.size() && grid->blocks[r][c].getState() == Block::enabled) {
		grid->blocks[r][c].changeState(Block::fall);
		++r;
	}

	cell.lastFall = mainTimer->time();
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
