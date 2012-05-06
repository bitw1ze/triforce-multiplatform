#include "game.h"

Combo::Combo(Grid *g) {
	grid = g;
	blocks = grid->blocks;
	_left = _right = _up = _down = _mid = NULL;
	interval = 0;
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

bool Combo::isVert() const {
	return (_up && _down) && !(_left && _right);
}

bool Combo::isHori() const {
	return (_left && _right) && !(_up && _down);
}

bool Combo::isMulti() const {
	return (_up && _down) && (_left && _right);
}

bool Combo::isVertCombo() const {
	return isVert() && count() >= 3;
}

bool Combo::isHoriCombo() const {
	return isHori() && count() >= 3;
}

bool Combo::isMultiCombo() const {
	return isMulti() && count() >= 3;
}

bool Combo::isCombo() const {
	return (isVertCombo() || isHoriCombo() || isMultiCombo());
}

int Combo::count() const {
	if (isVert() && !isHori())
		return _up->row - _down->row;
	else if (!isVert() && !isHori())
		return _right->col - _left->col;
	else if (isMulti())
		return (_up->row - _down->row) + (_right->col - _left->col) - 1;
	else
		return 0;
}

bool Combo::isFinished() {
	return timer.elapsed(startTime, interval);
}

bool Combo::initComboState() {
	if (!isCombo())
		return false;

	interval = count() * Block::interval_combo;
	timer.start();
	startTime = timer.time();

	int interval = count() * Block::interval_combo;

	if (isVert()) {
		int c = down()->col;
		for (int r = down()->row; r <= up()->row; ++r) {
			combo.push_back(blocks[r][c]);
		}
	}
	else if (isHori()) {
		int r = left()->row;
		for (int c = left()->col; c <= right()->col; ++c) {
			combo.push_back(blocks[r][c]);
		}
	}
	else if (isMulti()) {
		int r = left()->row;
		int c = left()->col;
		for (c; c <= right()->col; ++c) {
			combo.push_back(blocks[r][c]);
		}
		
		c = down()->col;
		for (r = down()->row; r <= up()->row; ++r) {
			if (r != mid()->row)
				combo.push_back(blocks[r][c]);
		}
	}

	grid->incComboTimer(interval);
	grid->changeState(Grid::combo);
	setBlockStates(Block::combo);

	return true;
}

void Combo::setBlockStates(Block::gameState gs) {
	for (list<Block>::iterator it = combo.begin(); it != combo.cend(); ++it)
		(*it).changeState(gs);
}