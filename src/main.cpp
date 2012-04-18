#include "game.h"

using namespace std;

extern void keyboard1(unsigned char key, int x, int y);
extern void reshape(int x, int y);
void display();
GameEnv *gameEnv;
CTimer *mainTimer;

void display()
{
	gameEnv->display();
}

void initGame()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Block-Game");
	glutReshapeFunc(reshape);
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
	
	glutMainLoop();
	
	return 0;
}