#pragma once

#include <list>
#include "2DGraphics.h"
#include "globals.h"
#include "triforce.h"

/*
 *  In short, the job of this class is tell the currently active part
 *  of the game that the user wants to take action X. 
 *
 *  Each part of the game that needs to take actions based on input will
 *  register a single callback function which handles all of its actions.
 *  When Input receives mouse/keyboard/controller/etc. input, it
 *  calls the appropriate callback function, which depends upon the state of
 *  Triforce. The callback func is passed an action, such as UP or DOWN,
 *  which it handles. This means that all functions in the game are
 *  encapsulated from any particular input device or key binding. Also, 
 *  the Input class is isolated from changes to each class, so coupling is
 *  as loose as it can be.
 *
 *  When the actions are performed may depend upon context. For example, when the
 *  button that swaps blocks is held down, it should only take the action once,
 *  and it should do so immediately upon pushing the button down. If one of the
 *  movement keys is held down, the action should be taken immediately and 
 *  repeatedly until it is depressed; perhaps even on a particular interval.
 *  If a menu button is pressed, however, the action should not fire until the
 *  key is released.
 * 
 * FIXME: explain how the above will be implemented
 *
 *  Implemetation:
 *    Each class registering actions with Input will supply:
 *       a pointer to itself
 *       a pointer to its action function, which knows how to handle each action
 *          type supported by the class.
 *       enums of the type of actions it supports
 *       strings describing those actions (for a future config class)
 *
 *     When input events occur, the state is checked, and the appropriate action
 *     function is called. Which arguments are passed to the action function
 *     depends upon how the keys are bound.
 *
 */

class Input
{
private:
	class Action
	{
	public:
		int activeState; // action is only active this matches getState()
		int actionType; // type of action, which is passed as an arg to the action func
		string shortDesc; // 1-2 word description of what action does

		void *actionsClassInstance; // class instance that action belongs to

		/* callbacks of all registered action functions */
		void (*action)(void (*actionsClassInstance)(), int actionType);
	};
	static int (*getState)(); // used to determine which actions are currently valid for Triforce
	static list<Action> actions;

	// keys that are currently being held down
	static list<unsigned char> keysDown;
	static list<unsigned char> keysSpecialDown;

public:
	static void setGSFunc(int (*getStateFunc)()) {getState = getStateFunc;}
	Input(int (*getState)());
	static void registerAction(void *classInstance, void (*action)(void *, int),
	                           int actionType, string shortDesc);
	// TODO: a class that loads config files will eventually handle binding these
	static void bindKey(Action action, unsigned char key);
	static void bindSpecialKey(Action action, int key);
	static void bindButton(Action action, int button);
	static void doAction(int actionType);

	// actions
	static void keyPress(unsigned char key, int x, int y);
	static void keyRelease(unsigned char key, int x, int y);
	static void keySpecialPress(int key, int x, int y);
	static void keySpecialRelease(int key, int x, int y);
	static void mousePress(int button, int mouseState, int x, int y);

	// motions
	static void mouseMove(int x, int y);
	static void mousePassiveMove(int x, int y);
};