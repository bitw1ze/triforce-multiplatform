#include "game.h"

using namespace std;

extern void reshape(int, int);
extern void keyboard1(unsigned char, int, int);
void onKeyDown(int, int, int);
void display();
GameEnv *gameEnv;
CTimer *mainTimer;
CTimer *keyTimer;
int last_arrowkeypress, arrowkey_delay = 50;


void display()
{
	gameEnv->display();
}

void onKeyDown(int key, int x, int y) {
	cout << last_arrowkeypress << endl;
	if (!keyTimer->elapsed(last_arrowkeypress, arrowkey_delay))
		return;
	else
		last_arrowkeypress = keyTimer->time();
	
	switch(key) {
	case GLUT_KEY_LEFT:
		gameEnv->cursor->moveLeft();
		cout << "left\n";
		break;

	case GLUT_KEY_RIGHT:
		gameEnv->cursor->moveRight();
		break;

	case GLUT_KEY_UP:
		gameEnv->cursor->moveUp();
		break;

	case GLUT_KEY_DOWN:
		gameEnv->cursor->moveDown();
		break;
	}

	glutPostRedisplay();
}

void initGame()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Block-Game");
	glutReshapeFunc(reshape);
	glutSpecialFunc(onKeyDown);
	glutKeyboardFunc(keyboard1);
	glutReshapeWindow(screen_w, screen_h);
    glutSwapBuffers();
}

int main(int argc,char** argv)
{
	glutInit(&argc, argv);
		
	initGame();
	gameEnv = new GameEnv();
    glutDisplayFunc(display);

	keyTimer = new CTimer();
	keyTimer->start();
	last_arrowkeypress = keyTimer->time();
	
	glutMainLoop();
	
	return 0;
}