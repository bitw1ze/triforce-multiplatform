#include "game.h"

// Constants
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

/* GameEnv methods */

GameEnv::GameEnv() { 
	showMenu = false;
	current_frame = 0; 
	loadImages(); 

	menu.Timer = Timer = new CTimer(); 
	Timer->start();
	last_time=Timer->time();
	last_pushtime = Timer->time();

	srand(time(NULL));

	grid = new Grid(this);
}

void GameEnv::displayGame() {
	composeFrame();
	background.drawGLbackground ();

	grid->display();

	glutSwapBuffers();
}

void GameEnv::processFrame()
{
	grid->setCoords();
}

void GameEnv::composeFrame()
{
	if (Timer->elapsed(last_pushtime, 900)) {
		grid->pushRow();
		last_pushtime = Timer->time();
	}

	if(Timer->elapsed(last_time,300))
	{
		processFrame();
		last_time=Timer->time();
		if(++current_frame>=1)
			current_frame=0;
	}
	glutPostRedisplay();
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
	  blockSprites[i]->loadFrame(frame, themeDirectory + blockFiles[i],
	                             r, g, b);
	  blockSprites[i]->loadGLTextures();
  }
}