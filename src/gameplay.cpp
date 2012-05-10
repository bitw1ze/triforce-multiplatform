/*	gameplay.cpp
	by Gabe Pike
	Changes:
		- handed off control of pushing new rows onto the grid to grid.cpp
		- now displays a border around the grid
*/

#include "game.h"
#include "input.h"

// Constants
const string GamePlay::bgFile = "bg-play.bmp";
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
const string GamePlay::blockInactiveFiles[] = {
	"block-blue-inactive.bmp",
	"block-green-inactive.bmp",
	"block-purple-inactive.bmp",
	"block-red-inactive.bmp",
	"block-special-inactive.bmp",
	"block-teal-inactive.bmp",
	"block-yellow-inactive.bmp"};

const int GamePlay::numCursorFiles = 10;
const string GamePlay::cursorFiles[] = {
	"cursor1.bmp", "cursor2.bmp", "cursor3.bmp", "cursor4.bmp", "cursor5.bmp",
	"cursor6.bmp", "cursor7.bmp", "cursor8.bmp", "cursor9.bmp", "cursor10.bmp"
};

/* GamePlay methods */
void GamePlay::declareActions()
{
	using namespace Input;
	using namespace PlayState;

	Action::ActionScope scope = Action::SCOPE_FIRST_PLAYER;
	Triforce::GameState state = Triforce::PLAY;

	declareAction(scope, state, PAUSE, actionLabels[PAUSE]);
}
void GamePlay::defineActions()
{
	using namespace Input;
	using namespace PlayState;

	Action::ActionScope scope = Action::SCOPE_FIRST_PLAYER;
	Triforce::GameState state = Triforce::PLAY;

	defineAction(scope, state, PAUSE, this, doAction);
}

void GamePlay::doAction(void *gridInstance, int actionState, int actionType)
{
	using namespace PlayState;

	GamePlay *g = (GamePlay *)gridInstance;
	switch((enum Input::Action::ActionState)actionState)
	{
	case Input::Action::STATE_PRESS:
		switch((enum Actions)actionType)
		{
		case PAUSE:
			if (g->getState() == pause) {
				g->changeState(GamePlay::play);
			}
			else if (g->getState() == GamePlay::play) {
				cout << "pressed pause\n";
				g->changeState(GamePlay::pause);
			}
			break;
		}
	/*
	case Input::Action::STATE_HOLD:
	case Input::Action::STATE_RELEASE:
	*/
	}
	glutPostRedisplay();
}

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
  int r=254, g=0, b=254, frameCount=3, frame=0;

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
	  blockSprites[i]->loadFrame(frame+2, themeDirectory + blockInactiveFiles[i], r, g, b);
	  blockSprites[i]->loadGLTextures();
  }

  cursorSprite = new CBaseSprite(numCursorFiles, background.getViewportWidth(), background.getViewportHeight());
  for (int i = 0; i < numCursorFiles; ++i)
	  cursorSprite->loadFrame(i, themeDirectory + cursorFiles[i], r, g, b);
  cursorSprite->loadGLTextures();
}

void GamePlay::changeState(gameState gs) {
	state = gs;
}

GamePlay::gameState GamePlay::getState() const {
	return state;
}