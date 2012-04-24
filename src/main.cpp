#include "triforce.h"

using namespace std;

// function declarations
extern void reshape(int x, int y);
void specialKeys(int key, int x, int y);
void normalKeys(unsigned char key, int x, int y);
void display();
void gameInit();

// globals
Triforce *triforce;
CTimer *mainTimer;

void display()
{
	triforce->display();
}

void specialKeys(int key, int x, int y) {
	triforce->specialKeys(key, x, y);
}

void normalKeys(unsigned char key, int x, int y) {
	triforce->normalKeys(key, x, y);
}

void gameInit()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("<^> TRIFORCE <^>");
	glutReshapeFunc(reshape);
	glutKeyboardFunc(normalKeys);
	glutSpecialFunc(specialKeys);
	glutReshapeWindow(screen_w, screen_h);
	glutSwapBuffers();
	mainTimer = new CTimer();
	mainTimer->start();
	glutDisplayFunc(display);
}

int main(int argc,char** argv)
{
	glutInit(&argc, argv);
		
	gameInit();
	triforce = new Triforce();
	glutMainLoop(); // never returns!
}