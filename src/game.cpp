#include "game.h"

const string GameEnv::themeDirectory = "themes\\classic\\";
const string GameEnv::blockFiles[] = {"block-blue.bmp",
	                                  "block-green.bmp",
									  "block-purple.bmp",
									  "block-red.bmp",
									  "block-special.bmp",
									  "block-teal.bmp",
									  "block-yellow.bmp"};
const string GameEnv::bgFile = "bg.bmp";

#define rowloop 

void GameEnv::Menu::display()
{
}

GameEnv::GameEnv() { 
	showMenu = false;
	current_frame = 0; 
	Timer = new CTimer(); 
	Timer->start();
	last_time=Timer->time();
	LoadImages(); 
	init();

	srand(time(NULL));
}

void GameEnv::init()
{
	row_bottom = 0;
	row_top = 1;
	row_xvel = 0;
	row_yvel = 1;
	block_w = blockSprites[0]->GetWidth();
	block_h = blockSprites[0]->GetHeight();
	grid_x = background.getViewportWidth()/2 - (block_w * ncols)/2;
	grid_y = background.getViewportHeight() - block_h;

	for (int row=0; row<nrows/2; ++row) {
		pushRow(row);
	}
} 


void GameEnv::pushRow(int row) {
	for (int col=0; col<ncols; ++col) {
		blocks[row][col].create(grid_x + col * block_w, 
								grid_y - row * block_h,
							    row_xvel, row_yvel, 
								blockSprites[ rand() % nblocktypes ], 
								Timer);
		++row_bottom;
	}
}

bool GameEnv::LoadImages()
{
  background.load( themeDirectory + bgFile );
  background.loadGLTextures();

  int r=254, g=0, b=254, frameCount=1, frame=0;    // r,g,b is background color to be filtered, frameCount and frame number
  
  for (int i=0; i<nblocktypes; ++i) {
	  blockSprites[i] = new CBaseSprite(frameCount, background.getViewportWidth(), background.getViewportHeight());
	  blockSprites[i]->loadFrame(frame, GameEnv::themeDirectory + blockFiles[i], r, g, b);
	  blockSprites[i]->loadGLTextures();
  }

  return true;
}

