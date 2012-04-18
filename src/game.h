#ifndef __GAME_H__
#define __GAME_H__

#include <string>
#include <vector>
#include <ctime>
#include <deque>
#include "2DGraphics.h"
#include "globals.h"

using namespace std;
using namespace Globals;

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

	Menu menu;
	Grid *grid;
	Cursor *cursor;

	static const string 
		blockFiles[],
		bgFile,
		themeDirectory;

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
};

/* Grid class holds abstracts all the operations on grid of blocks for a 
   single player */

class Grid {

protected:
	int grid_x, grid_y, 
		row_xvel, row_yvel,
		row_bottom, row_top,
		block_w, block_h;
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
};

/* The Block class abstracts operations on a single block, such as getting and 
   setting the x and y values and setting states */

class Block : public CObject {
public:
	// A block will only be displayed if it is enabled.
	bool enabled;

	Block() : CObject() { enabled = false; }
	void create(int x1, int y1, int xspeed1, int yspeed1, CBaseSprite *sprite, CTimer *timer);
	int getX() { float x, y; Getxy(x, y); return (int)x; }
	int getY() { float x, y; Getxy(x, y); return (int)y; }
	void setX(int x) { Setxy((float)x, getY()); }
	void setY(int y) { Setxy(getX(), (float)y); }
};

class Cursor {
protected:
	CObject *cursor;
	int row, col;
	Grid *grid;

	static const string spriteFile;

public:
	Cursor(Grid *g);
	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();
};

#endif