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
private:
	enum gameState {load, play, pause};
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
	// Temporarily here so that keyboard stuff can work in main... but
	// this should really be private
	GameEnv *gameEnv; 

	Triforce();
	~Triforce();
	void changeState(gameState s);
	void display();
};