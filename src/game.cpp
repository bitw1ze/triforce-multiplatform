#include "game.h"

// Constants
const string GamePlay::bgFile = "bg.bmp";
const string GamePlay::blockFiles[] = {
	"block-blue.bmp",
	"block-green.bmp",
	"block-purple.bmp",
	"block-red.bmp",
	"block-special.bmp",
	"block-teal.bmp",
	"block-yellow.bmp"};
const string GamePlay::cursorFile = "cursor.bmp";


/* GamePlay methods */

GamePlay::GamePlay() { 
	current_frame = 0; 
	
	last_time=mainTimer->time();
	last_pushtime = mainTimer->time();

	srand(time(NULL));
	loadImages(); 
	grid = new Grid(this, cursorSprite);
}

void GamePlay::display() {
	composeFrame();
	background.drawGLbackground ();

	grid->display();
	//printf("(%d, %d)\n", cursor->getX(), cursor->getY());

	glutSwapBuffers();
}

void GamePlay::processFrame()
{
	//grid->setCoords();
}

void GamePlay::composeFrame()
{
	if (mainTimer->elapsed(last_pushtime, 500)) {
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


void GamePlay::loadImages()
{
  background.load( themeDirectory + bgFile );
  background.loadGLTextures();

  // r,g,b is background color to be filtered, frameCount and frame number
  int r=254, g=0, b=254, frameCount=1, frame=0;
  
  for (int i=0; i<nblocktypes; ++i) {
	  // Block files are designed to not require a color filter. They aren't
	  // guaranteed to work as expected when their bg's are filtered out and a
	  // background image of a different color is used.
	  blockSprites[i] = new CBaseSprite(frameCount, background.getViewportWidth(), background.getViewportHeight());
	  blockSprites[i]->loadFrame(frame, themeDirectory + blockFiles[i], r, g, b);
	  blockSprites[i]->loadGLTextures();
  }

  r = 255, g = 255, b = 255;
  cursorSprite = new CBaseSprite(frameCount, background.getViewportWidth(), background.getViewportHeight());
  cursorSprite->loadFrame(frame, themeDirectory + cursorFile, r, g, b);
  cursorSprite->loadGLTextures();
}

void GamePlay::specialKeys(int key, int x, int y) {
	switch(key) {
	case GLUT_KEY_LEFT:
		grid->cursor->moveLeft(); //FIXME: this really begs for refactoring
		break;

	case GLUT_KEY_RIGHT:
		grid->cursor->moveRight();
		break;

	case GLUT_KEY_UP:
		grid->cursor->moveUp();
		break;

	case GLUT_KEY_DOWN:
		grid->cursor->moveDown();
		break;
	}

	glutPostRedisplay();
}

void GamePlay::normalKeys(unsigned char key, int x, int y) {
	switch (key) { 
	case ESC:
		exit(0);
		break;
	case 'z':
	case 'Z':
		grid->swapBlocks();
		break;
	}
}