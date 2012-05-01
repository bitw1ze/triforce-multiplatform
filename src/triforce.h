#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <deque>
#include "buttons.h"
#include "2DGraphics.h"
#include "globals.h"
#include "game.h"

using namespace Globals;

extern CTimer *mainTimer;

class Triforce {
public : enum gameState {menu, play, pause, quit};
private:
	GamePlay *gamePlay; 
	gameState state;

	Buttons * menuButtons;
	static const string bgFile;
	BMPClass background;

	int current_frame,
		last_time;

	list<unsigned char> keysDown;

	void displayMenu();
	void composeFrame();
	void processFrame();
	void loadImages();

public:

	Triforce();
	~Triforce();
	void changeState(gameState s);
	void display();
	static void changeStateWrapper(void *tfInstance, int gameState); // for callbacks
	void specialKeys(int key, int x, int y);
	void normalKeys(unsigned char key, int x, int y);
	void keyUp(unsigned char key, int x, int y);
	void mouseButtons(int button, int mouseState, int x, int y);
	void mouseMotion(int x, int y);
	void mousePassiveMotion(int x, int y);
};