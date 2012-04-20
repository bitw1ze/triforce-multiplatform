#include "game.h"

using namespace std;

#define ESC 27
#define SPACE 32

// function declarations
extern void reshape(int x, int y);
void specialKeys(int key, int x, int y);
void normalKeys(unsigned char key, int x, int y);
void display();
void initGlut();

// globals
GameEnv *gameEnv;
CTimer *mainTimer;

void display()
{
	gameEnv->display();
}

void specialKeys(int key, int x, int y) {
	switch(key) {
	case GLUT_KEY_LEFT:
		gameEnv->grid->cursor->moveLeft();
		break;

	case GLUT_KEY_RIGHT:
		gameEnv->grid->cursor->moveRight();
		break;

	case GLUT_KEY_UP:
		gameEnv->grid->cursor->moveUp();
		break;

	case GLUT_KEY_DOWN:
		gameEnv->grid->cursor->moveDown();
		break;
	}

	glutPostRedisplay();
}

void normalKeys(unsigned char key, int x, int y) {
	switch (key) { 
	case ESC:
		exit(0);
		break;
	case 'z':
	case 'Z':
		gameEnv->grid->swapBlocks();
		break;
	}
}

void initGlut()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Block-Game");
	glutReshapeFunc(reshape);
	glutKeyboardFunc(normalKeys);
	glutSpecialFunc(specialKeys);
	glutReshapeWindow(screen_w, screen_h);
	glutSwapBuffers();
	mainTimer = new CTimer();
	mainTimer->start();
}

int main(int argc,char** argv)
{
	glutInit(&argc, argv);
		
	initGlut();
	gameEnv = new GameEnv();
	glutDisplayFunc(display);
	
	glutMainLoop();
	
	return 0;
}