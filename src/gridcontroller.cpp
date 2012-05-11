/* gridcontroller.cpp

GridController controls the events that occur in a Grid instance. It keeps
track of Fall and Combo events. Grid calls GridController's composeFrame().
Every call to composeFrame() updates the state of events. GridController
detects Falls and Combos, initializes them, and cleans up after them when
they are done. It also calls the doFall() method to make Blocks fall for
every iteration that the Fall is active.
*/

#include "game.h"

GridController::GridController(Grid *grid) {
	set(grid);
}

GridController::GridController(const GridController &gc) {
	clone(gc);
}

GridController & GridController::operator =(const GridController &gc) {
	clone(gc);
	return *this;
}

void GridController::set(Grid *g) {
	grid = g;
	chainCount = 0;
}

void GridController::clone(const GridController &g) {
	grid = g.grid;
}

void GridController::initComboState(Combo &combo) {
	list<Cell> cells = combo.getList();

	combo.init();
	grid->changeState(Grid::combo);
	comboEvents.push_back(combo);
	setBlockStates(cells, Block::combo);
}


bool GridController::checkComboFinished(Combo &ev) {
	if (ev.getState() != Combo::NONE && ev.elapsed()) {
		list<Cell> cells = ev.getList();
		setBlockStates(cells, Block::disabled);
		detectFallAfterCombo(ev);
		return true;
	}

	return false;
}

void GridController::setBlockStates(list<Cell> & ev, Block::gameState gs) {
	for (list<Cell>::iterator it = ev.begin(); it != ev.end(); ++it)
		grid->blocks[(*it).row][(*it).col].changeState(gs);
}

void GridController::composeFrame() {
	if (grid == NULL)
		return;

	list<Combo> comboRemovals;
	list<Fall> fallRemovals;

	for (list<Combo>::iterator ev = comboEvents.begin(); ev != comboEvents.cend(); ++ev) {
		if (checkComboFinished(*ev)) {
			comboRemovals.push_back(*ev);
		}
	}

	if (comboRemovals.size() > 0) {
		for (list<Combo>::iterator ev = comboRemovals.begin(); ev != comboRemovals.cend(); ++ev) 
			comboEvents.remove(*ev);

		comboRemovals.clear();

		if (comboEvents.size() == 0)
			grid->changeState(Grid::play);
	}

	for (list<Fall>::iterator fl = fallEvents.begin(); fl != fallEvents.cend(); ++fl) {
		doFall(*fl);
		if (!(*fl).enabled) {
			fallRemovals.push_back(*fl);
		}
	}

	if (fallRemovals.size() > 0) {
		for (list<Fall>::iterator fl = fallRemovals.begin(); fl != fallRemovals.cend(); ++fl) 
			fallEvents.remove(*fl);

		fallRemovals.clear();
	}
}

void GridController::cleanupFall(Fall &fall) {
	int r = fall.row;
	int c = fall.col;

	int row;

	// re-enable all the blocks that were falling
	for (row = r; row < r + fall.numFalls; ++row) {
		grid->blocks[row][c].changeState(Block::enabled);
	}

	int isChain = false;

	for (row = r-1; row < r + fall.numFalls; ++row) {
		if (detectCombo(Cell(row, c))) {
			isChain = true;
			++chainCount;
		}
	}

	if (!isChain)
		chainCount = 0;

	fall.enabled = false;
}

void GridController::doFall(Fall &cell) {
	// row and column begin at the block immediately above a broken combo block
	int & r = cell.row;
	int c = cell.col;
	int row;

	if (cell.enabled == true && mainTimer->elapsed(cell.lastFall, Fall::fallInterval)) 
		cell.lastFall = mainTimer->time();
	else
		return;

	// loop through all the rows and make them fall one iteration at a time.
	for (row = r; row < r + cell.numFalls; ++row) {
		grid->blocks[row][c].fallDown();
	}

	bool swapDown = (grid->blocks[r][c].getFallOffset() <= 0);

	if (swapDown) {
	//	printf("r=%d\n", r);
	//	grid->printDebug();
		for (row = r; row < r + cell.numFalls; ++row) {
			grid->blocks[row-1][c] = grid->blocks[row][c];
		}
		grid->blocks[row-1][c].changeState(Block::disabled);

		--r;

		// Clean up and exit if we are finished falling.
		bool fallFinished = grid->blocks[r-1][c].getState() != Block::disabled
			&& grid->blocks[r-1][c].getState() != Block::fall;
		if (fallFinished) {
	//		grid->printDebug();
			cleanupFall(cell);
	//		grid->printDebug();
			return;
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
bool GridController::detectCombo(Cell &cell) {
	int r = cell.row;
	int c = cell.col;

	Combo combo;

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

				combo.changeState(Combo::MULTI);
				initComboState(combo);
				return true;
			}
		}

		combo.changeState(Combo::HORI);
		initComboState(combo);
		return true;
	}
	else {
		match1 = grid->downMatch(r, c);
		match2 = grid->upMatch(r, c);

		if (match1 + match2 >= 2) {
			combo.mid(r, c);
			combo.down(r - match1, c);
			combo.up(r + match2, c);

			for (int row=combo.down()->row; row <= combo.up()->row; ++row) {
				match1 = grid->leftMatch(row, c);
				match2 = grid->rightMatch(row, c);

				if (match1 + match2 >= 2) {
					combo.left(row, c - match1);
					combo.right(row, c + match2);

					combo.changeState(Combo::MULTI);
					initComboState(combo);
					return true;
				} 
			}

			combo.changeState(Combo::VERT);
			initComboState(combo);
			return true;
		}
		else {
			combo.changeState(Combo::NONE);
			return false;
		}
	}
}

/*	detectFallAfterCombo
	This fall detection subroutine passes the highest block for each row in the combo */

bool GridController::detectFallAfterCombo(Combo &ev) {
	if (ev.getState() == Combo::NONE)
		return false;

	list<Fall> falls;

	int r, c;

	switch (ev.getState()) {
	
	case Combo::HORI:
		r = ev.left()->row + 1;

		if (r < (int)grid->blocks.size())
			for (c = ev.left()->col; c <= ev.right()->col; ++c) 
				detectFall(Fall(r, c));

		break;

	case Combo::VERT:
		r = ev.up()->row + 1;
		c = ev.up()->col;
		if (r < (int)grid->blocks.size())
			detectFall(Fall(r, c));
		
		break;

	case Combo::MULTI:
		r = ev.left()->row + 1;
		if (r < (int)grid->blocks.size()) {
			
			for (c = ev.left()->col; c <= ev.right()->col; ++c)
				if (r != ev.mid()->row)
					detectFall(Fall(r, c));
		
			r = ev.up()->row + 1;
			c = ev.up()->col;
			detectFall(Fall(r, c));
		}

		break;

	default:
		return false;

	}
	
	ev.changeState(Combo::NONE);
	return true;
}

bool GridController::detectFall(Fall &cell) {
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
		initFallState(cell);
		fallEvents.push_back(cell);
		
		return true;
	}
	else if ( midState == Block::disabled && upState == Block::enabled ) {
		Fall fall(r+1, c);
		initFallState(fall);
		fallEvents.push_back(fall);

		return true;
	}

	return false;
}

void GridController::initFallState(Fall &cell) {
	int r = cell.row;
	int c = cell.col;

	cell.enabled = true;

	while (r < (int)grid->blocks.size() && grid->blocks[r][c].getState() == Block::enabled) {
		grid->blocks[r][c].changeState(Block::fall);
		++r;
	}

	cell.numFalls = r - cell.row;
	cell.lastFall = mainTimer->time();
}