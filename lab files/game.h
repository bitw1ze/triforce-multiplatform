
/* Gabe Pike
   CMPS 335
   Lab 02
   13 April 2012 */

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