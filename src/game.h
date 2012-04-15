#ifndef __GAME_H__
#define __GAME_H__

#include "2DGraphics.h"
#include <string>
#include <vector>
#include <ctime>

using namespace std;

#define NBLOCKTYPES 7
#define NCOLS 6
#define NROWS 10

class GameEnv {
protected:
	static const string blockFiles[];
	static const string GameEnv::bgFile;
	static const string themeDirectory;

	int grid_x, grid_y, row_xvel, row_yvel, block_w, block_h;

	void createBlockRow(int row);
public:
	CTimer *Timer;
	BMPClass background;
	CBaseSprite* blockSprites[NROWS][NCOLS];
	CObject blocks[NROWS][NCOLS];
	int current_frame;
	int last_time;

	GameEnv();
	void ComposeFrame();
	void CreateObjects();
	bool ProcessFrame();
	bool LoadImages();
};

#endif