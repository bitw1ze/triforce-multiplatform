/*	gameplay.cpp
	by Gabe Pike
	Changes:
		- handed off control of pushing new rows onto the grid to grid.cpp
		- now displays a border around the grid
*/

#include "game.h"

// Constants
const string GamePlay::bgFile = "bg.bmp";
const string GamePlay::gridBorderFile = "gridborder.bmp";
const string GamePlay::blockFiles[] = {
	"block-blue.bmp",
	"block-green.bmp",
	"block-purple.bmp",
	"block-red.bmp",
	"block-special.bmp",
	"block-teal.bmp",
	"block-yellow.bmp"};
const string GamePlay::blockComboFiles[] = {
	"block-blue-combo.bmp",
	"block-green-combo.bmp",
	"block-purple-combo.bmp",
	"block-red-combo.bmp",
	"block-special-combo.bmp",
	"block-teal-combo.bmp",
	"block-yellow-combo.bmp"};

const string GamePlay::cursorFile = "cursor.bmp";

/* GamePlay methods */

GamePlay::GamePlay() { 
	state = play;
	current_frame = 0; 
	
	last_time=mainTimer->time();

	srand(time(NULL));
	loadImages(); 
	grid = new Grid(this);
}

void GamePlay::display() {
	composeFrame();
	background.drawGLbackground ();

	grid->display();

    gridBorderSprite->draw(0, grid->getX() - 21,
	                       grid->getY() - 35 - (grid->getBlockLength() * (nrows)));
	glutSwapBuffers();
}

void GamePlay::processFrame()
{
	//grid->setCoords();
}

void GamePlay::composeFrame()
{
	switch (state) {
	case play:
		grid->composeFrame();
		if(mainTimer->elapsed(last_time,300))
		{
			processFrame();
			last_time=mainTimer->time();
			if(++current_frame>=1)
				current_frame=0;
		}
		break;
	}
	glutPostRedisplay();
}


void GamePlay::loadImages()
{
  background.load( themeDirectory + bgFile );
  background.loadGLTextures();

  // r,g,b is background color to be filtered, frameCount and frame number
  int r=254, g=0, b=254, frameCount=2, frame=0;

  // load grid border
  gridBorderSprite = new CBaseSprite(frameCount, background.getViewportWidth(), background.getViewportHeight());
  gridBorderSprite->loadFrame(frame, themeDirectory + gridBorderFile, r, g, b);
  gridBorderSprite->loadGLTextures();
  
  for (int i=0; i<nblocktypes; ++i) {
	  // Block files are designed to not require a color filter. They aren't
	  // guaranteed to work as expected when their bg's are filtered out and a
	  // background image of a different color is used.
	  blockSprites[i] = new CBaseSprite(frameCount, background.getViewportWidth(), background.getViewportHeight());
	  blockSprites[i]->loadFrame(frame, themeDirectory + blockFiles[i], r, g, b);
	  blockSprites[i]->loadFrame(frame+1, themeDirectory + blockComboFiles[i], r, g, b);
	  blockSprites[i]->loadGLTextures();
  }

  r = 255, g = 255, b = 255;
  cursorSprite = new CBaseSprite(frameCount, background.getViewportWidth(), background.getViewportHeight());
  cursorSprite->loadFrame(frame, themeDirectory + cursorFile, r, g, b);
  cursorSprite->loadGLTextures();
}

void GamePlay::specialKeys(int key, int x, int y) {
	if (state == pause)
		return;

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
	switch (tolower(key)) { 
	case 'a':
		grid->swapBlocks();
		break;
	case 's':
		if (grid->getState() == Grid::play)
			grid->pushRow();
		break;
	case 'p':
		if (getState() == pause) {
			changeState(play);
		}
		else if (getState() == play) {
			cout << "pressed p\n";
			changeState(pause);
		}
		break;
	}

	glutPostRedisplay();
}

void GamePlay::changeState(gameState gs) {
	state = gs;
}

GamePlay::gameState GamePlay::getState() const {
	return state;
}