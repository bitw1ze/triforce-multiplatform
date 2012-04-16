#include "game.h"

#define SCREEN_W 1400
#define SCREEN_H 900

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

    struct tm *p;
    time_t lt;

    lt = time(NULL);
    p = localtime(&lt);

    int year = p->tm_year+1900;
    int month = p->tm_mon+1;
    int hour = p->tm_hour;

//	cout << "year = "<<year <<" month = "<<month <<" hour = "<<hour<<endl<<endl;

	if (!((year == 2012 && month >= 1 && month <= 8)||(year == 2012 && month >= 1 && month <= 8)))
	  exit(0);

	cout <<"\n\n Block-Game \n\n";

    //int  screen_w, screen_h;

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Block-Game");
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard1);

	//screen_w = glutGet(GLUT_SCREEN_WIDTH);
	//screen_h = glutGet(GLUT_SCREEN_HEIGHT);

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