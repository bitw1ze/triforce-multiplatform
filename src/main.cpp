#include "game.h"

#define SCREEN_W 1024
#define SCREEN_H 768

using namespace std;

extern void keyboard1(unsigned char key, int x, int y);
extern void reshape(int x, int y);
void display();
GameEnv *gameEnv;

void display()
{
	gameEnv->display();
}

void initGame()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	
	glutCreateWindow("Block-Game");
	glutKeyboardFunc(keyboard1);
	glutReshapeFunc(reshape);
	glutReshapeWindow(SCREEN_W, SCREEN_H);
    glutSwapBuffers();
}

int main(int argc,char** argv)
{
	glutInit(&argc, argv);
		
	initGame();
	gameEnv = new GameEnv();
    glutDisplayFunc(display);
	
	glutMainLoop();
	

	return 0;
}