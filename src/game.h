/*	game.h
	by Gabe Pike
	Changes:
		- added prototypes for methods used by other classes
		- created a Point struct and a Cell struct for better x/y and row/col handling
*/

#pragma once

#include <string>
#include <list>
#include <ctime>
#include <deque>
#include <vector>
#include "2DGraphics.h"
#include "globals.h"

using namespace std;
using namespace Globals;

class GridController;
class GamePlay;
class Block;
class Cursor;
class Grid;
class Cell;
class Combo;
class Fall;

extern CTimer *mainTimer;

/* GamePlay is the main interface for controlling everything else within the 
   game. Classes other than GamePlay declared in this header will be
   implemented in their own .cpp files to make things more modular. This
   seems to be the best solution. */
namespace PlayState {
	enum Actions {
		UP, DOWN, LEFT, RIGHT,
		SWAP, PUSH, PAUSE,
		_NUMBER_OF_ACTIONS
	};
    static const string actionLabels[_NUMBER_OF_ACTIONS] = {
		"Up", "Down", "Left", "Right"
		"Swap", "Push", "Pause"
	};
}

struct Point {
	int x, y;
};

class Cell {
public:
	Cell() { row = 0; col = 0; }
	Cell(const Cell &src) { row = src.row; col = src.col; }
	Cell(int r, int c, bool e = true) { row = r, col = c; }
	bool operator ==(const Cell &cell) { return row == cell.row && col == cell.col; }
	int row, col;
};

class GamePlay {
public:
	enum gameState {play, pause, quit};
protected:
	/* bitmap files */
	static const string 
		blockFiles[],
		blockComboFiles[],
		blockInactiveFiles[],
		cursorFiles[],
		gridBorderFile,
		bgFile;
	static const int numCursorFiles;

	/* frame stuff */
	int current_frame, 
		last_time;

	BMPClass background;

	gameState state;

public:
	static void declareActions();
	void defineActions();
	static void doAction(void *gridInstance, int actionState, int actionType);

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
	gameState getState() const;
};

/* Grid class abstracts all the operations on grid of blocks for a 
   single player */

class Grid {

public:
	GridController *gridController;
	enum gameState { play, combo };
protected:
	int block_w, block_h,
		grid_yspeed, grid_yoff,
		lastPush, pushInterval, comboInterval,
		last_cursor_anim, timer_cursor_anim, current_cursor_frame;
	Point gridPos;
	GamePlay *gamePlay;
	CBaseSprite** blockSprites;
	gameState state;

public:
	static void declareActions();
	void defineActions();
	static void doAction(void *gridInstance, int actionState, int actionType);

	Grid(GamePlay *gp);
	virtual ~Grid();

	void loadImages();
	void display();
	void composeFrame();

	void pushRow();
	void addRow();
	void swapBlocks();

	void changeState(gameState gs);
	gameState getState() const { return state; }

	/* set/get properties */
	int getYOffset() { return grid_yoff; }
	int getX() { return gridPos.x; }
	int getY() { return gridPos.y; }
	int getBlockLength() { return block_h; }
	int countEnabledRows() const;

	int downMatch(int r, int c, bool ignoreActive = false);
	int upMatch(int r, int c, bool ignoreActive = false);
	int leftMatch(int r, int c, bool ignoreActive = false);
	int rightMatch(int r, int c, bool ignoreActive = false);

	void incComboInterval(int interval);

	bool containsPoint(int x, int y);

	void printDebug();

	Cursor *cursor;
	deque< vector<Block> > blocks; //i.e. blocks[row][col]
};

/* The Block class abstracts operations on a single block, such as getting and 
   setting the x and y values and setting states */

class Block : public CObject {
public: enum gameState { inactive, enabled, disabled, combo, fall };
protected:
	gameState state;
	int fallOffset;

public:
	Block();
	Block(const Block &block);
	Block & operator =(const Block &block);
	void clone(const Block &src);
	~Block() {}

	friend bool swap(Block &left, Block &right);
	friend bool match(const Block &left, const Block &right, bool ignoreActive = false);

	void display();
	void composeFrame();
	void changeState(gameState gs);
	gameState getState() const { return state; }
	
	Grid *grid;

	void fallDown();
	void resetFall();
	int getFallOffset() const;
	void setFallOffset(int f);

	static const int fallFactor;
};

class Combo {
public: 
	static int comboInterval;
	static enum comboState { NONE, HORI, VERT, MULTI };
protected:
	Cell *_left, *_right, *_up, *_down, *_mid;
	list<Cell> combo;
	int interval;
	int startTime;
	comboState state;

public:
	Combo();
	Combo(const Combo &ge);
	Combo & operator =(const Combo &ge);
	bool operator ==(const Combo &ev);
	void clone(const Combo &ge);

	void init();

	Cell *left() const { return _left; }
	Cell *left(int r, int c);
	Cell *right() const { return _right; }
	Cell *right(int r, int c);
	Cell *down() const { return _down; }
	Cell *down(int r, int c);
	Cell *up() const { return _up; }
	Cell *up(int r, int c);
	Cell *mid() const { return _mid; }
	Cell *mid(int r, int c);

	comboState getState() { return state; }
	void changeState(comboState st) { state = st; }

	const list<Cell> getList();

	int count() const;
	void activate();
	bool elapsed() const;

	void printDebug();
};

class Fall : public Cell {
public:
	static int fallInterval;
	int lastFall;
	int numFalls;
	bool enabled;

public:
	Fall();
	Fall(int r, int c);
	Fall(const Fall &src);
	Fall & operator =(const Fall &src);
	bool operator ==(const Fall &fl);
	void init();
	void clone(const Fall &src);

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

class GridController {
protected:
	Grid *grid;
	
	list<Combo> comboEvents;
	list<Fall> fallEvents;	
public:
	GridController(Grid *g = NULL);
	GridController(const GridController &g);
	GridController & operator =(const GridController &);
	~GridController();
	void set(Grid *g);
	void clone(const GridController &g);
	
	void composeFrame();

	bool checkComboFinished(Combo &ev);
	bool detectFallAfterCombo(Combo &e);
	bool detectFall(Fall &cell);

	void initFallState(Fall &fall);
	void cleanupFall(Fall &fall);
	void doFall(Fall &cell);
	bool detectCombo(Cell &cell);

	void initComboState(Combo &combo);
	void setBlockStates( list<Cell> &, Block::gameState gs);
};
