/*
Fall 
	- contains a list of FallNodes and maintains them.

FallNode member variables:
	- starting coordinate of the falling block
	- number of blocks that need to fall
	- last time that a fall iteration occured
	- current status (enabled/disabled)
*/

#include "game.h"
#include <list>

int Fall::fallInterval = 10;

Fall::Fall(int chains) : list<FallNode>() {
	__super::list();
	set(chains);
}

Fall::Fall(const Cell & cell, int chains) : list<FallNode>() {
	__super::list();
	set(chains);

	push_back((const FallNode &)cell);
}

Fall::Fall(const list<FallNode> &lst, int chains) {
	__super::operator =(lst);
	set(chains);
	possibleChain = true;
}

Fall & Fall::operator =(const Fall &src) {
	__super::operator =( (const list<FallNode> &)src );
	clone(src);
	return *this;
}

void Fall::set(int chains) {
	possibleChain = false;
	chainCount = chains;
}

void Fall::clone(const Fall &src) {
	possibleChain = src.possibleChain;
	chainCount = src.chainCount;
}

void Fall::init(Grid &grid) {
	for (iterator it = begin(); it != end(); ++it)
		(*it).init(grid, chainCount);
}

bool Fall::update(Grid &grid) {
	for (iterator it = begin(); it != end(); ) {
		if (it->update(grid)) 
			++it;
		else {
			it->cleanup(grid);
			chainCount += it->getChainCount();
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
	chainCount = 0;
}

void FallNode::clone(const FallNode &src) { 
	numFalls = src.numFalls; 
	lastFall = src.lastFall; 
	enabled = src.enabled; 
	chainCount = src.chainCount;
}

void FallNode::init(Grid &grid, int chains) {
	int r = row;
	int c = col;
	enabled = true;
	chainCount = chains;

	while (r < (int)grid.blocks.size() && grid.blocks[r][c].getState() == Block::enabled) {
		grid.blocks[r][c].changeState(Block::fall);
		++r;
	}

	numFalls = r - row;
	lastFall = mainTimer->time();
}

void FallNode::cleanup(Grid &grid) {
	int r = row;
	int c = col;

	int i;

	// re-enable all the blocks that were falling
	for (i = r; i < r + numFalls; ++i) 
		grid.blocks[i][c].changeState(Block::enabled);

	for (i = r-1; i < r + numFalls; ++i) {
		if (grid.detectCombo(Cell(i, c), chainCount)) {
			++chainCount;
			cout << "chaincount: " << chainCount << endl;
		}
	}

	enabled = false;
}

// return true while the block is still falling
bool FallNode::update(Grid &grid) {
	// row and column begin at the block immediately above a broken combo block
	int & r = row;
	int c = col;
	int i;

	if (r >= nrows)
		return true;

	if (enabled == true && mainTimer->elapsed(lastFall, Fall::fallInterval)) 
		lastFall = mainTimer->time();
	else
		return true;

	// loop through all the rows and make them fall one iteration at a time.
	for (i = r; i < r + numFalls; ++i) {
		grid.blocks[i][c].fallDown();
	}

	bool swapDown = (grid.blocks[r][c].getFallOffset() <= 0);

	if (swapDown) {
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
