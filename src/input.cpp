#include "input.h"
#include <map>
#include <list>

namespace Input
{

namespace
{
	list<Action *> availableActions;
	list<Controller> controllers;
	int (*getState)() = NULL; // used to determine which actions are currently valid for Triforce

	/*
	 * Handle mouse motion
	 */ 
	typedef int ActiveState;
	typedef void (*MouseMotionFunc)(int x, int y);
	typedef map<ActiveState, MouseMotionFunc>::iterator MouseMotionIter;
	map<ActiveState, MouseMotionFunc> mouseMotionFuncs;
	map<ActiveState, MouseMotionFunc> mousePassiveMotionFuncs;

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

void Controller::addAction(void *classInstance, void (*action)(void *, int),
                           int actionType, string shortDesc)
{
}

void setGSFunc(int (*getStateFunc)())
{
	getState = getStateFunc;
}

void declareAction(Action * action)
{
	availableActions.push_back(action);
}

void addMouseMotionFunc(int activeState, void (*mouseMotion)(int x, int y))
{
	pair<MouseMotionIter,bool> ret;
	ret = mouseMotionFuncs.insert(
			pair<ActiveState,MouseMotionFunc>(activeState, mouseMotion));
	if (!ret.second)
		warnTooManyStateHandlers(__FUNCTION__);
}

void addMousePassiveMotionFunc(int activeState, void (*mouseMotion)(int x, int y))
{
	pair<MouseMotionIter,bool> ret;
	ret = mousePassiveMotionFuncs.insert(
			pair<ActiveState,MouseMotionFunc>(activeState, mouseMotion));
	if (!ret.second)
		warnTooManyStateHandlers(__FUNCTION__);
}

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

void keySpecialPress(int key, int x, int y)
{
	
}

void keySpecialRelease(int key, int x, int y)
{
}

void keyPress(unsigned char key, int x, int y)
{
}

void keyRelease(unsigned char key, int x, int y)
{
}

void mousePress(int button, int mouseState, int x, int y)
{
}

/**
 *  Activated when mouse moves while a mouse button IS held down.
 */
void mouseMotion(int x, int y)
{
	MouseMotionIter it;
	it = mouseMotionFuncs.find(getState());
	if (it != mouseMotionFuncs.end())
		it->second(x, y);
}

/**
 *  Activated when mouse moves while a mouse button ISN'T held down.
 */
void mousePassiveMotion(int x, int y)
{
	MouseMotionIter it;
	it = mousePassiveMotionFuncs.find(getState());
	if (it != mousePassiveMotionFuncs.end())
		it->second(x, y);
}

} // Input