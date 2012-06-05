#include "triforce.h"
#include "Xbox.h"

const string Triforce::gameStateLabels[Triforce::_NUMBER_OF_STATES] = {
	"Menu", "Play", "Pause", "Quit"
};

const string Triforce::bgFile = "bg-menu3.bmp";
const string Triforce::playBtns[] = {
	"playBtn.bmp", "playBtnHover.bmp", "playBtnPressed.bmp"
};
const string Triforce::quitBtns[] = {
	"quitBtn.bmp", "quitBtnHover.bmp", "quitBtnPressed.bmp"
};
const string Triforce::backBtns[] = {
	"backBtn.bmp", "backBtnHover.bmp", "backBtnPressed.bmp"
};
const string Triforce::helpBtns[] = {
	"helpBtn.bmp", "helpBtnHover.bmp", "helpBtnPressed.bmp"
};
const string Triforce::pauseBtns[] = {
	"pauseBtn.bmp", "pauseBtnHover.bmp", "pauseBtnPressed.bmp"
};

Triforce::GameState Triforce::state = MENU;
GamePlay * Triforce::gamePlay = NULL;

/**
 * Input actions routines
 */
void Triforce::declareActions()
{
	using namespace MenuState;
	using namespace Input;
	declareAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::UP, actionLabels[MenuState::UP]);
	declareAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::DOWN, actionLabels[MenuState::DOWN]);
	declareAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::LEFT, actionLabels[MenuState::LEFT]);
	declareAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::RIGHT, actionLabels[MenuState::RIGHT]);
	declareAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::ACTIVATE, actionLabels[MenuState::ACTIVATE]);
	declareAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::QUIT, actionLabels[MenuState::QUIT]);

	GamePlay::declareActions();
	Grid::declareActions();
	Cursor::declareActions();
}

void Triforce::defineActions()
{
	using namespace Input;
	defineAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::UP, this, doAction);
	defineAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::DOWN, this, doAction);
	defineAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::LEFT, this, doAction);
	defineAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::RIGHT, this, doAction);
	defineAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::ACTIVATE, this, doAction);
	defineAction(Action::SCOPE_FIRST_PLAYER, MENU, MenuState::QUIT, this, doAction);
}

// Hard coded default bindings
void Triforce::bindDefaultActionKeys()
{
	using namespace Input;
	int player = 0;
	Action::ActionScope scope;

	/*
	 * MENU state
	 */
	scope = Action::SCOPE_FIRST_PLAYER;

	bindKey(player, scope, MENU, MenuState::ACTIVATE, BTN_SPACE);
	bindKey(player, scope, MENU, MenuState::ACTIVATE, BTN_ENTER);
	bindKey(player, scope, MENU, MenuState::ACTIVATE, 'a');
	bindKey(player, scope, MENU, MenuState::ACTIVATE, 'e');
	bindKey(player, scope, MENU, MenuState::ACTIVATE, 'z');

	bindKey(player, scope, MENU, MenuState::QUIT, BTN_ESC);

	bindSpecialKey(player, scope, MENU, MenuState::UP, GLUT_KEY_UP);
	bindSpecialKey(player, scope, MENU, MenuState::DOWN, GLUT_KEY_DOWN);
	bindSpecialKey(player, scope, MENU, MenuState::LEFT, GLUT_KEY_LEFT);
	bindSpecialKey(player, scope, MENU, MenuState::RIGHT, GLUT_KEY_RIGHT);

	// xbox bindings
	bindXboxButton(player, scope, MENU, MenuState::UP, XINPUT_GAMEPAD_DPAD_UP);
	bindXboxButton(player, scope, MENU, MenuState::DOWN, XINPUT_GAMEPAD_DPAD_DOWN);
	bindXboxButton(player, scope, MENU, MenuState::LEFT, XINPUT_GAMEPAD_DPAD_LEFT);
	bindXboxButton(player, scope, MENU, MenuState::RIGHT, XINPUT_GAMEPAD_DPAD_RIGHT);
	bindXboxButton(player, scope, MENU, MenuState::ACTIVATE, XINPUT_GAMEPAD_A | XINPUT_GAMEPAD_START);
	bindXboxButton(player, scope, MENU, MenuState::QUIT, XINPUT_GAMEPAD_BACK | XINPUT_GAMEPAD_B);

	/*
	 * PLAY state
	 */
	// FIXME: this stuff shouldn't be SCOPE_FIRST_PLAYER; this is temporary
	scope = Action::SCOPE_FIRST_PLAYER;

	// arrow keys
	bindSpecialKey(player, scope, PLAY, PlayState::UP, GLUT_KEY_UP);
	bindSpecialKey(player, scope, PLAY, PlayState::DOWN, GLUT_KEY_DOWN);
	bindSpecialKey(player, scope, PLAY, PlayState::LEFT, GLUT_KEY_LEFT);
	bindSpecialKey(player, scope, PLAY, PlayState::RIGHT, GLUT_KEY_RIGHT);

	// w-a-s-d style + play one-handed
	bindKey(player, scope, PLAY, PlayState::PUSH, 'q');
	bindKey(player, scope, PLAY, PlayState::SWAP, 'e');

	bindKey(player, scope, PLAY, PlayState::UP, 'w');
	bindKey(player, scope, PLAY, PlayState::DOWN, 's');
	bindKey(player, scope, PLAY, PlayState::LEFT, 'a');
	bindKey(player, scope, PLAY, PlayState::RIGHT, 'd');

	// other
	bindKey(player, scope, PLAY, PlayState::PUSH, BTN_ENTER);
	bindKey(player, scope, PLAY, PlayState::SWAP, BTN_SPACE);
	bindKey(player, scope, PLAY, PlayState::PUSH, 'z');
	bindKey(player, scope, PLAY, PlayState::SWAP, 'x');
	bindKey(player, scope, PLAY, PlayState::PAUSE_TOGGLE, 't');

	// xbox bindings
	bindXboxButton(player, scope, PLAY, PlayState::UP, XINPUT_GAMEPAD_DPAD_UP);
	bindXboxButton(player, scope, PLAY, PlayState::DOWN, XINPUT_GAMEPAD_DPAD_DOWN);
	bindXboxButton(player, scope, PLAY, PlayState::LEFT, XINPUT_GAMEPAD_DPAD_LEFT);
	bindXboxButton(player, scope, PLAY, PlayState::RIGHT, XINPUT_GAMEPAD_DPAD_RIGHT);
	bindXboxButton(player, scope, PLAY, PlayState::SWAP, XINPUT_GAMEPAD_A | XINPUT_GAMEPAD_B);
	bindXboxButton(player, scope, PLAY, PlayState::PUSH, XINPUT_GAMEPAD_LEFT_SHOULDER | XINPUT_GAMEPAD_RIGHT_SHOULDER);
	bindXboxButton(player, scope, PLAY, PlayState::PAUSE_TOGGLE, XINPUT_GAMEPAD_START);
	bindXboxButton(player, scope, PAUSE, PlayState::PAUSE_TOGGLE, XINPUT_GAMEPAD_START);
	bindXboxButton(player, scope, PLAY, PlayState::RETURN, XINPUT_GAMEPAD_BACK);
}

/**
 * Display routines
 */
void Triforce::displayMenu()
{
	glutPostRedisplay();
	background.drawGLbackground();
	menuButtons->display();
	glutSwapBuffers();
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
	case Input::Action::STATE_PRESS:
		switch((enum Actions)actionType)
		{
			case MenuState::ACTIVATE:
				t->menuButtons->pressCurrent();
				break;
		}
		break;
	case Input::Action::STATE_RELEASE:
		switch((enum Actions)actionType)
		{
			case MenuState::LEFT:
			case MenuState::UP:
				t->menuButtons->hoverPrev();
				break;
			case MenuState::RIGHT:
			case MenuState::DOWN:
				t->menuButtons->hoverNext();
				break;
			case MenuState::ACTIVATE:
				t->menuButtons->activateCurrent();
				break;
			case MenuState::QUIT:
				exit(0);
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
	loadImages(); 

	/*
	 * Create menu buttons
	 */
	int vpWidth = background.getViewportWidth(),
 	    vpHeight = background.getViewportHeight(),
		midHeight = vpHeight*.75;
	menuButtons = new Buttons(vpWidth, vpHeight);
	menuButtons->add(this, PLAY, setStateWrapper, playBtns, vpWidth*.5 - 64, midHeight - vpHeight*.1);
	menuButtons->add(this, HELP, setStateWrapper, helpBtns, vpWidth*.5 - 64, midHeight);
	menuButtons->add(this, QUIT, setStateWrapper, quitBtns, vpWidth*.5 - 64, midHeight + vpHeight*.1);

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
	defineActions();
	bindDefaultActionKeys();
}

Triforce::~Triforce()
{
	Input::undefineActions(this);
	Input::removeMouseMotions(menuButtons);
	delete menuButtons;
}


// Main display driver for the program.
void Triforce::display()
{
	switch (state)
	{
	case MENU: // fall through
		if (gamePlay)
		{
			delete gamePlay;
			gamePlay = NULL;
		}
	case HELP:
		displayMenu();
		break;
	case PLAY:
	case PAUSE:
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
		state = MENU;
		break;
	case PLAY:
		if (state == MENU) // state change from menu to play
			gamePlay = new GamePlay;
		state = PLAY;
		break;
	case HELP:
		s = state; //FIXME: temporarily ignore this state
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

void Triforce::mouseButtons(int button, int mouseState, int x, int y) {
	switch (state)
	{
	case PAUSE:
		if (mouseState == GLUT_DOWN)
			gamePlay->menuButtons->clickDown(x, y);
		else // implicit GLUT_UP
			gamePlay->menuButtons->clickUp(x, y);
		break;
	case PLAY:
		switch (button) {
		case GLUT_LEFT_BUTTON:
			if (mouseState == GLUT_DOWN)
			{
				gamePlay->menuButtons->clickDown(x, y);
				if (gamePlay->grid->containsPoint(x, y))
					gamePlay->grid->swapBlocks();
			}
			else // implicit GLUT_UP
				gamePlay->menuButtons->clickUp(x, y);
			break;
		}
		break;
	case MENU:
		switch (button) {
		case GLUT_LEFT_BUTTON:
			if (mouseState == GLUT_DOWN)
				menuButtons->clickDown(x, y);
			else
				menuButtons->clickUp(x, y);
			break;
		}
		break;
	}
}