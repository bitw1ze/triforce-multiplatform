#include "game.h"

using namespace std;

void display();
GameEnv *gameEnv;

void display()
{
   gameEnv->ComposeFrame();
   gameEnv->background.drawGLbackground ();

   for (int i=0; i<NROWS; ++i) 
	   for (int j=0; j<NCOLS; ++j) 
		   gameEnv->blocks[i][j].draw(0);

   glutSwapBuffers();
}

extern void init();

int main(int argc,char** argv)
{
	glutInit(&argc, argv);
		
	init();
	gameEnv = new GameEnv();
    glutDisplayFunc(display);
	
	glutMainLoop();
	

	return 0;
}