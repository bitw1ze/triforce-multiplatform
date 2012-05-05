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

class GamePlay {
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
	void specialKeys(int key, int x, int y);
	void normalKeys(unsigned char key, int x, int y);
	void keyUp(unsigned char key, int x, int y);

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
		last_fall, timer_fall;
	Point gridPos;
	GamePlay *gamePlay;
	deque< vector<Block> > blocks;
	CBaseSprite** blockSprites;
	gameState state;

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

	list<Block> & downMatch(int r, int c, bool ignoreActive = false);
	list<Block> & upMatch(int r, int c, bool ignoreActive = false);
	list<Block> & leftMatch(int r, int c, bool ignoreActive = false);
	list<Block> & rightMatch(int r, int c, bool ignoreActive = false);

	list<Block> & detectCombo(int r, int c);
	list<Block> & setComboState(list<Block> &);
	list<Block> & detectFalls(int r, int c);
	list<Block> & setFallState(list<Block> &);

	bool containsPoint(int x, int y);
	Cursor *cursor;
};

/* T:w
he Block class abstracts operations on a single block, such as getting and 
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

	//static int interval_combo;
public:
	Block();
	Block(const Block &block);
	Block & operator =(Block &block);
	~Block() {}
	bool swap(Block &right);
	void changeState(gameState gs);
	void display();
	void composeFrame();
	void setFallCount(int falls) { total_falls = falls * fall_factor; count_falls = 0; }
	gameState getState() const { return state; }

	bool match(const Block &right, bool ignoreActive = false) const;
	
	static int interval_combo;
	int getComboInterval() { return total_combo_interval; }
	void setComboInterval(int n) { total_combo_interval = n; }

	Grid *grid;
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