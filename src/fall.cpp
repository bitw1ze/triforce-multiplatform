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

Fall::Fall() : list<FallNode>() {
	__super::list();
	set();
}

Fall::Fall(const Cell & cell) : list<FallNode>() {
	__super::list();
	set();

	push_back((const FallNode &)cell);
}

Fall::Fall(const list<FallNode> &lst) {
	__super::operator =(lst);
	set();
	possibleChain = true;
}

Fall & Fall::operator =(const Fall &src) {
	__super::operator =( (const list<FallNode> &)src );
	clone(src);
	return *this;
}

void Fall::set() {
	possibleChain = false;
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
