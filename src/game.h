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

class GamePlay;
class Grid;
class Block;
class Cursor;
class Combo;
extern CTimer *mainTimer;

/* GamePlay is the main interface for controlling everything else within the 
   game. Classes other than GamePlay declared in this header will be
   implemented in their own .cpp files to make things more modular. This
   seems to be the best solution. */

struct Point {
	int x, y;
};

class Cell {
public:
	Cell() { row = 0; col = 0; }
	Cell(int r, int c) { row = r, col = c; }
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
		cursorFile,
		gridBorderFile,
		bgFile;

	/* frame stuff */
	int current_frame, 
		last_time;

	BMPClass background;

	gameState state;

public:
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
	void specialKeys(int key, int x, int y);
	void normalKeys(unsigned char key, int x, int y);
	void keyUp(unsigned char key, int x, int y);

	void changeState(gameState gs);
	gameState getState() const;
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
		last_fall, timer_fall;
	Point gridPos;
	GamePlay *gamePlay;
	CBaseSprite** blockSprites;
	gameState state;
	list<Combo> combos;

public:
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

	Combo & detectCombo(int r, int c);
	int detectFalls(int r, int c);
	void setFallState(Combo &);
	void incComboInterval(int interval);

	bool containsPoint(int x, int y);

	Cursor *cursor;
	deque< vector<Block> > blocks;
};

/* The Block class abstracts operations on a single block, such as getting and 
   setting the x and y values and setting states */

class Block : public CObject {
public: enum gameState { inactive, enabled, disabled, combo, fall };
protected:
	gameState state;
	CTimer *timer;
	//int last_fall, interval_fall, total_falls, count_falls, fall_factor;

public:
	Block();
	Block(const Block &block);
	Block & operator =(const Block &block);
	~Block() {}
	friend bool swap(Block &left, Block &right);
	void display();
	void composeFrame();
	//void setFallCount(int falls) { total_falls = falls * fall_factor; count_falls = 0; }
	void changeState(gameState gs);
	gameState getState() const { return state; }

	friend bool match(const Block &left, const Block &right, bool ignoreActive = false);
	
	Grid *grid;
};

class Combo {
protected:
	Cell *_left, *_right, *_up, *_down, *_mid;
	Grid *grid;
	CTimer timer;
	int interval;
	int startTime;
	list<Cell> combo;

	static int comboInterval;

	bool isVert() const;
	bool isHori() const;
	bool isMulti() const;
	
public:
	Combo(Grid *grid);
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

	bool isCombo() const;
	bool isVertCombo() const;
	bool isHoriCombo() const;
	bool isMultiCombo() const;

	static bool areFinished(list<Combo> &combos);
	static bool finish(list<Combo> &combos);

	void startTimer();
	bool initComboState();
	bool isFinished();
	void setBlockStates(Block::gameState gs);
	int count() const;

	void printDebug();
	void printStates();
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
	static void mousePassiveMotion(void *gridInstance, int x, int y);

	Cursor(Grid *, CBaseSprite *);
	bool moveUp(bool doDraw=true);
	bool moveDown(bool doDraw=true);
	bool moveLeft(bool doDraw=true);
	bool moveRight(bool doDraw=true);
	void setPos(int c, int r);
	int getRow() const { return row; }
	int getCol() const { return col; }
	void shiftRow();
};