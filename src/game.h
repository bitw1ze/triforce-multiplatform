#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <deque>
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
		bgFile;

	/* frame stuff */
	int current_frame, 
		last_time,
		last_pushtime;

	BMPClass background;

public:
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

	CBaseSprite* blockSprites[nblocktypes];
	CBaseSprite *cursorSprite;
	Grid *grid;
};

/* Grid class holds abstracts all the operations on grid of blocks for a 
   single player */

class Grid {

protected:
	int row_xvel, row_yvel,
		block_w, block_h,
		grid_yspeed, grid_yoff;
	Point gridPos;
	GamePlay *gamePlay;
	deque<Block **> blocks;
	CBaseSprite** blockSprites;

public:
	Grid(GamePlay *ge);
	virtual ~Grid();
	void pushRow();
	void addRow();
	void loadImages();
	void display();
	void setCoords();
	void swapBlocks();
	int downMatch(int row, int col);
	int upMatch(int row, int col);
	int leftMatch(int row, int col);
	int rightMatch(int row, int col);
	void detectCombos(int r, int c);
	void killRows(Cell cells[4]);

	/* set/get properties */
	int getX() { return gridPos.x; }
	int getY() { return gridPos.y; }
	int getBlockWidth() { return block_w; }
	int getBlockHeight() { return block_h; }
	int getTopRow() { return blocks.size() - 1; }

	Cursor *cursor;
};

/* The Block class abstracts operations on a single block, such as getting and 
   setting the x and y values and setting states */

class Block : public CObject {
public:
	Block() : CObject() { }
	bool match(const Block *right) { 
		return getSprite() == right->getSprite() && enabled == right->enabled; 
	}
};

/* The Cursor class controls the operations on the player's cursor, liie moving
   it around. */

class Cursor : public CObject {
protected:
	int row, col,
		cursor_delta;
	Grid *grid;

	static const string spriteFile;

public:
	Cursor(Grid *, CBaseSprite *);
	void moveUp(bool doDraw=true);
	void moveDown(bool doDraw=true);
	void moveLeft(bool doDraw=true);
	void moveRight(bool doDraw=true);
	void setPos(int c, int r);
	int getRow() const { return row; }
	int getCol() const { return col; }
	void shiftRow();
	void passiveMouseHover(int x, int y);
};