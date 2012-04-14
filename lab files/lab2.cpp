
/* Gabe Pike
   CMPS 335
   Lab 02
   13 April 2012 */

#include "game.h"

void GameEnv::CreateObjects()
{
  int x=64, y=background.getViewportHeight()-64, xspeed=0, yspeed=1;
  //crow.create(x, y, xspeed, yspeed, crowsprite, Timer);
  for (int i=0; i<NROWS; ++i) {
	  for (int j=0; j<NCOLS; ++j) {
		  blocks[i][j].create(x+j*blockSprites[i][j]->GetWidth(), y-i*blockSprites[i][j]->GetHeight(), xspeed, yspeed, blockSprites[i][j], Timer);
	  }
  }
} 

bool GameEnv::ProcessFrame()
{
	for (int i=0; i<NROWS; ++i)
		for (int j=0; j<NCOLS; ++j)
			blocks[i][j].move();
	return true;
}

bool GameEnv::LoadImages()
{
  background.load("bg.bmp");
  background.loadGLTextures();

  int r=254, g=0, b=254, frameCount=1, frame=0;    // r,g,b is background color to be filtered, frameCount and frame number
  
  for (int i=0; i<NROWS; ++i) {
	  for (int j=0; j<NCOLS; ++j) {
		  blockSprites[i][j] = new CBaseSprite(frameCount, background.getViewportWidth(), background.getViewportHeight());
		  blockSprites[i][j]->loadFrame(frame, blockFiles[rand() % NBLOCKS], r, g, b);
		  blockSprites[i][j]->loadGLTextures();
	  }
  }

  return true;
}

void GameEnv::ComposeFrame()
{
  if(Timer->elapsed(last_time,300))
  {
    last_time=Timer->time();
    if(++current_frame>=1)current_frame=0;
  }
  ProcessFrame();

  glutPostRedisplay();
}


void display()
{
   gameEnv->ComposeFrame();
   gameEnv->background.drawGLbackground ();

   for (int i=0; i<NROWS; ++i) 
	   for (int j=0; j<NCOLS; ++j) 
		   gameEnv->blocks[i][j].draw(0);

   glutSwapBuffers();
}

GameEnv::GameEnv(int *argc, char **argv) { 
	current_frame = 0; 
	Timer = new CTimer(); 
	Timer->start();
	last_time=Timer->time();
	LoadImages(); 
	CreateObjects();

	srand(time(NULL));
	
}

extern void init();

int main(int argc,char** argv)
{
	glutInit(&argc, argv);

	init();
	gameEnv = new GameEnv(&argc, argv);
    glutDisplayFunc(display);

	glutMainLoop();
	

	return 0;
}

