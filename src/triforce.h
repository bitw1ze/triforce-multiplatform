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

namespace MenuState {
	enum Actions {
		UP, DOWN, LEFT, RIGHT,
		ACTIVATE, QUIT,
		_NUMBER_OF_ACTIONS
	};
	static const string actionLabels[_NUMBER_OF_ACTIONS] = {
		"Up", "Down", "Left", "Right",
		"Activate", "Quit"
	};
};

 // FIXME: nearly everything is static... either:
 //   A) change to namespace  (i.e. no triforce class)
 //   B) use member functions instead (i.e. multiple class instances makes sense)
class Triforce {
public :
	enum GameState {MENU, PLAY, PAUSE, QUIT, _NUMBER_OF_STATES};
private:
	const static string gameStateLabels[_NUMBER_OF_STATES];

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
	void defineActions();
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
	void mouseButtons(int button, int mouseState, int x, int y);
};
