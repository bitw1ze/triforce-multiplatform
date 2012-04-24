#include "triforce.h"

using namespace std;

// function declarations
extern void reshape(int, int);
void reshape2(int w, int h);
void specialKeys(int key, int x, int y);
void normalKeys(unsigned char key, int x, int y);
void display();
void initGlut();

// globals
Triforce *triforce;
CTimer *mainTimer;

void reshape2(int w, int h) {
	if (w != screen_w && h != screen_h) {
		glutReshapeWindow(screen_w, screen_h);
	}
	reshape(w, h);
}

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

void mouseButtons(int button, int state, int x, int y) {
	triforce->mouseButtons(button, state, x, y);
}

void mousePassiveMotion(int x, int y) {
	triforce->mousePassiveMotion(x, y);
}

void initGlut()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("<^> TRIFORCE <^>");
	glutReshapeFunc(reshape2);
	glutMouseFunc(mouseButtons);
	glutPassiveMotionFunc(mousePassiveMotion);
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
		
	initGlut();
	triforce = new Triforce();
	glutMainLoop(); // never returns!
}