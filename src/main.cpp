#include "game.h"

using namespace std;

void display();
GameEnv *gameEnv;

void display()
{
   gameEnv->display();
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