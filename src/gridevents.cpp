#include "game.h"

int Fall::fallInterval = 10;
int Combo::comboInterval = 500;

Fall::Fall() : Cell() {
	init(); 
}

Fall::Fall(int r, int c) : Cell(r, c) { 
	init(); 
}

Fall::Fall(const Fall &src) : Cell(src) { 
	clone(src); 
}

Fall & Fall::operator =(const Fall &src) { 
	__super::operator =(src); 
	clone(src); 
	return *this;
}

bool Fall::operator ==(const Fall &fl) { 
	return __super::operator ==(fl); 
}

void Fall::init() { 
	numFalls = 0; 
	lastFall = -1; 
	enabled = false; 
}

void Fall::clone(const Fall &src) { 
	numFalls = src.numFalls; 
	lastFall = src.lastFall; 
	enabled = src.enabled; 
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

const list<Cell> & Combo::getList() {
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

