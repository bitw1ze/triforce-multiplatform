#ifndef __GAME_H__
#define __GAME_H__

#include "2DGraphics.h"
#include "block.h"
#include <string>
#include <vector>
#include <ctime>

#define nblocktypes 7
#define nrows 10
#define ncols 6

using namespace std;

class GameEnv {
protected:
	static const string blockFiles[];
	static const string bgFile;
	static const string themeDirectory;

	int grid_x, grid_y, 
		row_xvel, row_yvel, 
		block_w, block_h,
		grid_bottomRow, grid_topRow;

	void pushRow(int row);
public:
	CTimer *Timer;
	BMPClass background;
	CBaseSprite* blockSprites[nrows][ncols];
	Block blocks[nrows][ncols];
	int current_frame;
	int last_time;

	GameEnv();
	void ComposeFrame();
	void init();
	bool ProcessFrame();
	bool LoadImages();
	void display();
};

const string GameEnv::themeDirectory = "themes\\classic\\";
const string GameEnv::blockFiles[] = {"block-blue.bmp",
	                                  "block-green.bmp",
									  "block-purple.bmp",
									  "block-red.bmp",
									  "block-special.bmp",
									  "block-teal.bmp",
									  "block-yellow.bmp"};
const string GameEnv::bgFile = "bg.bmp";

#endif