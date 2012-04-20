#include "triforce.h"

const string Triforce::bgFile = "bg.bmp";

Triforce::Triforce()
{ 
	state = load; // initialize before using changeState
	//changeState(play); // temp: this is temporily here until the load screen is ready
	current_frame = 0; 
	loadImages(); 
	menuButtons = new Buttons(background.getViewportWidth(), background.getViewportHeight());
	menuButtons->add("block-blue.bmp", 100, 100); // tmp, for testing buttons
	menuButtons->add("block-special.bmp", 350, 350); // tmp, for testing buttons
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
			delete gameEnv;
		break;
	case play:
		
		if (state == load) // state change from load to play
			gameEnv = new GameEnv;
		break;
	case pause:
		break; // not yet implemented
	}
	state = s;
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
		gameEnv->display();
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