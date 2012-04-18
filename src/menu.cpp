#include "game.h"

/* Menu methods
   They are no longer inline. If there is a performance problem we can put it
   back to the way it was. Inline does come with problems. The project is
   messier with inline functions and all files that include a header with
   inline functions must be recompiled as well (not really a big deal for small
   projects, but it will make large projects more annoying to compile. */

GameEnv::Menu::Menu() { 
	current_frame = 0; 
	loadImages(); 
}

void GameEnv::Menu::composeFrame()
{
	if(Timer->elapsed(last_time,300))
	{
		//processFrame();
		last_time=Timer->time();
	}
	glutPostRedisplay();
}

void GameEnv::display() {
	if (showMenu)
		menu.display();
	else
		displayGame();
}

void GameEnv::Menu::display()
{
	composeFrame();
	background.drawGLbackground ();

	glutSwapBuffers();
}

void GameEnv::Menu::loadImages()
{
  background.load( themeDirectory + bgFile );
  background.loadGLTextures();

  // r,g,b is background color to be filtered, frameCount and frame number
  //  int r=254, g=0, b=254, frameCount=1, frame=0;
}