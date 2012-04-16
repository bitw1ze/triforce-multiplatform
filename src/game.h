#ifndef __GAME_H__
#define __GAME_H__

#include "2DGraphics.h"
#include "block.h"
#include <string>
#include <vector>
#include <ctime>
#include <deque>

#define nblocktypes 7
#define nrows 10
#define ncols 6

using namespace std;

class GameEnv {
protected:
	class Menu {
	private:
		static const string bgFile;
		BMPClass background;
	public:
		void display();
		void composeFrame();
		void processFrame();
		void loadImages();
		void init();
	};
	Menu menu;

	static const string 
		blockFiles[],
		bgFile,
		themeDirectory;

	int grid_x, grid_y, 
		row_xvel, row_yvel, 
		block_w, block_h,
		row_bottom, row_top,

		current_frame, 
		last_time,
		last_pushtime;

	bool showMenu;
	CTimer *Timer;
	BMPClass background;
	CBaseSprite* blockSprites[nblocktypes];
	deque<Block *> blocks;

	void pushRow(int row);
public:
	GameEnv();
	void display();
	void displayGame();
	void composeFrame();
	void init();
	void processFrame();
	void loadImages();
};

/* These functions have been defined as inline since
 * they are called many times per second. They must be in the
 * header file because of this.
 */

inline
void GameEnv::display() {
	if (showMenu)
		menu.display();
	else
		displayGame();
}

inline
void GameEnv::Menu::display()
{
}

inline
void GameEnv::displayGame() {
	composeFrame();
	background.drawGLbackground ();

	for (deque<Block *>::iterator it = blocks.begin(); it < blocks.end(); ++it)
		for (int j=0; j<ncols; ++j) 
			if ((*it)[j].enabled)
				(*it)[j].draw(0);

	glutSwapBuffers();
}

inline
void GameEnv::processFrame()
{
	int i = 0;
	for (deque<Block *>::iterator it = blocks.begin(); it < blocks.end(); ++it, ++i) 
		for (int j=0; j<ncols; ++j) 
			if ((*it)[j].enabled) 
				(*it)[j].setY(grid_y - i * block_h);
}

inline
void GameEnv::composeFrame()
{
	if (Timer->elapsed(last_pushtime, 900)) {
		pushRow(0);
		last_pushtime = Timer->time();
	}

	if(Timer->elapsed(last_time,300))
	{
		processFrame();
    last_time=Timer->time();
	if(++current_frame>=1)
		current_frame=0;
	}

  glutPostRedisplay();
}

#endif