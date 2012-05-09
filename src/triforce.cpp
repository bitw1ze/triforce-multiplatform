#include "triforce.h"

const string Triforce::gameStateLabels[Triforce::_NUMBER_OF_STATES] = {
	"Menu", "Play", "Pause", "Quit"
};
const string Triforce::actionLabels[Triforce::_NUMBER_OF_ACTIONS] = {
	"Up", "Down", "Left", "Right", "Activate", "Quit"
};

const string Triforce::bgFile = "bg-menu.bmp";
const string Triforce::playBtns[] = {
	"playBtn_small.bmp", "playBtnHover_small.bmp", "playBtnPressed_small.bmp"
};
const string Triforce::quitBtns[] = {
	"quitBtn_small.bmp", "quitBtnHover_small.bmp", "quitBtnPressed_small.bmp"
};

Triforce::GameState Triforce::state = MENU;
GamePlay * Triforce::gamePlay = NULL;

/**
 * Input actions routines
 */
void Triforce::declareActions()
{
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_UP, actionLabels[ACT_UP]);
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_DOWN, actionLabels[ACT_DOWN]);
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_LEFT, actionLabels[ACT_LEFT]);
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_RIGHT, actionLabels[ACT_RIGHT]);
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_ACTIVATE, actionLabels[ACT_ACTIVATE]);
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_QUIT, actionLabels[ACT_QUIT]);

	Grid::declareActions();
	Cursor::declareActions();
}

// Hard coded default bindings
void Triforce::bindDefaultActionKeys()
{
	int player = 0;
	Input::Action::ActionScope scope;

	/*
	 * MENU state
	 */
	scope = Input::Action::SCOPE_FIRST_PLAYER;

	Input::bindKey(player, scope, MENU, ACT_ACTIVATE, SPACE);
	Input::bindKey(player, scope, MENU, ACT_ACTIVATE, ENTER);
	Input::bindKey(player, scope, MENU, ACT_ACTIVATE, 'a');
	Input::bindKey(player, scope, MENU, ACT_ACTIVATE, 'e');

	Input::bindKey(player, scope, MENU, ACT_QUIT, ESC);

	Input::bindSpecialKey(player, scope, MENU, ACT_UP, GLUT_KEY_UP);
	Input::bindSpecialKey(player, scope, MENU, ACT_DOWN, GLUT_KEY_DOWN);
	Input::bindSpecialKey(player, scope, MENU, ACT_LEFT, GLUT_KEY_LEFT);
	Input::bindSpecialKey(player, scope, MENU, ACT_RIGHT, GLUT_KEY_RIGHT);

	/*
	 * PLAY state
	 */
	scope = Input::Action::SCOPE_FIRST_PLAYER;
	// FIXME: I have a hunch that SCOPE_CURRENT_PLAYER stuff is broken, so
    //	      comment it out and use SCOPE_FIRST_PLAYER for now
	//scope = Input::Action::SCOPE_CURRENT_PLAYER;

	// arrow keys
	Input::bindSpecialKey(player, scope, PLAY, Cursor::ACT_UP, GLUT_KEY_UP);
	Input::bindSpecialKey(player, scope, PLAY, Cursor::ACT_DOWN, GLUT_KEY_DOWN);
	Input::bindSpecialKey(player, scope, PLAY, Cursor::ACT_LEFT, GLUT_KEY_LEFT);
	Input::bindSpecialKey(player, scope, PLAY, Cursor::ACT_RIGHT, GLUT_KEY_RIGHT);

	// w-a-s-d style + play one-handed
	Input::bindKey(player, scope, PLAY, Grid::ACT_PUSH, 'q');
	Input::bindKey(player, scope, PLAY, Grid::ACT_SWAP, 'e');

	Input::bindKey(player, scope, PLAY, Cursor::ACT_UP, 'w');
	Input::bindKey(player, scope, PLAY, Cursor::ACT_DOWN, 's');
	Input::bindKey(player, scope, PLAY, Cursor::ACT_LEFT, 'a');
	Input::bindKey(player, scope, PLAY, Cursor::ACT_RIGHT, 'd');

	// other
	Input::bindKey(player, scope, PLAY, Grid::ACT_PUSH, ENTER);
	Input::bindKey(player, scope, PLAY, Grid::ACT_SWAP, SPACE);
	Input::bindKey(player, scope, PLAY, Grid::ACT_PUSH, 'z');
	Input::bindKey(player, scope, PLAY, Grid::ACT_SWAP, 'x');
}

/**
 * Display routines
 */
void Triforce::displayMenu()
{
	composeFrame();
	background.drawGLbackground();
	menuButtons->display();
	glutSwapBuffers();
}

void Triforce::composeFrame()
{
	/*
	if(mainTimer->elapsed(last_time, 300))
	{
		processFrame();
		last_time=mainTimer->time();
	}
	*/
	glutPostRedisplay();
}

void Triforce::processFrame()
{
}

void Triforce::loadImages()
{
  background.load(themeDirectory + bgFile);
  background.loadGLTextures();
}

void Triforce::doAction(void *tfInstance, int actionState, int actionType) {
	Triforce *t = (Triforce *)tfInstance;

	switch ((enum Input::Action::ActionState)actionState)
	{
	case Input::Action::STATE_RELEASE:
		switch((enum Actions)actionType)
		{
		case ACT_LEFT:
		case ACT_UP:
			t->menuButtons->hoverPrev();
			break;
		case ACT_RIGHT:
		case ACT_DOWN:
			t->menuButtons->hoverNext();
			break;
		case ACT_ACTIVATE:
			t->menuButtons->activateCurrent();
			break;
		case ACT_QUIT:
			exit(0);
			break;
		}
		break;
	case Input::Action::STATE_PRESS:
	case Input::Action::STATE_HOLD:
		switch((enum Actions)actionType)
		{
		case ACT_ACTIVATE:
			t->menuButtons->pressCurrent();
			break;
		}
		break;
	}
}

/**
 * Public
 */

Triforce::Triforce()
{ 
	current_frame = 0; 
	loadImages(); 

	/*
	 * Create menu buttons
	 */
	int vpWidth = background.getViewportWidth(),
 	    vpHeight = background.getViewportHeight();
	menuButtons = new Buttons(vpWidth, vpHeight);
	menuButtons->add(this, PLAY, setStateWrapper, playBtns, vpWidth*.5 - 64, vpHeight*.8);
	menuButtons->add(this, QUIT, setStateWrapper, quitBtns, vpWidth*.5 - 64, vpHeight*.9);

	/*
	 * Configure input
	 */

	Input::setGSFunc((int(*)()) getState);
	Input::setGSLabels(gameStateLabels);

	// Since the Buttons class be instanced multiple times (for different states),
    //   and it's better for reuse if it doesn't depend upon Input, lets declare
    //   it's actions wherever it is instanced.
	Input::addMousePassiveMotionFunc(menuButtons, MENU,
								 	 menuButtons->mousePassiveMotion);
	Input::addMouseMotionFunc(menuButtons, MENU, // same as passive
	                          menuButtons->mousePassiveMotion);
	Input::addPlayer();
	declareActions();
	Input::defineActions(Input::Action::SCOPE_FIRST_PLAYER, MENU, this, doAction);
	bindDefaultActionKeys();
}

Triforce::~Triforce()
{
	Input::removeMotions(menuButtons);
	delete menuButtons;
}


// Main display driver for the program.
void Triforce::display()
{
	switch (state)
	{
	case MENU: // fall through
	case PAUSE:
		displayMenu();
		break;
	case PLAY:
		gamePlay->display();
		break;
	}
}

/**
 * State
 */

void Triforce::setState(GameState s)
{
	switch (s)
	{
	case MENU:
		if (state == PLAY) // state change from play to load
			delete gamePlay;
		break;
	case PLAY:
		if (state == MENU) // state change from menu to play
			gamePlay = new GamePlay;
		break;
	case PAUSE:
		break; // TODO: not yet implemented
	case QUIT:
		exit(0);
	}
	state = s;
}

void Triforce::setStateWrapper(void *tfInstance, int gameState)
{
	Triforce * t = (Triforce *)tfInstance;
	t->setState((enum GameState)gameState);
}

void Triforce::specialKeys(int key, int x, int y) {
	if (state == PLAY) {
		gamePlay->specialKeys(key, x, y);
	}
}

void Triforce::normalKeys(unsigned char key, int x, int y) {
	// context-sensetive bindings
	if (state == PLAY) {
		if (key == ESC)
			exit(0);
		gamePlay->normalKeys(key, x, y);
	}

}

void Triforce::mouseButtons(int button, int mouseState, int x, int y) {
	if (state == PLAY)
	{
		switch (button) {
		case GLUT_LEFT_BUTTON:
			if (mouseState == GLUT_DOWN)
				gamePlay->grid->swapBlocks();
		}
	}
}