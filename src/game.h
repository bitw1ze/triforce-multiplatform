/*	game.h
	by Gabe Pike
	Changes:
		- added prototypes for methods used by other classes
		- created a Point struct and a Cell struct for better x/y and row/col handling
*/

#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <deque>
#include <list>
#include "2DGraphics.h"
#include "globals.h"

using namespace std;
using namespace Globals;

class GamePlay;
class Grid;
class Block;
class Cursor;
extern CTimer *mainTimer;

/* GamePlay is the main interface for controlling everything else within the 
   game. Classes other than GamePlay declared in this header will be
   implemented in their own .cpp files to make things more modular. This
   seems to be the best solution. */

struct Point {
	int x, y;
};

struct Cell {
	int row, col;
};

namespace PlayState {
	enum Actions {
		UP, DOWN, LEFT, RIGHT,
		SWAP, PUSH,
		_NUMBER_OF_ACTIONS
	};
    static const string actionLabels[_NUMBER_OF_ACTIONS] = {
		"Up", "Down", "Left", "Right"
		"Swap", "Push",
	};
}

class GamePlay {
protected:
	/* bitmap files */
	static const string 
		blockFiles[],
		blockComboFiles[],
		cursorFiles[],
		gridBorderFile,
		bgFile;
	static const int numCursorFiles;

	/* frame stuff */
	int current_frame, 
		last_time;

	BMPClass background;

public:

	enum gameState {play, pause, quit, combo, fall};
	CBaseSprite *blockSprites[nblocktypes],
	            *cursorSprite,
				*gridBorderSprite;
	Grid *grid;

	GamePlay();
	void display();
	void composeFrame();
	void init();
	void processFrame();
	void loadImages();
	int getWidth() { return background.getViewportWidth();} 
	int getHeight() { return background.getViewportHeight();} 

	void changeState(gameState gs);
	gameState getState();
};

/* Grid class holds abstracts all the operations on grid of blocks for a 
   single player */

class Grid {

public:
	enum gameState { play, combo };
protected:
	int block_w, block_h,
		grid_yspeed, grid_yoff,
		last_push, last_combo,
		timer_push, timer_combo,
		last_fall, timer_fall,
		last_cursor_anim, timer_cursor_anim, current_cursor_frame;
	Point gridPos;
	GamePlay *gamePlay;
	deque<Block **> blocks;
	CBaseSprite** blockSprites;
	gameState state;
	void composeFrame();

public:
	static void declareActions();
	void defineActions();
	static void doAction(void *gridInstance, int actionState, int actionType);

	Grid(GamePlay *ge);
	virtual ~Grid();
	void pushRow();
	void addRow();
	void loadImages();
	void display();
	void setCoords();
	void swapBlocks();
	Block * getBlock(int row, int col);

	bool containsPoint(int x, int y);
	void onCombo();
	void onPlay();
	void changeState(gameState gs);
	gameState getState() const { return state; }

	/* set/get properties */
	int getYOffset() { return grid_yoff; }
	int getX() { return gridPos.x; }
	int getY() { return gridPos.y; }
	int getBlockWidth() { return block_w; }
	int getBlockHeight() { return block_h; }
	int countEnabledRows() const;

	Cursor *cursor;
};

/* The Block class abstracts operations on a single block, such as getting and 
   setting the x and y values and setting states */

class Block : public CObject {
public: enum gameState { enabled, disabled, combo, fall, inactive };
protected:
	gameState state, nextState;
	void onCombo();
	CTimer *timer;
	int last_combo;
	int last_fall, interval_fall, total_falls, count_falls, fall_factor;
	int total_combo_interval;
	Grid *grid;

	//static int interval_combo;
public:
	Block(Grid *g);
	Block *left, *right, *up, *down;
	bool swap(Block &right);
	void changeState(gameState gs);
	void display();
	void composeFrame();
	void setFallCount(int falls) { total_falls = falls * fall_factor; count_falls = 0; }
	gameState getState() const { return state; }

	bool match(const Block *right, bool ignoreActive = false) const;
	int downMatch(bool ignoreActive = false);
	int upMatch(bool ignoreActive = false);
	int leftMatch(bool ignoreActive = false);
	int rightMatch(bool ignoreActive = false);
	bool detectAndSetComboState();
	void detectAndSetFallState();
	int downDistance( Block *) const;
	int upDistance(Block *) const;
	int leftDistance(Block *) const;
	int rightDistance(Block *) const;
	void transferDown();
	Block * offsetRow(int n);
	Block * offsetCol(int n);
	
	static int interval_combo;
	//static int getComboInterval() { return interval_combo; }
	//static void setComboInterval(int n) { interval_combo = n; }
};

/* The Cursor class controls the operations on the player's cursor, like moving
   it around. */

class Cursor : public CObject {
protected:
	int row, col,
		cursor_delta;
	Grid *grid;

	static const string spriteFile;

public:
	static void declareActions();
	void defineActions();
	static void doAction(void *cursorInstance, int actionState, int actionType);
	static void mousePassiveMotion(void *cursorInstance, int x, int y);

	Cursor(Grid *, CBaseSprite *);
	~Cursor();
	bool moveUp(bool doDraw=true);
	bool moveDown(bool doDraw=true);
	bool moveLeft(bool doDraw=true);
	bool moveRight(bool doDraw=true);
	void setPos(int c, int r);
	int getRow() const { return row; }
	int getCol() const { return col; }
	void shiftRow();
};