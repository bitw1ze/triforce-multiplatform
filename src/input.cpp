#include "input.h"
#include <map>
#include <list>
#include <set>
#include <vector>

namespace Input
{
namespace
{
	typedef list<Action *> Actions;
	typedef vector<Player *> Players;
	typedef unsigned char Key;
	typedef int SpecialKey;
	typedef int MouseButton;
	typedef int XboxButton;
	typedef map<Key, Actions> KeyBindings;
	typedef map<SpecialKey, Actions> SpecialKeyBindings;
	typedef map<MouseButton, Actions> MouseButtonBindings;
	typedef map<XboxButton, Actions> XboxButtonBindings;

	Actions availableActions; // all declared but undefined actions are here
	Players players;
	KeyBindings keyBindings;
	SpecialKeyBindings specialKeyBindings;
	MouseButtonBindings mouseButtonBindings;
	XboxButtonBindings xboxButtonBindings;
	ActionQueue actionQueue;

	int (*getState)() = NULL; // used to determine which actions are currently valid for Triforce
	const string *stateLabels; // array of labels; indices are states returned by getState()
	int numStates;

	/*
	 * Keys currently being held down
	 */ 
	typedef set<unsigned char> KeysDown;
	typedef set<int> SpecialKeysDown;

	KeysDown keysDown;
	SpecialKeysDown specialKeysDown;

	/*
	 * Handle mouse motion
	 */ 
	class MouseCallback
	{
	private:
		typedef void (*MouseMotionFunc)(void *classInstance, int x, int y);
	public:
		MouseCallback(void *classInstance, MouseMotionFunc mouseMotionFunc) : classInstance(classInstance), mouseMotionFunc(mouseMotionFunc) {}
		void *classInstance;
		MouseMotionFunc mouseMotionFunc;
	};

	typedef int ActiveState;
	typedef list<MouseCallback> MouseCallbacks;
	typedef map<ActiveState, MouseCallbacks> MouseMotion;

	MouseMotion mouseMotionFuncs;
	MouseMotion mousePassiveMotionFuncs;

	// update bindings when adding 2nd, 3rd, etc actions for same scope/state/type
	void bindUpdateAll(Action * action)
	{
		Actions::iterator a;
		for (KeyBindings::iterator k = keyBindings.begin(); k != keyBindings.end(); ++k)
		{
			a = k->second.begin();
			if ((*a)->isSameAction(action))
			{
				k->second.push_back(action);
				break;
			}
		}
		for (SpecialKeyBindings::iterator k = specialKeyBindings.begin(); k != specialKeyBindings.end(); ++k)
		{
			a = k->second.begin();
			if ((*a)->isSameAction(action))
			{
				k->second.push_back(action);
				break;
			}
		}
		for (MouseButtonBindings::iterator k = mouseButtonBindings.begin(); k != mouseButtonBindings.end(); ++k)
		{
			a = k->second.begin();
			if ((*a)->isSameAction(action))
			{
				k->second.push_back(action);
				break;
			}
		}
	}
} // unnamed

void Action::define(void * actionClassInstance, ActionFunc action)
{
	this->actionsClassInstance = actionClassInstance;
	this->action = action;
}

bool Action::isSameAction(Action * action)
{
	// if the overloaded function changes, this won't compile (good)
	return isSameAction(action->scope, action->activeState, action->actionType);
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

Player::Player()
{
	enabled = false;
}

void Player::addAction(Action *action)
{
	actions.push_back(action);
}

bool Player::isActionDefined(Action::ActionScope scope, int activeState, int actionType)
{
	for (Actions::iterator a = availableActions.begin(); a != availableActions.end(); ++a)
		if ((*a)->isSameAction(scope, activeState, actionType))
			return true;
	return false;
}

bool Player::hasActionsDefined(Action::ActionScope scope, int activeState)
{
	for (Actions::iterator a = availableActions.begin(); a != availableActions.end(); ++a)
		if ((*a)->isRelatedAction(scope, activeState))
			return true;
	return false;
}

Action * Player::getAction(Action::ActionScope scope, int activeState, int actionType)
{
	for (Actions::iterator a = actions.begin(); a != actions.end(); ++a)
		if ((*a)->isSameAction(scope, activeState, actionType))
			return *a;
	return (Action *)NULL;
}

ActionQueue::ActionQueue()
{
	gameStateActionsAreFor = -1;
}

bool ActionQueue::updateGameState()
{
	int gameState = getState();
	if (gameState != gameStateActionsAreFor)
	{
		gameStateActionsAreFor = gameState;
		clear();
		return true;
	}
	return false;
}

void ActionQueue::enqueue(Action * action, Action::ActionState state)
{
	updateGameState();
	ActionEntry entry(action, state);
	actions.push_back(entry);
}

void ActionQueue::doAll()
{
	// add all buttons being held down (since repeated GLUT interrupts are ignored)
	Actions::iterator action;
	KeyBindings::iterator b;
	for (KeysDown::const_iterator kd = keysDown.cbegin(); kd != keysDown.cend(); ++kd)
	{
		b = keyBindings.find(*kd);
		if (b == keyBindings.end())
			continue;
		for (action = b->second.begin(); action != b->second.end(); ++action) 
			if ((*action)->hasActiveStateOf(getState()))
				enqueue(*action, Action::STATE_HOLD);
	}
	SpecialKeyBindings::iterator sb;
	for (SpecialKeysDown::const_iterator kd = specialKeysDown.cbegin(); kd != specialKeysDown.cend(); ++kd)
	{
		sb = specialKeyBindings.find(*kd);
		if (sb == specialKeyBindings.end())
			continue;
		for (action = sb->second.begin(); action != sb->second.end(); ++action) 
			if ((*action)->hasActiveStateOf(getState()))
				enqueue(*action, Action::STATE_HOLD);
	}

	// do all actions
	ActionEntry ae;
	while (actions.size())
	{
		// detect state changes, even as a result of prior actions in the same queue
		if (updateGameState()) 
			return;

        ae = actions.front();
		ae.action->doAction(ae.state);
		actions.pop_front();
	}
}

void handleInput()
{
	actionQueue.doAll();
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

template <class B, class D, class K>
void anyKeyPress(B &bindings, D &keysDown, K key, int x, int y)
{
	B::iterator b = bindings.find(key);
	if (b == bindings.end())
		return;

	Actions::iterator action;
	for (action = b->second.begin(); action != b->second.end(); ++action) 
		if ((*action)->hasActiveStateOf(getState()))
		{
			if (keysDown.find(key) == keysDown.end())
			{
				actionQueue.enqueue(*action, Action::STATE_PRESS);
				keysDown.insert(key);
			}
			// ignore multiple calls for a single press (they're handled using keysDown)
		}
}

template <class B, class D, class K>
void anyKeyRelease(B &bindings, D &keysDown, K key, int x, int y)
{
	B::iterator b = bindings.find(key);
	D::iterator k;
	if (b == bindings.end())
		return;

	Actions::iterator action;
	for (action = b->second.begin(); action != b->second.end(); ++action) 
		if ((*action)->hasActiveStateOf(getState()))
		{
			actionQueue.enqueue(*action, Action::STATE_RELEASE);
			k = keysDown.find(key);
			if (k != keysDown.end())
				keysDown.erase(*k);
		}
}

void keyPress(unsigned char key, int x, int y)
{
	anyKeyPress(keyBindings, keysDown, key, x, y);
}

void keyRelease(unsigned char key, int x, int y)
{
	anyKeyRelease(keyBindings, keysDown, key, x, y);
}

void keySpecialPress(int key, int x, int y)
{
	anyKeyPress(specialKeyBindings, specialKeysDown, key, x, y);
}

void keySpecialRelease(int key, int x, int y)
{
	anyKeyRelease(specialKeyBindings, specialKeysDown, key, x, y);
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

	MouseMotion::iterator mm = mouseMotionFuncs.find(getState());
	if (mm != mouseMotionFuncs.end())
		for (MouseCallbacks::iterator mc = mm->second.begin(); mc != mm->second.end(); ++mc)
			mc->mouseMotionFunc(mc->classInstance, x, y);
}

// Activated when mouse moves while a mouse button ISN'T held down.
void mousePassiveMotion(int x, int y)
{
	if (!getState)
		return;

	MouseMotion::iterator mm = mousePassiveMotionFuncs.find(getState());
	if (mm != mousePassiveMotionFuncs.end())
		for (MouseCallbacks::iterator mc = mm->second.begin(); mc != mm->second.end(); ++mc)
			mc->mouseMotionFunc(mc->classInstance, x, y);
}

void addMouseMotionFunc(void *classInstance, int activeState, void (*mouseMotion)(void *classInstance, int x, int y))
{
	MouseCallback cb(classInstance, mouseMotion);
	MouseMotion::iterator mm = mouseMotionFuncs.find(activeState);
	if (mm == mouseMotionFuncs.end())
	{
		MouseCallbacks cbs; // create dummy list to push to
		cbs.push_back(cb);
		mouseMotionFuncs.insert(pair<ActiveState, MouseCallbacks>(activeState, cbs));
	}
	else
		mm->second.push_back(cb);
}

void addMousePassiveMotionFunc(void *classInstance, int activeState, void (*mouseMotion)(void *classInstance, int x, int y))
{
	MouseCallback cb(classInstance, mouseMotion);
	MouseMotion::iterator mm = mousePassiveMotionFuncs.find(activeState);
	if (mm == mousePassiveMotionFuncs.end())
	{
		MouseCallbacks cbs; // create dummy list to push to
		cbs.push_back(cb);
		mousePassiveMotionFuncs.insert(pair<ActiveState, MouseCallbacks>(activeState, cbs));
	}
	else
		mm->second.push_back(cb);
}

void removeMotions(void *classInstance)
{
	for (MouseMotion::iterator m = mouseMotionFuncs.begin(); m != mouseMotionFuncs.end(); ++m)
		for (MouseCallbacks::iterator mc = m->second.begin(); mc != m->second.end(); ++mc)
			if (mc->classInstance == classInstance)
				m->second.erase(mc);
	for (MouseMotion::iterator m = mousePassiveMotionFuncs.begin(); m != mousePassiveMotionFuncs.end(); ++m)
		for (MouseCallbacks::iterator mc = m->second.begin(); mc != m->second.end(); ++mc)
			if (mc->classInstance == classInstance)
				m->second.erase(mc);
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
	Players::iterator p = players.begin();
	switch (scope)
	{
	case Action::SCOPE_FIRST_PLAYER:
		// only check first player to see if this action is defined
		Action * a = (*p)->getAction(scope, activeState, actionType);
		if (a)
		{
			if (a->isDefined())
			{
				// link another action for the same thing
				Action * newAction = new Action(*a);
				newAction->define(classInstance, action);
				(*p)->addAction(newAction);

				// FIXME: update bindings to account for this action
				bindUpdateAll(newAction);
			}
			else
				a->define(classInstance, action);
		}
		else // player doesn't have this action yet, so create it
		{
			a = new Action(*findActionDecl(scope, activeState, actionType));
			a->define(classInstance, action);
			(*p)->addAction(a);
		}
		break;
	// FIXME: support for other players not yet fully implemented
	//case Action::SCOPE_CURRENT_PLAYER:
	//case Action::SCOPE_ALL_PLAYERS:
	}
}

Action * findActionDecl(Action::ActionScope scope, int activeState, int actionType)
{
	// Find the action decl that matches this definition; this is the action to
	//  duplicate/add to players.
	Actions::iterator a = availableActions.begin();
	for (; a != availableActions.end(); ++a)
		if ((*a)->isSameAction(scope, activeState, actionType))
			return *a;
	return (Action *)NULL;
}


/**
 * Binding
 */

//FIXME: too much duplicate code in bindKey()/bindSpecialKey()
void bindKey(int player, Action::ActionScope scope, int activeState, int actionType, unsigned char key)
{
	// find Action to bind to
	Action *action = players[player]->getAction(scope, activeState, actionType);
	if (!action)
		action = new Action(*findActionDecl(scope, activeState, actionType));
	players[player]->addAction(action);

	KeyBindings::iterator binding = keyBindings.find(key);
	if (binding == keyBindings.end()) // add binding for a new key
	{
		Actions al; // create dummy list to push to k
		al.push_back(action);
		keyBindings.insert(pair<unsigned char, Actions>(key, al));
	}
	else // or a binding for another state for an existing key
		binding->second.push_back(action);
}

void bindSpecialKey(int player, Action::ActionScope scope, int activeState, int actionType, int key)
{
	// find Action to bind to
	Action *action = players[player]->getAction(scope, activeState, actionType);
	if (!action)
		action = new Action(*findActionDecl(scope, activeState, actionType));
	players[player]->addAction(action);

	SpecialKeyBindings::iterator binding = specialKeyBindings.find(key);
	if (binding == specialKeyBindings.end()) // add binding for a new key
	{
		Actions al; // create dummy list to push to k
		al.push_back(action);
		specialKeyBindings.insert(pair<int, Actions>(key, al));
	}
	else // or a binding for another state for an existing key
		binding->second.push_back(action);
}

void bindButton(Action action, int button)
{
}

void bindXboxButton(int player, Action::ActionScope scope, int activeState, int actionType, int key)
{
	// find Action to bind to
	Action *action = players[player]->getAction(scope, activeState, actionType);
	if (!action)
		action = new Action(*findActionDecl(scope, activeState, actionType));
	players[player]->addAction(action);

	XboxButtonBindings::iterator binding = xboxButtonBindings.find(key);
	if (binding == xboxButtonBindings.end()) // add binding for a new key
	{
		Actions al; // create dummy list to push to k
		al.push_back(action);
		xboxButtonBindings.insert(pair<int, Actions>(key, al));
	}
	else // or a binding for another state for an existing key
		binding->second.push_back(action);
}

/*
template <type T>
void bind(T Bindings, int player, Action::ActionScope scope, int activeState, int actionType, int key)
{
}
*/

} // Input