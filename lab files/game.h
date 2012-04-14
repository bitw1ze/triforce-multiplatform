
/* Gabe Pike
   CMPS 335
   Lab 02
   13 April 2012 */

#include "2DGraphics.h"
#include <string>
#include <vector>
#include <ctime>

string blockFiles[] = {"block-blue.bmp", "block-green.bmp", "block-purple.bmp", 
	"block-red.bmp", "block-special.bmp", "block-teal.bmp", "block-yellow.bmp"};

#define NBLOCKS 7
#define NCOLS 6
#define NROWS 10

class GameEnv {
protected:

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

GameEnv *gameEnv = NULL;

extern void init();