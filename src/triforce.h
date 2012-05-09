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
	enum GameState {MENU, PLAY, PAUSE, QUIT, _NUMBER_OF_STATES};
	enum Actions {ACT_UP, ACT_DOWN, ACT_LEFT, ACT_RIGHT, ACT_ACTIVATE, ACT_QUIT, _NUMBER_OF_ACTIONS};
private:
	const static string gameStateLabels[_NUMBER_OF_STATES];
	const static string actionLabels[_NUMBER_OF_ACTIONS];

	/**
	  * Files
	  */
	static const string bgFile,
		                playBtns[],
		                quitBtns[];

	static GameState state;
	static GamePlay *gamePlay;
	
	Buttons * menuButtons;
	BMPClass background;

	int current_frame,
		last_time;

	void bindDefaultActionKeys();

	/**
	 * Display routines
	 */
	void displayMenu();
	void composeFrame();
	void processFrame();
	void loadImages();

public:
	static void declareActions();
	static void doAction(void *tfInstance, int actionState, int actionType);

	Triforce();
	~Triforce();
	void display();

	/**
	 * State
	 */
	static void setState(GameState s);
	// FIXME: now that setState is static, does this wrapper really need to exist?
	static void setStateWrapper(void *tfInstance, int gameState); // for Button callbacks
	static GameState getState() {return state;}

	// deprecated
	void specialKeys(int key, int x, int y);
	void normalKeys(unsigned char key, int x, int y);
	void mouseButtons(int button, int mouseState, int x, int y);
};
