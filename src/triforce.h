#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <deque>
#include "buttons.h"
#include "gobjlib.h"
#include "globals.h"
#include "game.h"

using namespace Globals;

extern CTimer *mainTimer;

class Triforce {
public : enum gameState {load, play, pause, menu, quit};
private:
	GamePlay *gamePlay; 
	gameState state;

	Buttons * menuButtons;
	static const string bgFile;
	BMPClass background;

	int current_frame,
		last_time;

	void displayMenu();
	void composeFrame();
	void processFrame();
	void loadImages();

public:

	Triforce();
	~Triforce();
	void changeState(gameState s);
	static void changeStateWrapper(void *tfInstance, int gameState); // for callbacks
	void display();
	void specialKeys(int key, int x, int y);
	void normalKeys(unsigned char key, int x, int y);
};