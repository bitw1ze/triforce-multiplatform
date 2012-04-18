#ifndef __GAME_H__
#define __GAME_H__

#include <string>
#include <vector>
#include <ctime>
#include <deque>
#include "betterlib.h"
#include "globals.h"

using namespace std;
using namespace Globals;
using namespace BetterLib;

class GameEnv;
class Grid;
class Block;
class Cursor;
extern CTimer *mainTimer;

/* GameEnv is the main interface for controlling everything else within the 
   game. Classes other than GameEnv declared in this header will be
   implemented in their own .cpp files to make things more modular. This
   seems to be the best solution. */

class GameEnv {
protected:
	
	class Menu {
	private:
		static const string bgFile;
		BMPClass background;
		int current_frame,
			last_time;

	public:
		Menu();
		void display();
		void composeFrame();
		void processFrame();
		void loadImages();
	};

	/* bitmap files */
	static const string 
		blockFiles[],
		cursorFile,
		bgFile;

	/* frame stuff */
	int current_frame, 
		last_time,
		last_pushtime;

	bool showMenu;
	BMPClass background;

public:
	GameEnv();
	void display();
	void displayGame();
	void composeFrame();
	void init();
	void processFrame();
	void loadImages();
	int getWidth() { return background.getViewportWidth();} 
	int getHeight() { return background.getViewportHeight();} 

	CBaseSprite* blockSprites[nblocktypes];
	CBaseSprite *cursorSprite;
	Menu menu;
	Grid *grid;
	Cursor *cursor;
};

/* Grid class holds abstracts all the operations on grid of blocks for a 
   single player */

class Grid {

protected:
	int grid_x, grid_y, 
		row_xvel, row_yvel,
		block_w, block_h,
		speed;
	deque<Block *> blocks;
	CBaseSprite** blockSprites;

public:
	Grid(GameEnv *ge);
	void pushRow();
	void loadImages();
	void display();
	void setCoords();

	/* set/get properties */
	int getX() { return grid_x; }
	int getY() { return grid_y; }
	int getBlockWidth() { return block_w; }
	int getBlockHeight() { return block_h; }
	int getTopRow() { return blocks.size(); }
};

/* The Block class abstracts operations on a single block, such as getting and 
   setting the x and y values and setting states */

class Block : public GObject {
public:
	Block() : GObject() { }
};

/* The Cursor class controls the operations on the player's cursor, liie moving
   it around. */

class Cursor : public GObject {
protected:
	int row, col,
		cursor_w, cursor_h,
		cursor_x, cursor_y,
		cursor_delta;
	Grid *grid;

	static const string spriteFile;

public:
	Cursor(Grid *, CBaseSprite *);
	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();
	void setPos(int c, int r);
};

#endif