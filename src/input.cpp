#include "input.h"
#include <map>
#include <list>
#include <set>
#include <vector>
#include "Xbox.h"

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

	XboxController xboxPlayer1; // only handle 1 xbox ctrl for now

	/*
	 * Keys currently being held down
	 */ 
	typedef set<unsigned char> KeysDown;
	typedef set<int> SpecialKeysDown;
	typedef set<int> XboxButtonsDown;

	KeysDown keysDown;
	SpecialKeysDown specialKeysDown;
	XboxButtonsDown xboxButtonsDown;

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


	/*
	 * Private Input functions
	 */

	// subroutine for bindUpdateAll()
	template <class B>
	void bindUpdate(B &bindings, Action * action)
	{
		Actions::iterator a;
		for (B::iterator k = bindings.begin(); k != bindings.end(); ++k)
		{
			a = k->second.begin();
			if ((*a)->isSameAction(action))
			{
				k->second.push_back(action);
				break;
			}
		}
	}

	// update bindings when adding 2nd, 3rd, etc actions for same scope/state/type
	void bindUpdateAll(Action * action)
	{
		bindUpdate(keyBindings, action);
		bindUpdate(specialKeyBindings, action);
		bindUpdate(xboxButtonBindings, action);
		bindUpdate(mouseButtonBindings, action);
	}

	template <class M>
	void anyMouseMotion(M &mouseMotionFuncs, int x, int y)
	{
		if (!getState)
			return;

		M::iterator mm = mouseMotionFuncs.find(getState());
		if (mm != mouseMotionFuncs.end())
			for (MouseCallbacks::iterator mc = mm->second.begin(); mc != mm->second.end(); ++mc)
				mc->mouseMotionFunc(mc->classInstance, x, y);
	}

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

	template <class M>
	void addAnyMouseMotionFunc(M &mouseMotionFuncs, void *classInstance, int activeState, void (*mouseMotion)(void *classInstance, int x, int y))
	{
		MouseCallback cb(classInstance, mouseMotion);
		M::iterator mm = mouseMotionFuncs.find(activeState);
		if (mm == mouseMotionFuncs.end())
		{
			MouseCallbacks cbs; // create dummy list to push to
			cbs.push_back(cb);
			mouseMotionFuncs.insert(pair<ActiveState, MouseCallbacks>(activeState, cbs));
		}
		else
			mm->second.push_back(cb);
	}

	template <class M>
	void removeMouseMotionFuncs(M &mouseMotionFuncs, void *classInstance)
	{
		for (M::iterator m = mouseMotionFuncs.begin(); m != mouseMotionFuncs.end(); ++m)
			for (MouseCallbacks::iterator mc = m->second.begin(); mc != m->second.end(); ++mc)
				if (mc->classInstance == classInstance)
					m->second.erase(mc);
	}

	template <class B, class K>
	void bind(B &bindings, int player, Action::ActionScope scope, int activeState, int actionType, K key)
	{
		// find Action to bind to
		Action *action = players[player]->getAction(scope, activeState, actionType);
		if (!action)
			action = new Action(*findActionDecl(scope, activeState, actionType));
		players[player]->addAction(action);

		B::iterator binding = bindings.find(key);
		if (binding == bindings.end()) // add binding for a new key
		{
			Actions al; // create dummy list to push to k
			al.push_back(action);
			bindings.insert(pair<int, Actions>(key, al));
		}
		else // or a binding for another state for an existing key
			binding->second.push_back(action);
	}

} // unnamed namespace

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

template <class D, class B>
void ActionQueue::queueAllHeldKeys(D keysDown, B bindings)
{
	Actions::iterator action;
	B::iterator b;
	for (D::const_iterator kd = keysDown.cbegin(); kd != keysDown.cend(); ++kd)
	{
		b = bindings.find(*kd);
		if (b == bindings.end())
			continue;
		for (action = b->second.begin(); action != b->second.end(); ++action) 
			if ((*action)->hasActiveStateOf(getState()))
				enqueue(*action, Action::STATE_HOLD);
	}
}

void ActionQueue::doAll()
{
	// repeated GLUT 'press' interrupts are ignored, so add held btns manually
	queueAllHeldKeys(keysDown, keyBindings);
	queueAllHeldKeys(specialKeysDown, specialKeyBindings);

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

void handleXboxInput(XboxController &player)
{
	if (!player.isConnected())
		return;
	const int pressed = player.GetState().Gamepad.wButtons;
	Action::ActionState actionState;
	XboxButtonBindings::const_iterator binding;
	XboxButton boundButton;
	XboxButtonsDown newXboxButtonsDown; // used to determine when btns are released

	// we're looking for bindings in an active state that match the buttons pressed
	for (binding = xboxButtonBindings.cbegin(); binding != xboxButtonBindings.cend(); ++binding)
	{
		boundButton = binding->first;
		if (!(pressed & boundButton))
			continue;

		if (xboxButtonsDown.find(boundButton) == xboxButtonsDown.cend())
			actionState = Action::STATE_PRESS;
		else
			actionState = Action::STATE_HOLD;

		for (Actions::const_iterator action = binding->second.begin(); action != binding->second.end(); ++action) 
		{
			if (!(*action)->hasActiveStateOf(getState()))
				continue;
			newXboxButtonsDown.insert(boundButton);
			actionQueue.enqueue(*action, actionState);
		}
	}

	// determine which buttons were released
	Actions::const_iterator action;
	for (XboxButtonsDown::const_iterator x = xboxButtonsDown.cbegin(); x != xboxButtonsDown.cend(); ++x)
	{
		if (newXboxButtonsDown.find(*x) != newXboxButtonsDown.end())
			continue;

		binding = xboxButtonBindings.find(*x);
		for (Actions::const_iterator action = binding->second.begin(); action != binding->second.end(); ++action) 
			actionQueue.enqueue(*action, Action::STATE_RELEASE);
	}
	xboxButtonsDown = newXboxButtonsDown;
}

void handleInput()
{
	handleXboxInput(xboxPlayer1);
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
	anyMouseMotion(mouseMotionFuncs, x, y);
}

// Activated when mouse moves while a mouse button ISN'T held down.
void mousePassiveMotion(int x, int y)
{
	anyMouseMotion(mousePassiveMotionFuncs, x, y);
}

void addMouseMotionFunc(void *classInstance, int activeState, void (*mouseMotion)(void *classInstance, int x, int y))
{
	addAnyMouseMotionFunc(mouseMotionFuncs, classInstance, activeState, mouseMotion);
}

void addMousePassiveMotionFunc(void *classInstance, int activeState, void (*mouseMotion)(void *classInstance, int x, int y))
{
	addAnyMouseMotionFunc(mousePassiveMotionFuncs, classInstance, activeState, mouseMotion);
}

void removeMouseMotions(void *classInstance)
{
	removeMouseMotionFuncs(mouseMotionFuncs, classInstance);
	removeMouseMotionFuncs(mousePassiveMotionFuncs, classInstance);
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

				// update bindings to account for this action
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

void bindKey(int player, Action::ActionScope scope, int activeState, int actionType, unsigned char key)
{
	bind(keyBindings, player, scope, activeState, actionType, key);
}

void bindSpecialKey(int player, Action::ActionScope scope, int activeState, int actionType, int key)
{
	bind(specialKeyBindings, player, scope, activeState, actionType, key);
}


void bindXboxButton(int player, Action::ActionScope scope, int activeState, int actionType, int key)
{
	bind(xboxButtonBindings, player, scope, activeState, actionType, key);
}

void bindButton(Action action, int button)
{
	//TODO
}

} // Input