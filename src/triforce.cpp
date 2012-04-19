/*
version 1:


version 2:

*/

#include "triforce.h"

const string Triforce::bgFile = "bg.bmp";
const string Triforce::btnPlayFile = "block-special.bmp";

Triforce::Triforce()
{ 
	state = load; // initialize before using changeState
	//changeState(play); // FIXME: this is temporily here until the load screen is ready
	current_frame = 0; 
	loadImages(); 
	//btnPlay.create(0, 0, 0, 0, btnPlaySprite);
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

void Triforce::display()
{
	switch (state)
	{
	case load:
		composeFrame();
		background.drawGLbackground ();
		buttons.display();
//		btnPlay.draw(0);
		glutSwapBuffers();
		break;
	case play:
		gameEnv->display();
		break;
	case pause:
		break; // not yet implemented
	}
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
  btnPlaySprite = new CBaseSprite(1, background.getViewportWidth(), background.getViewportHeight());

  // r,g,b is background color to be filtered, frameCount and frame number
  int r=254, g=0, b=254, frameCount=1, frame=0;
  btnPlaySprite->loadFrame(frame, themeDirectory + btnPlayFile, r, g, b);
  btnPlaySprite->loadGLTextures();
}