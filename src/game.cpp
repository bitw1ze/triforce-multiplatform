#include "game.h"

// Constants
const string GameEnv::bgFile = "bg.bmp";
const string GameEnv::Menu::bgFile = "bg.bmp";
const string GameEnv::blockFiles[] = {
	"block-blue.bmp",
	"block-green.bmp",
	"block-purple.bmp",
	"block-red.bmp",
	"block-special.bmp",
	"block-teal.bmp",
	"block-yellow.bmp"};
const string GameEnv::cursorFile = "cursor.bmp";


/* GameEnv methods */

GameEnv::GameEnv() { 
	showMenu = false;
	current_frame = 0; 
	
	mainTimer = new CTimer();
	mainTimer->start();
	last_time=mainTimer->time();
	last_pushtime = mainTimer->time();

	srand(time(NULL));
	loadImages(); 
	grid = new Grid(this);
	cursor = new Cursor(grid, cursorSprite);
}

void GameEnv::display() {
	if (showMenu)
		menu.display();
	else
		displayGame();
}

void GameEnv::displayGame() {
	composeFrame();
	background.drawGLbackground ();

	grid->display();
	//printf("(%d, %d)\n", cursor->getX(), cursor->getY());
	cursor->draw(0);

	glutSwapBuffers();
}

void GameEnv::processFrame()
{
	grid->setCoords();
}

void GameEnv::composeFrame()
{
	if (mainTimer->elapsed(last_pushtime, 5000)) {
		grid->pushRow();
		last_pushtime = mainTimer->time();
	}

	if(mainTimer->elapsed(last_time,300))
	{
		processFrame();
		last_time=mainTimer->time();
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
	  blockSprites[i] = new CBaseSprite(frameCount, background.getViewportWidth(), background.getViewportHeight());
	  blockSprites[i]->loadFrame(frame, themeDirectory + blockFiles[i], r, g, b);
	  blockSprites[i]->loadGLTextures();
  }

  r = 255, g = 255, b = 255;
  cursorSprite = new CBaseSprite(frameCount, background.getViewportWidth(), background.getViewportHeight());
  cursorSprite->loadFrame(frame, themeDirectory + cursorFile, r, g, b);
  cursorSprite->loadGLTextures();
}