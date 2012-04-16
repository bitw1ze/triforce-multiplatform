#include "game.h"

const string GameEnv::themeDirectory = "themes\\classic\\";
const string GameEnv::blockFiles[] = {
	"block-blue.bmp",
	"block-green.bmp",
	"block-purple.bmp",
	"block-red.bmp",
	"block-special.bmp",
	"block-teal.bmp",
	"block-yellow.bmp"};
const string GameEnv::bgFile = "bg.bmp";
const string GameEnv::Menu::bgFile = "bg.bmp";

#define rowloop 

GameEnv::Menu::Menu() { 
	current_frame = 0; 
	loadImages(); 
}

void GameEnv::Menu::loadImages()
{
  background.load( themeDirectory + bgFile );
  background.loadGLTextures();

  // r,g,b is background color to be filtered, frameCount and frame number
  //  int r=254, g=0, b=254, frameCount=1, frame=0;
}

GameEnv::GameEnv() { 
	showMenu = false;
	current_frame = 0; 
	loadImages(); 
	init();
	srand(time(NULL));
}

void GameEnv::init()
{

	menu.Timer = Timer = new CTimer(); 
	Timer->start();
	last_time=Timer->time();
	last_pushtime = Timer->time();

	row_bottom = 0;
	row_top = 1;
	row_xvel = 0;
	row_yvel = 1;
	block_w = blockSprites[0]->GetWidth();
	block_h = blockSprites[0]->GetHeight();
	grid_x = background.getViewportWidth()/2 - (block_w * ncols)/2;
	grid_y = background.getViewportHeight() - block_h * 2;

	for (int row=0; row<nrows/2; ++row) {
		pushRow(row);
	}
} 

void GameEnv::pushRow(int row) {
	if (blocks.size() == nrows)
		blocks.pop_back();

	Block *blockRow = new Block[ncols];

	for (int col=0; col<ncols; ++col) {
		blockRow[col].create(grid_x + col * block_w, -block_h * 2,
							 row_xvel, row_yvel, 
							 blockSprites[ rand() % nblocktypes ], 
							 Timer);
	}
	row_bottom = (row_bottom + 1) % nrows;

	blocks.push_front(blockRow);
}

void GameEnv::loadImages()
{
  background.load( themeDirectory + bgFile );
  background.loadGLTextures();

  // r,g,b is background color to be filtered, frameCount and frame number
  int r=254, g=0, b=254, frameCount=1, frame=0;
  
  for (int i=0; i<nblocktypes; ++i) {
	  blockSprites[i] = new CBaseSprite(frameCount, background.getViewportWidth(),
		                                background.getViewportHeight());
	  blockSprites[i]->loadFrame(frame, GameEnv::themeDirectory + blockFiles[i],
	                             r, g, b);
	  blockSprites[i]->loadGLTextures();
  }
}

