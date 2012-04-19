#include "triforce.h"

const string Triforce::bgFile = "bg.bmp";

Triforce::Triforce()
{ 
	state = load;
	changeState(play); // FIXME: this is temporily here until the load screen is ready
	current_frame = 0; 
	loadImages(); 
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
		// not yet implemented
		break;
	}
	state = s;
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

void Triforce::display()
{
	switch (state)
	{
	case load:
		composeFrame();
		background.drawGLbackground ();
		glutSwapBuffers();
	case play:
		gameEnv->display();
	case pause:
		;
	}
}

void Triforce::loadImages()
{
  background.load(themeDirectory + bgFile);
  background.loadGLTextures();

  // r,g,b is background color to be filtered, frameCount and frame number
  //  int r=254, g=0, b=254, frameCount=1, frame=0;
}