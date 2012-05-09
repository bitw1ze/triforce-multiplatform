#include "input.h"
#include <map>
#include <list>
#include <vector>

namespace Input
{
namespace
{
	list<Action *> availableActions; // all declared but undefined actions are here
	vector<Player *> players;
	map<unsigned char, list<Action *>> keyBindings;
	map<int, list<Action *>> specialKeyBindings;
	map<int, list<Action *>> mouseButtonBindings;

	int (*getState)() = NULL; // used to determine which actions are currently valid for Triforce
	const string *stateLabels; // array of labels; indices are states returned by getState()
	int numStates;

	/*
	 * Handle mouse motion
	 */ 
	typedef int ActiveState;
	typedef void (*MouseMotionFunc)(void *classInstance, int x, int y);
	class MouseCallback
	{
	public:
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

void Action::doAction(int actionState)
{
	if (getState && getState() == activeState)
		action(actionsClassInstance, actionState, actionType);
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

Action * Player::getAction(Action::ActionScope scope, int activeState, int actionType)
{
	for (list<Action *>::iterator a = actions.begin(); a != actions.end(); ++a)
		if ((*a)->isSameAction(scope, activeState, actionType))
			return *a;
	return (Action *)NULL;
}

void setGSFunc(int (*getStateFunc)())
{
	getState = getStateFunc;
}

void setGSLabels(const string * labels)
{
	stateLabels = labels;
}

void addPlayer()
{
	players.push_back(new Player());
}

/**
 * Button/key Inputs
 */

//FIXME: whole lot'a redundnancy 'round these here parts

void keyPress(unsigned char key, int x, int y)
{
	map<unsigned char, list<Action *>>::iterator b = keyBindings.find(key);
	if (b == keyBindings.end())
		return;

	//TODO: push key to the container that indicates it is being held (then remove it on Release)
	list<Action *>::iterator action;
	for (action = b->second.begin(); action != b->second.end(); ++action) 
		if ((*action)->hasActiveStateOf(getState()))
		{
			(*action)->doAction(Action::STATE_PRESS);
			return;
		}
}

void keyRelease(unsigned char key, int x, int y)
{
	map<unsigned char, list<Action *>>::iterator b = keyBindings.find(key);
	if (b == keyBindings.end())
		return;

	//TODO: push key to the container that indicates it is being held (then remove it on Release)
	list<Action *>::iterator action;
	for (action = b->second.begin(); action != b->second.end(); ++action) 
		if ((*action)->hasActiveStateOf(getState()))
		{
			(*action)->doAction(Action::STATE_RELEASE);
			return;
		}
}

void keySpecialPress(int key, int x, int y)
{
	map<int, list<Action *>>::iterator b = specialKeyBindings.find(key);
	if (b == specialKeyBindings.end())
		return;

	//TODO: push key to the container that indicates it is being held (then remove it on Release)
	list<Action *>::iterator action;
	for (action = b->second.begin(); action != b->second.end(); ++action) 
		if ((*action)->hasActiveStateOf(getState()))
		{
			(*action)->doAction(Action::STATE_PRESS);
			return;
		}
}

void keySpecialRelease(int key, int x, int y)
{
	map<int, list<Action *>>::iterator b = specialKeyBindings.find(key);
	if (b == specialKeyBindings.end())
		return;

	//TODO: push key to the container that indicates it is being held (then remove it on Release)
	list<Action *>::iterator action;
	for (action = b->second.begin(); action != b->second.end(); ++action) 
		if ((*action)->hasActiveStateOf(getState()))
		{
			(*action)->doAction(Action::STATE_RELEASE);
			return;
		}
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
	if (!getState)
		return;

	MouseMotionIter it;
	it = mouseMotionFuncs.find(getState());
	if (it != mouseMotionFuncs.end())
		it->second.mouseMotionFunc(it->second.classInstance, x, y);
}

// Activated when mouse moves while a mouse button ISN'T held down.
void mousePassiveMotion(int x, int y)
{
	if (!getState)
		return;

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
	availableActions.push_back(new Action(scope, activeState, actionType, shortDesc));
}

void defineAction(Action::ActionScope scope, int activeState, int actionType, void *classInstance, Action::ActionFunc action)
{
	vector<Player *>::iterator p = players.begin();
	switch (scope)
	{
	case Action::SCOPE_FIRST_PLAYER:
		// only check first player to see if this action is defined
		Action * a = (*p)->getAction(scope, activeState, actionType);
		if (a)
			a->define(classInstance, action);
		else // player doesn't have this action yet, so create it
		{
			a = new Action(findActionDecl(scope, activeState, actionType));
			a->define(classInstance, action);
			(*p)->addAction(a);
		}
		break;
		// FIXME: this stuff needs to be reviewed
		/*
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
		*/
	}
}

Action & findActionDecl(Action::ActionScope scope, int activeState, int actionType)
{
	// Find the action decl that matches this definition; this is the action to
	//  duplicate/add to players.
	list<Action *>::iterator a = availableActions.begin();
	for (; a != availableActions.end(); ++a)
		if ((*a)->isSameAction(scope, activeState, actionType))
			return **a;
}


/**
 * Binding
 */

void bindKey(Action action, unsigned char key)
{
}

//FIXME: too much duplicate code in bindKey()/bindSpecialKey()
void bindKey(int player, Action::ActionScope scope, int activeState, int actionType, unsigned char key)
{
	// find Action to bind to
	Action *action = players[player]->getAction(scope, activeState, actionType);
	if (!action)
		action = new Action(findActionDecl(scope, activeState, actionType));
	players[player]->addAction(action);

	map<unsigned char, list<Action *>>::iterator binding = keyBindings.find(key);
	if (binding == keyBindings.end()) // add binding for a new key
	{
		list<Action *> al; // create dummy list to push to k
		al.push_back(action);
		keyBindings.insert(pair<unsigned char, list<Action *>>(key, al));
	}
	else // or a binding for another state for an existing key
		binding->second.push_back(action);
}

void bindSpecialKey(Action action, int key)
{
}

void bindSpecialKey(int player, Action::ActionScope scope, int activeState, int actionType, int key)
{
	// find Action to bind to
	Action *action = players[player]->getAction(scope, activeState, actionType);
	if (!action)
		action = new Action(findActionDecl(scope, activeState, actionType));
	players[player]->addAction(action);

	map<int, list<Action *>>::iterator binding = specialKeyBindings.find(key);
	if (binding == specialKeyBindings.end()) // add binding for a new key
	{
		list<Action *> al; // create dummy list to push to k
		al.push_back(action);
		specialKeyBindings.insert(pair<int, list<Action *>>(key, al));
	}
	else // or a binding for another state for an existing key
		binding->second.push_back(action);
}

void bindButton(Action action, int button)
{
}

} // Input