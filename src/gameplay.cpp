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
const string GamePlay::bonusFile = "bonus.bmp";

const int GamePlay::numCursorFiles = 10;

int GamePlay::blockLength;
int GamePlay::gridHeight;
int GamePlay::gridWidth;
void * GamePlay::font1 = GLUT_BITMAP_TIMES_ROMAN_24;
float GamePlay::fcolor1[3] = {1, 1, 1};
float GamePlay::fcolor2[3] = {0, 0, 0};
CBaseSprite * GamePlay::blockSprites[nblocktypes];
CBaseSprite * GamePlay::cursorSprite = NULL;
CBaseSprite * GamePlay::gridBorderSprite = NULL;
CBaseSprite * GamePlay::bonusSprite = NULL;
CBaseSprite * GamePlay::chainFontSprite = NULL;
CBaseSprite * GamePlay::comboFontSprite = NULL;
BMPClass GamePlay::background;

GamePlay::GamePlay() { 
	srand(time(NULL));

	state = play;
	loadImages(); 
	grid = new Grid();
	grid->setDifficulty(Grid::MEDIUM); // should be dynamically set
	hud = new HUD(grid->getX() + (float)gridWidth * 1.15, grid->getY() + .03 * (float)gridHeight);

	menuButtons = new Buttons(this->getWidth(), this->getHeight());
	int yOffset = 85,
		xOffset = 150,
		xpos = this->getWidth() - xOffset,
		ypos = this->getHeight() - yOffset;
	menuButtons->add(this, GamePlay::quit, changeStateWrapper, Triforce::backBtns, xpos, ypos);
	ypos -= yOffset;
	menuButtons->add(this, GamePlay::play, changeStateWrapper, Triforce::playBtns, xpos, ypos);
	menuButtons->add(this, GamePlay::pause, changeStateWrapper, Triforce::pauseBtns, xpos, ypos);
	menuButtons->disable(GamePlay::play);

	Input::addMousePassiveMotionFunc(menuButtons, Triforce::PLAY,
								 	 menuButtons->mousePassiveMotion);
	Input::addMousePassiveMotionFunc(menuButtons, Triforce::PAUSE,
								 	 menuButtons->mousePassiveMotion);
	// same as passive
	Input::addMouseMotionFunc(menuButtons, Triforce::PLAY,
	                          menuButtons->mousePassiveMotion);
	Input::addMouseMotionFunc(menuButtons, Triforce::PAUSE,
	                          menuButtons->mousePassiveMotion);
	defineActions();
}

/* GamePlay methods */
void GamePlay::declareActions()
{
	using namespace Input;
	using namespace PlayState;

	Action::ActionScope scope = Action::SCOPE_FIRST_PLAYER;
	Triforce::GameState state = Triforce::PLAY;

	declareAction(scope, state, PAUSE_TOGGLE, actionLabels[PAUSE_TOGGLE]);
	declareAction(scope, state, RETURN, actionLabels[RETURN]);

	state = Triforce::PAUSE;
	declareAction(scope, state, PAUSE_TOGGLE, actionLabels[PAUSE_TOGGLE]);
}
void GamePlay::defineActions()
{
	using namespace Input;
	using namespace PlayState;

	Action::ActionScope scope = Action::SCOPE_FIRST_PLAYER;
	Triforce::GameState state = Triforce::PLAY;

	defineAction(scope, state, PAUSE_TOGGLE, this, doAction);
	defineAction(scope, state, RETURN, this, doAction);

	state = Triforce::PAUSE;
	defineAction(scope, state, PAUSE_TOGGLE, this, doAction);
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
		case PAUSE_TOGGLE:
			if (g->state == pause)
				g->changeState(play);
			else if (g->state == play)
				g->changeState(pause);
			break;
		}
	/*
	case Input::Action::STATE_HOLD:
	case Input::Action::STATE_RELEASE:
	*/
	}
}

void GamePlay::display() {
	if (state == GamePlay::quit)
		Triforce::setState(Triforce::MENU);
	composeFrame();

	background.drawGLbackground ();
	grid->display();
	hud->display();
	int xPos = grid->getX() - 21,
	yPos = grid->getY() - 35 - gridHeight;
    gridBorderSprite->draw(0, xPos, yPos);
	grid->displayBonus();
	menuButtons->display();

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

		break;
	}
	glutPostRedisplay();
}


void GamePlay::loadImages()
{
	string cwd;

	cwd = themeDirectory;
	background.load( cwd + bgFile );
	background.loadGLTextures();

	// r,g,b is background color to be filtered, frameCount and frame number
	int r=254, g=0, b=254, frameCount, frame=0;

	// load grid border
	gridBorderSprite = new CBaseSprite(frameCount=1, screen_w, screen_h);
	gridBorderSprite->loadFrame(frame, themeDirectory + gridBorderFile, r, g, b);
	gridBorderSprite->loadGLTextures();
  
	char num[8];
	cwd = themeDirectory + "blocks\\";
	for (int i=0; i<nblocktypes; ++i) {
		// Block files are designed to not require a color filter. They aren't
		// guaranteed to work as expected when their bg's are filtered out and a
		// background image of a different color is used.
		
		sprintf_s(num, "%d", i);
		blockSprites[i] = new CBaseSprite(frameCount = 5, screen_w, screen_h);
		blockSprites[i]->loadFrame(frame+0, cwd + "normal\\" + num + ".bmp", r, g, b);
		blockSprites[i]->loadFrame(frame+1, cwd + "combo\\" + num + "-0.bmp", r, g, b);
		blockSprites[i]->loadFrame(frame+2, cwd + "combo\\" + num + "-1.bmp", r, g, b);
		blockSprites[i]->loadFrame(frame+3, cwd + "combo\\" + num + "-0.bmp", r, g, b);
		blockSprites[i]->loadFrame(frame+4, cwd + "inactive\\" + num + ".bmp", r, g, b);
		blockSprites[i]->loadGLTextures();
	}

  cursorSprite = new CBaseSprite(numCursorFiles, screen_w, screen_h);
  cwd = themeDirectory + "cursor\\";
  for (int i = 0; i < numCursorFiles; ++i) {
	  sprintf_s(num, "%d", i);
	  cursorSprite->loadFrame(i, cwd + num + ".bmp", r, g, b);
  }
  cursorSprite->loadGLTextures();

  // bonus sprite
  cwd = themeDirectory + "bonus\\";
  bonusSprite = new CBaseSprite(2, screen_w, screen_h);
  bonusSprite->loadFrame(0, cwd + "chain.bmp", r, g, b);
  bonusSprite->loadFrame(1, cwd + "combo.bmp", r, g, b);
  bonusSprite->loadGLTextures();

  r = 254;
  g = 0;
  b = 254;
  int maxChain = 15, minChain = 2;
  chainFontSprite = new CBaseSprite(maxChain - minChain + 1, screen_w, screen_h);
  for (int i = minChain; i <= maxChain; ++i) {
	  sprintf_s(num, "x%d", i); 
	  chainFontSprite->loadFrame(i - minChain, cwd + num + ".bmp", r, g, b);
  }
  chainFontSprite->loadGLTextures();
  
  // load combo sprite
  int maxCombo = 10, minCombo = 4;
  comboFontSprite = new CBaseSprite(maxCombo - minCombo + 1, screen_w, screen_h);
  for (int i = minCombo; i <= maxCombo; ++i) {
	  sprintf_s(num, "%d", i); 
	  comboFontSprite->loadFrame(i - minCombo, cwd + num + ".bmp", r, g, b);
  }
  comboFontSprite->loadGLTextures();

  GamePlay::blockLength = blockSprites[0]->GetHeight();
  GamePlay::gridHeight = GamePlay::blockLength * nrows;
  GamePlay::gridWidth = GamePlay::blockLength * ncols;
}

void GamePlay::changeState(gameState gs) {
	if (state == play && gs == pause)
	{
		Triforce::setState(Triforce::PAUSE); // used to disable/enable actions
		menuButtons->disable(pause);
		menuButtons->enable(play);
		state = pause;
		mainTimer->pause();
	}
	else if (state == pause && gs == play)
	{
		Triforce::setState(Triforce::PLAY); // used to disable/enable actions
		menuButtons->disable(play);
		menuButtons->enable(pause);
		state = play;
		mainTimer->unpause();
	}
	else
		state = gs;
}

void GamePlay::changeStateWrapper(void *gamePlayInstance, int state) {
	GamePlay *g = (GamePlay *)gamePlayInstance;
	g->changeState((gameState)state);
}

GamePlay::gameState GamePlay::getState() const {
	return state;
}