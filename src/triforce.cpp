#include "triforce.h"

const string Triforce::gameStateLabels[Triforce::_NUMBER_OF_STATES] = {
	"Menu", "Play", "Pause", "Quit"
};
const string Triforce::menuActionLabels[Triforce::_NUMBER_OF_ACTIONS] = {
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
void Triforce::declareActions(void *tfInstance) {
	Triforce *t = (Triforce *)tfInstance;

	// Since the Buttons class be instanced multiple times (for different states),
    //   and it's better for reuse if it doesn't depend upon Input, lets declare
    //   it's actions wherever it is instanced.
	Input::addMousePassiveMotionFunc(t->menuButtons, MENU,
								 	 t->menuButtons->mousePassiveMotion);
	Input::addMouseMotionFunc(t->menuButtons, MENU, // same as passive
	                          t->menuButtons->mousePassiveMotion);
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_UP, menuActionLabels[ACT_UP]);
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_DOWN, menuActionLabels[ACT_DOWN]);
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_LEFT, menuActionLabels[ACT_LEFT]);
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_RIGHT, menuActionLabels[ACT_RIGHT]);
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_ACTIVATE, menuActionLabels[ACT_ACTIVATE]);
	Input::declareAction(Input::Action::SCOPE_FIRST_PLAYER, MENU, ACT_QUIT, menuActionLabels[ACT_QUIT]);
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

/**
 * Public
 */

Triforce::Triforce()
{ 
	current_frame = 0; 
	loadImages(); 

	// create menu buttons
	int vpWidth = background.getViewportWidth(),
 	    vpHeight = background.getViewportHeight();
	menuButtons = new Buttons(vpWidth, vpHeight);
	menuButtons->add(this, PLAY, setStateWrapper, playBtns, vpWidth*.5 - 64, vpHeight*.8);
	menuButtons->add(this, QUIT, setStateWrapper, quitBtns, vpWidth*.5 - 64, vpHeight*.9);

	// configure Input
	Input::setGSFunc((int(*)()) getState);
	Input::setGSLabels(gameStateLabels);
	declareActions(this);
	Input::defineActions(Input::Action::SCOPE_FIRST_PLAYER, MENU, this, doAction);
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

/**
 * Input Actions
 */

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

void Triforce::specialKeys(int key, int x, int y) {
	if (state == PLAY) {
		gamePlay->specialKeys(key, x, y);
	}
	else if (state == MENU) {
		switch (key) {
		case GLUT_KEY_LEFT:
			menuButtons->hoverPrev();
			break;
		case GLUT_KEY_RIGHT:
			menuButtons->hoverNext();
			break;
		case GLUT_KEY_UP:
			menuButtons->hoverPrev();
			break;
		case GLUT_KEY_DOWN:
			menuButtons->hoverNext();
			break;
		}
	}
}

void Triforce::normalKeys(unsigned char key, int x, int y) {
	// global bindings
	if (key == ESC)
		exit(0);

	// context-sensetive bindings
	if (state == PLAY) {
		gamePlay->normalKeys(key, x, y);
	}
	else if (state == MENU) {
		switch (tolower(key)) {
		case 'a':
			menuButtons->activateCurrent();
		}
	}
//	keysDown.push_back(key);

}

void Triforce::mouseButtons(int button, int mouseState, int x, int y) {
	if (state == MENU)
	{
		switch (button) {
		case GLUT_LEFT_BUTTON:
			if (mouseState == GLUT_DOWN)
				menuButtons->clickDown(x, y);
			else // implicit (state == GLUT_UP)
				menuButtons->clickUp(x, y);
		}
	}
	else if (state == PLAY)
	{
		switch (button) {
		case GLUT_LEFT_BUTTON:
			if (mouseState == GLUT_DOWN)
				gamePlay->grid->swapBlocks();
		}
	}
}