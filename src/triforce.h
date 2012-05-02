#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <deque>
#include "input.h"
#include "buttons.h"
#include "2DGraphics.h"
#include "globals.h"
#include "game.h"

using namespace Globals;

extern CTimer *mainTimer;

class Triforce {
public : enum gameState {MENU, PLAY, PAUSE, QUIT};
public : enum actions {ACT_UP, ACT_DOWN, ACT_LEFT, ACT_RIGHT};
private:
	static gameState state;
	static GamePlay *gamePlay; 

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
	static void setState(gameState s);
	static gameState getState() {return state;}
	void display();
	// FIXME: now that setState is static, does this wrapper really need to exist?
	static void setStateWrapper(void *tfInstance, int gameState); // for Button callbacks

	static void registerActions();
	static void doAction(void *tfInstance, actions action); // for Input callback

	/* deprecated */
	void specialKeys(int key, int x, int y);
	void normalKeys(unsigned char key, int x, int y);
	void mouseButtons(int button, int mouseState, int x, int y);
	void mouseMotion(int x, int y);
	void mousePassiveMotion(int x, int y);
};