#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <deque>
#include "buttons.h"
#include "2DGraphics.h"
#include "globals.h"
#include "game.h"
#include "input.h"

using namespace Globals;

extern CTimer *mainTimer;

 // FIXME: nearly everything is static... either:
 //   A) change to namespace  (i.e. no triforce class)
 //   B) use member functions instead (i.e. multiple class instances makes sense)

class Triforce {
public :
	enum gameState {MENU, PLAY, PAUSE, QUIT, _NUMBER_OF_STATES};
	const static string gameStateLabels[_NUMBER_OF_STATES];

	enum actions {ACT_UP, ACT_DOWN, ACT_LEFT, ACT_RIGHT, ACT_ACTIVATE, ACT_QUIT, _NUMBER_OF_ACTIONS};
	const static string menuActionLabels[_NUMBER_OF_ACTIONS];
private:
	/**
	  * Files
	  */
	static const string playBtns[],
		                quitBtns[];
	static const string bgFile;

	static gameState state;
	static GamePlay *gamePlay;
	
	Buttons * menuButtons;
	BMPClass background;

	int current_frame,
		last_time;

	/**
	 * Input actions routines
	 */
	static void declareActions(void *tfInstance);
	void defineActions();

	/**
	 * Display routines
	 */
	void displayMenu();
	void composeFrame();
	void processFrame();
	void loadImages();

public:

	Triforce();
	~Triforce();
	void display();

	/**
	 * State
	 */
	static void setState(gameState s);
	// FIXME: now that setState is static, does this wrapper really need to exist?
	static void setStateWrapper(void *tfInstance, int gameState); // for Button callbacks
	static gameState getState() {return state;}

	/**
	 * Input Actions
	 */
	static void doAction(void *tfInstance, actions action); // for Input callback

	// deprecated
	void specialKeys(int key, int x, int y);
	void normalKeys(unsigned char key, int x, int y);
	void mouseButtons(int button, int mouseState, int x, int y);
};
