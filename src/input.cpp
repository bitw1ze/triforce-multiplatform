#include "input.h"
#include <map>
#include <list>

namespace Input
{

namespace
{
	list<Action *> availableActions;
	list<Player> players;
	int (*getState)() = NULL; // used to determine which actions are currently valid for Triforce

	/*
	 * Handle mouse motion
	 */ 
	typedef int ActiveState;
	typedef void (*MouseMotionFunc)(void *classInstance, int x, int y);
	struct MouseCallback
	{
		void *classInstance;
		MouseMotionFunc mouseMotionFunc;
	};
	typedef map<ActiveState, MouseCallback>::iterator MouseMotionIter;
	map<ActiveState, MouseCallback> mouseMotionFuncs;
	map<ActiveState, MouseCallback> mousePassiveMotionFuncs;

	/*
	 * Keys currently being held down
	 */ 
	list<unsigned char> keysDown;
	list<unsigned char> keysSpecialDown;

	void warnTooManyStateHandlers(string callerName)
	{
		cout << "Warning: ignored attempt to register more than one callback"
			 << "function to handle the same input for the same programs state "
			 << "(" << callerName << ")" << endl;
	}
} // unnamed

void Action::define(void * actionClassInstance, ActionFunc action)
{
	this->actionsClassInstance = actionClassInstance;
	this->action = action;
}

void Player::addAction(void *classInstance, void (*action)(void *, int),
                           int actionType, string shortDesc)
{
}

void setGSFunc(int (*getStateFunc)())
{
	getState = getStateFunc;
}

/**
 * Button/key Inputs
 */

void keyPress(unsigned char key, int x, int y)
{
}

void keyRelease(unsigned char key, int x, int y)
{
}

void keySpecialPress(int key, int x, int y)
{
	
}

void keySpecialRelease(int key, int x, int y)
{
}

void mousePress(int button, int mouseState, int x, int y)
{
}


/**
 * Motion Inputs
 */

// Activated when mouse moves while a mouse button IS held down.
void mouseMotion(int x, int y)
{
	MouseMotionIter it;
	it = mouseMotionFuncs.find(getState());
	if (it != mouseMotionFuncs.end())
		it->second.mouseMotionFunc(it->second.classInstance, x, y);
}

// Activated when mouse moves while a mouse button ISN'T held down.
void mousePassiveMotion(int x, int y)
{
	MouseMotionIter it;
	it = mousePassiveMotionFuncs.find(getState());
	if (it != mousePassiveMotionFuncs.end())
		it->second.mouseMotionFunc(it->second.classInstance, x, y);
}

void addMouseMotionFunc(void *classInstance, int activeState, void (*mouseMotion)(void *classInstance, int x, int y))
{
	MouseCallback cb;
	cb.classInstance = classInstance;
	cb.mouseMotionFunc = mouseMotion;

	pair<MouseMotionIter,bool> ret;
	ret = mouseMotionFuncs.insert(
			pair<ActiveState,MouseCallback>(activeState, cb));
	if (!ret.second)
		warnTooManyStateHandlers(__FUNCTION__);
}

void addMousePassiveMotionFunc(void *classInstance, int activeState, void (*mouseMotion)(void *classInstance, int x, int y))
{
	MouseCallback cb;
	cb.classInstance = classInstance;
	cb.mouseMotionFunc = mouseMotion;

	pair<MouseMotionIter,bool> ret;
	ret = mousePassiveMotionFuncs.insert(
			pair<ActiveState,MouseCallback>(activeState, cb));
	if (!ret.second)
		warnTooManyStateHandlers(__FUNCTION__);
}

void removeMotions(void *classInstance)
{
	for (MouseMotionIter m = mouseMotionFuncs.begin(); m != mouseMotionFuncs.end(); ++m)
		if (m->second.classInstance == classInstance)
			mouseMotionFuncs.erase(m);
	for (MouseMotionIter m = mousePassiveMotionFuncs.begin(); m != mousePassiveMotionFuncs.end(); ++m)
		if (m->second.classInstance == classInstance)
			mousePassiveMotionFuncs.erase(m);
}

/**
 * Actions
 */

void declareAction(int activeState, int actionType, string shortDesc)
{
	Action *a = new Action(activeState, actionType, shortDesc);
	availableActions.push_back(a);
}

/**
 * Binding
 */

void bindKey(Action action, unsigned char key)
{
}

void bindSpecialKey(Action action, int key)
{
}

void bindButton(Action action, int button)
{
}

void doAction(int actionType)
{
	int activeState = getState();
	// FIXME: incomplete
}

} // Input