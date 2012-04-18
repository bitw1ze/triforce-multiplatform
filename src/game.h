#ifndef __GAME_H__
#define __GAME_H__

#include <string>
#include <vector>
#include <ctime>
#include <deque>
#include "block.h"

#define nrows 12
#define ncols 6
#define nblocktypes 7
#define rowloop 

using namespace std;

class GameEnv;
class Grid;

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
		CTimer *Timer; // yuck: points to same Timer as parent
		Menu();
		void display();
		void composeFrame();
		void processFrame();
		void loadImages();
	};

	Menu menu;
	Grid *grid;

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
	CTimer *Timer;
};

/* Grid class holds abstracts all the operations on grid of blocks for a single player */

class Grid {
protected:
	int grid_x, grid_y, 
		row_xvel, row_yvel, 
		block_w, block_h,
		row_bottom, row_top;
	deque<Block *> blocks;
	CBaseSprite** blockSprites;
public:
	CTimer *Timer;
	Grid(GameEnv *ge);
	void pushRow();
	void loadImages();
	void display();
	void setCoords();
};

#endif