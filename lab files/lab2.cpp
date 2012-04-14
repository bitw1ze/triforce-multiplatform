
/* Gabe Pike
   CMPS 335
   Lab 02
   13 April 2012 */

#include "2DGraphics.h"
#include <string>
#include <vector>
#include <ctime>

void display();
void ComposeFrame();

int current_frame=0;

int last_time;
CTimer *Timer=new CTimer();

string blockFiles[] = {"block-blue.bmp", "block-green.bmp", "block-purple.bmp", 
	"block-red.bmp", "block-special.bmp", "block-teal.bmp", "block-yellow.bmp"};
const int NBLOCKS = 7;

BMPClass background; 
//CBaseSprite *crowsprite=NULL;
const unsigned int NCOLS = 6, NROWS = 10;
CBaseSprite* blockSprites[NROWS][NCOLS];

//CObject crow;
CObject blocks[NROWS][NCOLS];

void CreateObjects()
{
  int x=64, y=background.getViewportHeight()-64, xspeed=0, yspeed=1;
  //crow.create(x, y, xspeed, yspeed, crowsprite, Timer);
  for (int i=0; i<NROWS; ++i) {
	  for (int j=0; j<NCOLS; ++j) {
		  blocks[i][j].create(x+j*blockSprites[i][j]->GetWidth(), y-i*blockSprites[i][j]->GetHeight(), xspeed, yspeed, blockSprites[i][j], Timer);
	  }
  }
} 

bool ProcessFrame()
{
	for (int i=0; i<NROWS; ++i)
		for (int j=0; j<NCOLS; ++j)
			blocks[i][j].move();
	return true;
}

bool LoadImages()
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

extern void init();


int main(int argc,char** argv)
{
	srand(time(NULL));
	glutInit(&argc,argv);

	init();
    glutDisplayFunc(display);

	Timer->start();
	last_time=Timer->time();
    LoadImages(); 
    CreateObjects(); 

	glutMainLoop();
	return 0;
}

void ComposeFrame()
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
   ComposeFrame();
   background.drawGLbackground ();

   for (int i=0; i<NROWS; ++i) 
	   for (int j=0; j<NCOLS; ++j) 
		   blocks[i][j].draw(0);

   glutSwapBuffers();
}