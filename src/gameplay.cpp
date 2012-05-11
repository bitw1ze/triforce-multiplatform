/*	gameplay.cpp
		
GamePlay is the entry point of the actual game. It is responsible for 
initializing the gameplay area. It loads the resources needed in the game, 
calls the Grid display and composeFrame functions, and controls the state 
of the game. It is also responsible for cleaning up when the game is finished.
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

int GamePlay::blockLength;
int GamePlay::gridHeight;
int GamePlay::gridWidth;

GamePlay::GamePlay() { 
	state = play;
	current_frame = 0; 
	
	last_time=mainTimer->time();

	srand(time(NULL));
	loadImages(); 
	grid = new Grid(this);
	hud = new HUD(grid->getX() + (float)gridWidth * 1.15, grid->getY() + .03 * (float)gridHeight);
	defineActions();
}

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

	defineAction(scope, state, PAUSE, this, doAction); // duplicate definition (OK)
}

void GamePlay::doAction(void *gamePlayInstance, int actionState, int actionType)
{
	using namespace PlayState;

	GamePlay *g = (GamePlay *)gamePlayInstance;
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
	// FIXME: safe to remove this?
	//glutPostRedisplay();
}

void GamePlay::display() {
	composeFrame();

	background.drawGLbackground ();
	grid->display();
	hud->display();
	int xPos = grid->getX() - 21,
	yPos = grid->getY() - 35 - gridHeight;
    gridBorderSprite->draw(0, xPos, yPos);

	glutSwapBuffers();
}

#if 0
void GamePlay::processFrame()
{
	//grid->setCoords();
}
#endif

void GamePlay::composeFrame()
{
	switch (state) {
	case play:
		grid->composeFrame();
		hud->composeFrame();
		if(mainTimer->elapsed(last_time,300))
		{
			//processFrame();
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

  GamePlay::blockLength = blockSprites[0]->GetHeight();
  GamePlay::gridHeight = GamePlay::blockLength * nrows;
  GamePlay::gridWidth = GamePlay::blockLength * ncols;
}

void GamePlay::changeState(gameState gs) {
	state = gs;
}

GamePlay::gameState GamePlay::getState() const {
	return state;
}