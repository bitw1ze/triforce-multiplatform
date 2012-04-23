#include "triforce.h"

const string Triforce::bgFile = "bg.bmp";

Triforce::Triforce()
{ 
	state = load; // initialize before using changeState
	changeState(play); // this is temporily here until the load screen is ready
	current_frame = 0; 
	loadImages(); 

	string playBtns[] = {"playBtn_small.bmp", "playBtnHover_small.bmp", "playBtnPressed_small.bmp"};
	string quitBtns[] = {"quitBtn_small.bmp", "quitBtnHover_small.bmp", "quitBtnPressed_small.bmp"};
	int vpWidth = background.getViewportWidth(),
 	    vpHeight = background.getViewportHeight();
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
	case load:
		if (state == play) // state change from play to load
			delete gamePlay;
		state = menu;
		/* fall through */
	case menu:
		break; // nothing to do
	case play:
		
		if (state == load) // state change from load to play
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
	case load:
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
	if(mainTimer->elapsed(last_time, 300))
	{
		//processFrame();
		last_time=mainTimer->time();
	}
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
			menuButtons->hoverNext();
			break;
		case GLUT_KEY_RIGHT:
			menuButtons->hoverNext();
			break;
		case GLUT_KEY_UP:
			menuButtons->hoverNext();
			break;
		case GLUT_KEY_DOWN:
			menuButtons->hoverNext();
			break;
		}
	}
}

void Triforce::normalKeys(unsigned char key, int x, int y) {
	if (state == play) {
		gamePlay->normalKeys(key, x, y);
	}
	else if (state == menu) {
		switch (toupper(key)) {
		case 'A':
			menuButtons->pressActive();
		}
	}
}