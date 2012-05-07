#include "game.h"

int GridEvent::comboInterval = 500;

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

	interval = src.interval;
	startTime = src.startTime;

	state = src.state;
	comboType = src.comboType;
	fallType = src.fallType;

	timer.start();

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

bool GridEvent::isFinished() {
	return timer.elapsed(startTime, interval);
}

bool GridEvent::initComboState() {
	if (state != COMBO)
		return false;

	interval = count() * GridEvent::comboInterval;
	timer.start();
	startTime = timer.time();

	int r, c;

	switch (comboType) {
	case VERT:
		c = down()->col;
		for (r = down()->row; r <= up()->row; ++r) {
			combo.push_back(Cell(r, c));
		}
		break;

	case HORI:
		r = left()->row;
		for (c = left()->col; c <= right()->col; ++c) {
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
		for (r = down()->row; r <= up()->row; ++r) {
			if (r != mid()->row)
				combo.push_back(Cell(r, c));
		}
	}

	//printDebug();
	grid->incComboInterval(interval);
	grid->changeState(Grid::combo);
	setBlockStates(Block::combo);

	return true;
}

void GridEvent::setBlockStates(Block::gameState gs) {
	for (list<Cell>::iterator it = combo.begin(); it != combo.cend(); ++it)
		grid->blocks[(*it).row][(*it).col].changeState(gs);
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

bool GridEvent::areFinished(list<GridEvent> &combos) {
	for (list<GridEvent>::iterator it = combos.begin(); it != combos.cend(); ++it)
		if (!(*it).isFinished())
			return false;
	
	return true;
}

bool GridEvent::finish(list<GridEvent> &combos) {
	if (areFinished(combos)) {
		for (list<GridEvent>::iterator c = combos.begin(); c != combos.end(); ++c)
			(*c).setBlockStates(Block::disabled);
		return true;
	}
	
	return false;
}

bool GridEvent::detectComboFall() {
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
		return falls.size() > 0;

	case VERT:
		r = up()->row + 1;
		c = up()->col;
		if (r >= (int)grid->blocks.size())
			return false;
		else {
			if (grid->blocks[r][c].getState() == Block::enabled)
				falls.push_back(Cell(r, c));
		}
		return falls.size() > 0;

	case MULTI:
		r = left()->row + 1;
		if (r >= (int)grid->blocks.size())
			return false;
		for (c = left()->col; c <= right()->col; ++c) {
			if (grid->blocks[r+1][c].getState() == Block::enabled)
				falls.push_back(Cell(r, c));
		}
		r = up()->row + 1;
		int c = up()->col;
		if (r < (int)grid->blocks.size() - 1) {
			if (grid->blocks[r][c].getState() == Block::enabled)
				falls.push_back(Cell(r, c));
		}
		return falls.size() > 0;
	}

	return false;
}

bool GridEvent::detectSwapFall(Cell & cell) {
	int r = cell.row;
	int c = cell.col;

	if (grid->blocks[r-1][c].getState() == Block::disabled || grid->blocks[r-1][c].getState() == Block::fall) 
		falls.push_back(Cell(r, c));

	return falls.size() > 0;
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
				return true;
			}
		}

		state = COMBO;
		comboType = HORI;
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
					return true;
				} 
			}

			state = COMBO;
			comboType = VERT;
			return true;
		}

		state = NONE;
		return false;
	}
}
