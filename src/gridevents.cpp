/* gridevent.cpp

This file contains the FallNode class and Combo class. Each class contains
information relevant to their respective events. They only have methods
and variables that are useful to the GridController class. They do not
actually control the state of Blocks because that is GridController's job.

Combo member variables:
	- coordinates of the up, down, left, and right, and mid Blocks.
	- type/state of combo (horizontal, vertical, or multiple).
	- time that the combo was activated.
	- interval that the combo lasts.

FallNode member variables:
	- starting coordinate of the falling block
	- number of blocks that need to fall
	- last time that a fall iteration occured
	- current status (enabled/disabled)
*/

#include "game.h"
#include <list>

int FallNode::fallInterval = 10;
int Combo::comboInterval = 500;

Fall::Fall() : list<FallNode>() {
	__super::list();
	possibleChain = false;

	set();
}

Fall::Fall(const Cell & cell) {
	__super::list();
	set();

	push_back((const FallNode &)cell);
	possibleChain = false;
}

Fall::Fall(const list<FallNode> &src) {
	__super::operator =(src);
	possibleChain = true;
}

Fall & Fall::operator =(const Fall &src) {
	__super::operator =( (const list<FallNode> &)src );
	clone(src);
	return *this;
}

void Fall::set() {
}

void Fall::clone(const Fall &src) {
	possibleChain = src.possibleChain;
}

void Fall::init(Grid &grid) {
	for (iterator it = begin(); it != end(); ++it)
		(*it).init(grid);
}

bool Fall::update(Grid &grid) {
	for (iterator it = begin(); it != end(); ) {
		if ((*it).update(grid)) 
			++it;
		else {
			(*it).cleanup(grid);
			erase(it++);
		}
	}

	return size() > 0;
}

void Fall::cleanup(Grid &grid) {
}

void Fall::adjustRow() {
	for (iterator it = begin(); it != end(); ++it)
		(*it).row += 1;
}

FallNode::FallNode() : Cell() {
	set(); 
}

FallNode::FallNode(int r, int c) : Cell(r, c) { 
	set(); 
}

FallNode::FallNode(const FallNode &src) : Cell(src) { 
	clone(src); 
}

FallNode & FallNode::operator =(const FallNode &src) { 
	__super::operator =(src); 
	clone(src); 
	return *this;
}

void FallNode::set() { 
	numFalls = 0; 
	lastFall = 0; 
	enabled = false; 
}

void FallNode::clone(const FallNode &src) { 
	numFalls = src.numFalls; 
	lastFall = src.lastFall; 
	enabled = src.enabled; 
}

void FallNode::init(Grid &grid) {
	int r = row;
	int c = col;
	enabled = true;

	while (r < (int)grid.blocks.size() && grid.blocks[r][c].getState() == Block::enabled) {
		grid.blocks[r][c].changeState(Block::fall);
		++r;
	}

	numFalls = r - row;
	lastFall = mainTimer->time();
}

int FallNode::cleanup(Grid &grid) {
	int r = row;
	int c = col;

	int i;

	// re-enable all the blocks that were falling
	for (i = r; i < r + numFalls; ++i) {
		grid.blocks[i][c].changeState(Block::enabled);
	}

	int tempChains = 0;

	for (i = r-1; i < r + numFalls; ++i) {
		if (grid.detectCombo(Cell(i, c))) 
			++tempChains;
	}

	//cout << "Falltype: " << fall.fallType << endl;
	//cout << "chains: " << tempChains << endl;

	return tempChains;
	enabled = false;
}

// return true while the block is still falling
bool FallNode::update(Grid &grid) {
	// row and column begin at the block immediately above a broken combo block
	int & r = row;
	int c = col;
	int i;

	if (enabled == true && mainTimer->elapsed(lastFall, FallNode::fallInterval)) 
		lastFall = mainTimer->time();
	else
		return true;

	// loop through all the rows and make them fall one iteration at a time.
	for (i = r; i < r + numFalls; ++i) {
		grid.blocks[i][c].fallDown();
	}

	bool swapDown = (grid.blocks[r][c].getFallOffset() <= 0);

	if (swapDown) {
	//	printf("r=%d\n", r);
	//	printDebug();
		for (i = r; i < r + numFalls; ++i) {
			grid.blocks[i-1][c] = grid.blocks[i][c];
		}
		grid.blocks[i-1][c].changeState(Block::disabled);

		--r;

		// Clean up and exit if we are finished falling.
		bool isFalling		=  grid.blocks[r-1][c].getState() == Block::disabled
							|| grid.blocks[r-1][c].getState() == Block::fall;
		return isFalling;
	}
	else return true;
}

Combo::Combo() {
	_left = _right = _up = _down = _mid = NULL;
	interval = 0;
	state = NONE;
}

Combo::Combo(const Combo &src) {
	clone(src);
}

Combo & Combo::operator =(const Combo &src) {
	clone(src);

	return *this;
}

void Combo::clone(const Combo &src) {
	_left = src._left;
	_right = src._right;
	_up = src._up;
	_down = src._down;
	_mid = src._mid;

	interval = src.interval;
	startTime = src.startTime;

	state = src.state;

	combo = src.combo;
}

bool Combo::operator ==(const Combo &ev) {
	return 
		left() == ev.left() && right() == ev.right() &&
		up() == ev.up() && down() == ev.down() &&
		mid() == ev.mid();
}

void Combo::activate() {
	startTime = mainTimer->time();
}

bool Combo::elapsed() const {
	return mainTimer->elapsed(startTime, interval);
}

void Combo::init() {
	interval = count() * Combo::comboInterval;
	startTime = mainTimer->time();
}

const list<Cell> Combo::getList() {
	int r, c;

	list<Cell> cells;

	switch (getState()) {
	case Combo::VERT:
		r = down()->row;
		c = down()->col;
		for (r; r <= up()->row; ++r) {
			cells.push_back(Cell(r, c));
		}
		break;

	case Combo::HORI:
		r = left()->row;
		c = left()->col;
		for (c; c <= right()->col; ++c) {
			cells.push_back(Cell(r, c));
		}
		break;

	case Combo::MULTI:
		r = left()->row;
		c = left()->col;
		for (c; c <= right()->col; ++c) {
			cells.push_back(Cell(r, c));
		}
		
		c = down()->col;
		r = down()->row;
		for (r; r <= up()->row; ++r) {
			if (r != mid()->row)
				cells.push_back(Cell(r, c));
		}
	}

	return cells;
}

void Combo::printDebug() {
	switch (state) {
	case Combo::VERT:
		printf("count = %d: (%d, %d)->(%d, %d)\n", count(), down()->row, down()->col, up()->row, up()->col);
		break;

	case Combo::HORI:
		printf("count = %d: (%d, %d)->(%d, %d)\n", count(), left()->row, left()->col, right()->row, right()->col);
		break;

	case Combo::MULTI:
		printf("count = %d: (%d, %d)->(%d, %d) (%d, %d)->(%d, %d)\n", count(),
			left()->row, left()->col, right()->row, right()->col,
			down()->row, down()->col, up()->row, up()->col);
		break;
	}
}

Cell * Combo::left(int r, int c) { 
	if (!_left) 
		_left = new Cell; 
	_left->row = r; 
	_left->col = c; 

	return _left;
}

Cell * Combo::right(int r, int c) { 
	if (!_right) 
		_right = new Cell; 
	_right->row = r;
	_right->col = c;

	return _right;
}

Cell * Combo::down(int r, int c) { 
	if (!_down) 
		_down = new Cell; 
	_down->row = r;
	_down->col = c; 

	return _down;
}

Cell * Combo::up(int r, int c) { 
	if (!_up) 
		_up = new Cell; 
	_up->row = r; 
	_up->col = c; 

	return _up;
}

Cell * Combo::mid(int r, int c) { 
	if (!_mid) 
		_mid = new Cell; 
	_mid->row = r; 
	_mid->col = c; 

	return _mid;
}

int Combo::count() const {
	switch (state) {
	case Combo::HORI:
		return _right->col - _left->col + 1;

	case Combo::VERT:
		return _up->row - _down->row + 1;

	case Combo::MULTI:
		return (_up->row - _down->row) + (_right->col - _left->col);

	default:
		return 0;
	}
}

