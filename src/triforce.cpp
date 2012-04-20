#include "triforce.h"

const string Triforce::bgFile = "bg.bmp";

Triforce::Triforce()
{ 
	state = load; // initialize before using changeState
	changeState(play); // this is temporily here until the load screen is ready
	current_frame = 0; 
	loadImages(); 

	string playBtns[] = {"playBtn_small.bmp", "playBtnHover_small.bmp", "playBtnPressed_small.bmp"};
	int vpWidth = background.getViewportWidth(),
 	    vpHeight = background.getViewportHeight();
	menuButtons = new Buttons(vpWidth, vpHeight);
	menuButtons->add(playBtns, vpWidth*.5 - 64, vpHeight*.8);
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