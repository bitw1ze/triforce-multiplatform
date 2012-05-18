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
	current_frame = 0; 
	
	last_time=mainTimer->time();

	loadImages(); 
	grid = new Grid();
	hud = new HUD(grid->getX() + (float)gridWidth * 1.15, grid->getY() + .03 * (float)gridHeight);

	menuButtons = new Buttons(this->getWidth(), this->getHeight());
	int yOffset = 85,
		xOffset = 150,
		xpos = this->getWidth() - xOffset,
		ypos = this->getHeight() - yOffset;
	menuButtons->add(this, GamePlay::quit, changeStateWrapper, Triforce::quitBtns, xpos, ypos);
	ypos -= yOffset;
	menuButtons->add(this, GamePlay::play, changeStateWrapper, Triforce::playBtns, xpos, ypos);
	menuButtons->add(this, GamePlay::pause, changeStateWrapper, Triforce::pauseBtns, xpos, ypos);
	menuButtons->disable(GamePlay::play);

	Input::addMousePassiveMotionFunc(menuButtons, Triforce::PLAY,
								 	 menuButtons->mousePassiveMotion);
	Input::addMouseMotionFunc(menuButtons, Triforce::PLAY, // same as passive
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
			g->changeState(pause);
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
	if (state == GamePlay::quit)
		Triforce::setState(Triforce::QUIT);
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
	string cwd;

	cwd = themeDirectory;
	background.load( cwd + bgFile );
	background.loadGLTextures();

	// r,g,b is background color to be filtered, frameCount and frame number
	int r=254, g=0, b=254, frameCount=3, frame=0;

	// load grid border
	gridBorderSprite = new CBaseSprite(frameCount, screen_w, screen_h);
	gridBorderSprite->loadFrame(frame, themeDirectory + gridBorderFile, r, g, b);
	gridBorderSprite->loadGLTextures();
  
	char num[8];
	cwd = themeDirectory + "blocks\\";
	for (int i=0; i<nblocktypes; ++i) {
		// Block files are designed to not require a color filter. They aren't
		// guaranteed to work as expected when their bg's are filtered out and a
		// background image of a different color is used.
		
		sprintf_s(num, "%d", i);
		blockSprites[i] = new CBaseSprite(frameCount, screen_w, screen_h);
		blockSprites[i]->loadFrame(frame+0, cwd + "normal\\" + num + ".bmp", r, g, b);
		blockSprites[i]->loadFrame(frame+1, cwd + "combo\\" + num + ".bmp", r, g, b);
		blockSprites[i]->loadFrame(frame+2, cwd + "inactive\\" + num + ".bmp", r, g, b);
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
  bonusSprite = new CBaseSprite(1, screen_w, screen_h);
  bonusSprite->loadFrame(0, cwd + "bonus.bmp", r, g, b);
  bonusSprite->loadGLTextures();

  r = 254;
  g = 0;
  b = 254;
  chainFontSprite = new CBaseSprite(15 - 2 + 1, screen_w, screen_h);
  for (int i = 2; i <= 15; ++i) {
	  sprintf_s(num, "x%d", i); 
	  chainFontSprite->loadFrame(i-2, cwd + num + ".bmp", r, g, b);
  }
  chainFontSprite->loadGLTextures();
  
  // load combo sprite
  /* comboFontSprite = new CBaseSprite(15 - 4 + 1, screen_w, screen_h);
  for (int i = 4; i <= 10; ++i) {
	  sprintf_s(num, "%d", i); 
	  comboFontSprite->loadFrame(i - 4, cwd + num + ".bmp", r, g, b));
  }
  comboFontSprite->loadGLTextures();*/

  GamePlay::blockLength = blockSprites[0]->GetHeight();
  GamePlay::gridHeight = GamePlay::blockLength * nrows;
  GamePlay::gridWidth = GamePlay::blockLength * ncols;
  Grid::forcedPushSpeed = blockLength / 8;
}

void GamePlay::changeState(gameState gs) {
	if (state == play && gs == pause)
	{
		menuButtons->disable(pause);
		menuButtons->enable(play);
		state = pause;
		mainTimer->pause();
	}
	else if (state == pause && gs == play)
	{
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