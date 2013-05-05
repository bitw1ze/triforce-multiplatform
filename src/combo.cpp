/* gridevent.cpp

Combo member variables:
	- coordinates of the up, down, left, and right, and mid Blocks.
	- type/state of combo (horizontal, vertical, or multiple).
	- time that the combo was activated.
	- interval that the combo lasts.
*/

#include "game.h"
#include <list>
#include <cstdio>

int Combo::comboInterval = 500;

Combo::Combo(int chains) {
	set(chains);
}

Combo::Combo(const Combo &src) {
	clone(src);
}

Combo & Combo::operator =(const Combo &src) {
	clone(src);

	return *this;
}

void Combo::set(int chains) {
	chainCount = chains + 1;

	_left = _right = _up = _down = _mid = NULL;

	startTime = 0;
	interval = 0;

	state = NONE;
}

void Combo::clone(const Combo &src) {
	chainCount = src.chainCount; 

	_left = src._left;
	_right = src._right;
	_up = src._up;
	_down = src._down;
	_mid = src._mid;

	interval = src.interval;
	startTime = src.startTime;

	state = src.state;
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

void Combo::init(Grid &grid) {
	interval = count() * Combo::comboInterval;
	startTime = mainTimer->time();
	
	list<Cell> cells = getList();

	grid.incComboInterval(interval);
	grid.changeState(Grid::combo);
	grid.setBlockStates(cells, Block::combo);
}

const list<Cell> Combo::getList() {
	unsigned int r, c;

	list<Cell> cells;

	switch (getState()) {
	case Combo::VERT:
		r = down()->row;
		c = down()->col;
		for (; r <= up()->row; ++r) {
			cells.push_back(Cell(r, c));
		}
		break;

	case Combo::HORI:
		r = left()->row;
		c = left()->col;
		for (; c <= right()->col; ++c) {
			cells.push_back(Cell(r, c));
		}
		break;

	case Combo::MULTI:
		r = left()->row;
		c = left()->col;
		for (; c <= right()->col; ++c) {
			cells.push_back(Cell(r, c));
		}
		
		c = down()->col;
		r = down()->row;
		for (; r <= up()->row; ++r) {
			cells.push_back(Cell(r, c));
		}

  case NONE:
  default:
    break;
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
  
  default:
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
		return (_up->row - _down->row) + (_right->col - _left->col) + 1;

	default:
		return 0;
	}
}

bool Combo::update(Grid &grid) {
	if (getState() != Combo::NONE && elapsed()) {
		list<Cell> cells = getList();
		grid.setBlockStates(cells, Block::disabled);
		grid.detectFall(*this);
		changeState(Combo::NONE);
		return false;
	}
	else return true;
}

Combo::operator bool() const {
	return getState() != Combo::NONE;
}
