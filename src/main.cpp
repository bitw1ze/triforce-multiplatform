#include "triforce.h"

using namespace std;

#define ESC 27
#define SPACE 32

// function declarations
extern void reshape(int x, int y);
//void specialKeys(int key, int x, int y);
//void normalKeys(unsigned char key, int x, int y);
void display();
void initGlut();

// globals
Triforce *triforce;
CTimer *mainTimer;

void display()
{
	triforce->display();
}

// Temporarily disable controls while Triforce class is being written
void specialKeys(int key, int x, int y) {
	switch(key) {
	case GLUT_KEY_LEFT:
		triforce->gameEnv->grid->cursor->moveLeft(); //FIXME: this really begs for refactoring
		break;

	case GLUT_KEY_RIGHT:
		triforce->gameEnv->grid->cursor->moveRight();
		break;

	case GLUT_KEY_UP:
		triforce->gameEnv->grid->cursor->moveUp();
		break;

	case GLUT_KEY_DOWN:
		triforce->gameEnv->grid->cursor->moveDown();
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
		triforce->gameEnv->grid->swapBlocks();
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
	triforce = new Triforce();
	glutDisplayFunc(display);
	glutMainLoop();

	delete triforce;
	return 0;
}