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

class GamePlay {
protected:

	/* bitmap files */
	static const string 
		blockFiles[],
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

	void changeState(gameState gs);
	gameState getState();
};

/* Grid class holds abstracts all the operations on grid of blocks for a 
   single player */

class Grid {

public:
	enum gameState { play, combo, fall };
protected:
	int row_xvel, row_yvel,
		block_w, block_h,
		grid_yspeed, grid_yoff,
		last_push, last_combo,
		timer_push, timer_combo;
	Point gridPos;
	GamePlay *gamePlay;
	deque<Block **> blocks;
	CBaseSprite** blockSprites;
	gameState state;
	list<Cell *> combos;
	Cell *currentCombo;
	void composeFrame();

public:
	Grid(GamePlay *ge);
	virtual ~Grid();
	void pushRow();
	void addRow();
	void loadImages();
	void display();
	void setCoords();
	void swapBlocks();

	int downMatch(int row, int col, bool ignoreActive = false);
	int upMatch(int row, int col, bool ignoreActive = false);
	int leftMatch(int row, int col, bool ignoreActive = false);
	int rightMatch(int row, int col, bool ignoreActive = false);
	bool detectCombos(int r, int c);

	void onCombo();
	void onFall();
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
public: enum gameState { enabled, disabled, combo, fall };
protected:
	gameState state;
	void onCombo();
	CTimer *timer;
	int last_time;
	bool active;
public:

	Block();
	
	bool match(const Block &right, bool ignoreActive = false) const;
	bool swap(Block &right);
	void changeState(gameState gs);
	void display();
	bool isActive() const { return active; }
	void setActive(bool act) { active = act; }
	gameState getState() const { return state; }
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
	Cursor(Grid *, CBaseSprite *);
	bool moveUp(bool doDraw=true);
	bool moveDown(bool doDraw=true);
	bool moveLeft(bool doDraw=true);
	bool moveRight(bool doDraw=true);
	void setPos(int c, int r);
	int getRow() const { return row; }
	int getCol() const { return col; }
	void shiftRow();
	void passiveMouseHover(int x, int y);
};