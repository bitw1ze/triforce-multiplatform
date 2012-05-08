#include "input.h"
#include <map>
#include <list>
#include <vector>

namespace Input
{
	int Player::playerCount = 0;

namespace
{
	list<Action *> availableActions; // all declared but undefined actions are here
	vector<Player *> players;
	int (*getState)() = NULL; // used to determine which actions are currently valid for Triforce
	const string *stateLabels; // array of labels; indices are states returned by getState()
	int numStates;

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

bool Action::isSameAction(ActionScope scope, int activeState, int actionType)
{
	return this->scope == scope &&
	       this->activeState == activeState &&
		   this->actionType == actionType;
}

bool Action::isRelatedAction(ActionScope scope, int activeState)
{
	return this->scope == scope &&
	       this->activeState == activeState;
}

void Player::addAction(Action *action)
{
	actions.push_back(action);
}

bool Player::isActionDefined(Action::ActionScope scope, int activeState, int actionType)
{
	for (list<Action *>::iterator a = availableActions.begin(); a != availableActions.end(); ++a)
		if ((*a)->isSameAction(scope, activeState, actionType))
			return true;
	return false;
}

bool Player::hasActionsDefined(Action::ActionScope scope, int activeState)
{
	for (list<Action *>::iterator a = availableActions.begin(); a != availableActions.end(); ++a)
		if ((*a)->isRelatedAction(scope, activeState))
			return true;
	return false;
}

void setGSFunc(int (*getStateFunc)())
{
	getState = getStateFunc;
}

void setGSLabels(const string * labels)
{
	stateLabels = labels;
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

void declareAction(Action::ActionScope scope, int activeState, int actionType, string shortDesc)
{
	Action *a = new Action(scope, activeState, actionType, shortDesc);
	availableActions.push_back(a);
}

void defineAction(Action::ActionScope scope, int activeState, int actionType, void *classInstance, Action::ActionFunc action)
{
	// Find the action decl that matches this definition; this is the action to
	//  duplicate/add to players.
	list<Action *>::iterator a = availableActions.begin();
	for (; a != availableActions.end(); ++a)
		if ((*a)->isSameAction(scope, activeState, actionType))
			break;
	//FIXME: should bomb if action is not declared

	Action * newAction = new Action(**a);
	newAction->define(classInstance, action);

	vector<Player *>::iterator p = players.begin();
	switch (scope)
	{
	case Action::SCOPE_FIRST_PLAYER:
		// only check first player to see if this action is defined
		(*p)->addAction(newAction);
		break;
	case Action::SCOPE_CURRENT_PLAYER:
		// go through each player; if every player has this action defined, create
		// a new player and add the action to him
		for (; p != players.end(); ++p)
			if (!((*p)->isActionDefined(scope, activeState, actionType)))
			{
				(*p)->addAction(newAction);
				break;
			}
			// push back a new player, since the action is defined for everyone else
			players.push_back(new Player());
			players.back()->addAction(newAction);
		break;
	case Action::SCOPE_ALL_PLAYERS:
		// add definition to all player actions containers
		for (; p != players.end(); ++p)
			(*p)->addAction(newAction);
		break;
	}
}

void defineActions(Action::ActionScope scope, int activeState, void *classInstance, Action::ActionFunc action)
{
	Action * newAction;
	vector<Player *>::iterator p;

	// for every action decl that matches this definition, duplicate/add to players.
	for (list<Action *>::iterator a = availableActions.begin(); a != availableActions.end(); ++a)
	{
		if ((*a)->isRelatedAction(scope, activeState))
		{
			newAction = new Action(**a);
			newAction->define(classInstance, action);
			switch (scope)
			{
			case Action::SCOPE_FIRST_PLAYER:
				(*p)->addAction(newAction);
				break;
			case Action::SCOPE_CURRENT_PLAYER:
				// go through each player; if every player has this action defined, create
				// a new player and add the action to him
				for (p = players.begin(); p != players.end(); ++p)
					if(!(*p)->hasActionsDefined(scope, activeState))
					{
						(*p)->addAction(newAction);
						break;
					}
				// push back a new player, since the action is defined for everyone else
				players.push_back(new Player());
				players.back()->addAction(newAction);
				break;
			case Action::SCOPE_ALL_PLAYERS:
				for (p = players.begin(); p != players.end(); ++p)
					(*p)->addAction(newAction);
				break;
			}
		}
	}
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