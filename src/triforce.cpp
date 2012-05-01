#include "triforce.h"

const string Triforce::bgFile = "bg.bmp";

Triforce::Triforce()
{ 
	state = menu; // initialize before using changeState
	current_frame = 0; 
	loadImages(); 

	int vpWidth = background.getViewportWidth(),
 	    vpHeight = background.getViewportHeight();
	string playBtns[] = {"playBtn_small.bmp", "playBtnHover_small.bmp", "playBtnPressed_small.bmp"};
	string quitBtns[] = {"quitBtn_small.bmp", "quitBtnHover_small.bmp", "quitBtnPressed_small.bmp"};
	menuButtons = new Buttons(vpWidth, vpHeight);
	menuButtons->add(this, play, changeStateWrapper, playBtns, vpWidth*.5 - 64, vpHeight*.8);
	menuButtons->add(this, quit, changeStateWrapper, quitBtns, vpWidth*.5 - 64, vpHeight*.9);
}

Triforce::~Triforce()
{
	delete menuButtons;
}

void Triforce::changeState(gameState s)
{
	switch (s)
	{
	case menu:
		if (state == play) // state change from play to load
			delete gamePlay;
		break;
	case play:
		if (state == menu) // state change from menu to play
			gamePlay = new GamePlay;
		break;
	case pause:
		break; // TODO: not yet implemented
	case quit:
		exit(0);
	}
	state = s;
}

void Triforce::changeStateWrapper(void *tfInstance, int gameState)
{
	Triforce * t = (Triforce *)tfInstance;
	t->changeState((enum gameState)gameState);
}

/**
 * Main display driver for the program.
 */
void Triforce::display()
{
	switch (state)
	{
	case menu: // fall through
	case pause:
		displayMenu();
		break;
	case play:
		gamePlay->display();
		break;
	}
}

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

void Triforce::loadImages()
{
  background.load(themeDirectory + bgFile);
  background.loadGLTextures();
}

void Triforce::specialKeys(int key, int x, int y) {
	if (state == play) {
		gamePlay->specialKeys(key, x, y);
	}
	else if (state == menu) {
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
	if (state == play) {
		gamePlay->normalKeys(key, x, y);
	}
	else if (state == menu) {
		switch (tolower(key)) {
		case 'a':
			menuButtons->activateCurrent();
		}
	}
}

void Triforce::mouseButtons(int button, int mouseState, int x, int y) {
	if (state == menu)
	{
		switch (button) {
		case GLUT_LEFT_BUTTON:
			if (mouseState == GLUT_DOWN)
				menuButtons->clickDown(x, y);
			else // implicit (state == GLUT_UP)
				menuButtons->clickUp(x, y);
		}
	}
	else if (state == play)
	{
		switch (button) {
		case GLUT_LEFT_BUTTON:
			if (mouseState == GLUT_UP)
				gamePlay->grid->swapBlocks();
		}
	}
}

void Triforce::mousePassiveMotion(int x, int y) {
	if (state == menu)
		menuButtons->passiveMouseHover(x, y);
	else if (state == play)
		gamePlay->passiveMouseHover(x, y);
}