#ifndef __GAME_H__
#define __GAME_H__

#include "2DGraphics.h"
#include "block.h"
#include <string>
#include <vector>
#include <ctime>

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
		last_time;

	bool showMenu;
	CTimer *Timer;
	BMPClass background;
	CBaseSprite* blockSprites[nblocktypes];
	Block blocks[nrows][ncols];
	

	void pushRow(int row);
public:
	GameEnv();
	void display();
	void displayGame();
	void ComposeFrame();
	void init();
	void ProcessFrame();
	bool LoadImages();
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
void GameEnv::displayGame() {
	ComposeFrame();
	background.drawGLbackground ();

	for (int i=0; i<nrows; ++i) 
		for (int j=0; j<ncols; ++j) 
			if (blocks[i][j].enabled)
				blocks[i][j].draw(0);

	glutSwapBuffers();
}

inline
void GameEnv::ProcessFrame()
{
	for (int i=0; i<nrows; ++i)
		for (int j=0; j<ncols; ++j)
			if (blocks[i][j].enabled)
				blocks[i][j].move();
}

inline
void GameEnv::ComposeFrame()
{
  if(Timer->elapsed(last_time,300))
  {
	  ProcessFrame();
    last_time=Timer->time();
    if(++current_frame>=1)
		current_frame=0;
  }

  glutPostRedisplay();
}

#endif