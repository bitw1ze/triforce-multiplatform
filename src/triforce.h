#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <deque>
#include "gobjlib.h"
#include "globals.h"
#include "game.h"

extern CTimer *mainTimer;

class Triforce {
private:
	enum gameState {load, play, pause};
	gameState state;

	static const string bgFile;
	BMPClass background;
	int current_frame,
		last_time;

public:
	GameEnv *gameEnv; //Temporarily here so that keyboard stuff can work in main; this should be private

	Triforce();
	void changeState(gameState s);
	void display();
	void composeFrame();
	void processFrame();
	void loadImages();
};