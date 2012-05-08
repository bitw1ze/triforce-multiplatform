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
 *  have a static function that is called upon the start of the *program*,
 *  which registers every possible action on input that the class can perform.
 *  (this information will be used later to build the controller config
 *  dialogs). When any classes that need to take actions based on input
 *  are instantiated, their constructor will register a single callback
 *  function which handles all of its actions for that instance only. Other
 *  instances are used for other players. For example: if two cursors are
 *  instantiated, the first one is for the first players control, the second
 *  one is for the second players control, etc.
 *
 *  When Input receives mouse/keyboard/controller/etc. input, it
 *  calls the appropriate callback function, which depends upon the state of
 *  Triforce. The callback func is passed an action, such as UP or SWAP,
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
 *  key is released. This is handled by simply passing the state of the input
 *  to the action function (i.e. press, hold, release). The hold state provides
 *  the option of Input handling button repeats, but it still allows the
 *  action function to handle it, since it calls "press" first, then "hold"
 *  if the button has fired again but not been released, and "release" once
 *  done.
 * 
 *  Implemetation:
 *   
 *  To control the repeat rate, an Input function will have to be inserted into
 *  the display() loop, which will fire the action again if the key has not
 *  been released within a certain number of ms.
 * 
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

namespace Input
{

	/**
	 *  Each instance of any class that takes actions on input has
	 *   Action object entries. Each entry points to the action() function
	 *   for that class.
	 */
	class Action
	{
	public:
		enum ActionState {STATE_PRESS, STATE_HOLD, STATE_RELEASE};
		enum ActionScope {SCOPE_FIRST_PLAYER, SCOPE_CURRENT_PLAYER, SCOPE_ALL_PLAYERS};
		typedef void (*ActionFunc)(void *actionsClassInstance,
								   int actionState, int actionType);
	private:
		ActionScope scope;
		int activeState; // action is only active this matches getState()
		int actionType; // type of action, which is passed as an arg to the action func
		string shortDesc; // 1-2 word description of what action does

		void *actionsClassInstance; // class instance that action belongs to

		// callback of registered action function
		ActionFunc action;
	public:
		// declare an action with no definition (no action function attached)
		Action(ActionScope scope, int activeState, int actionType, string shortDesc) :
		  scope(scope), activeState(activeState), actionType(actionType), shortDesc(shortDesc){}
		bool isSameAction(ActionScope scope, int activeState, int actionType);
		bool isRelatedAction(ActionScope scope, int activeState);
		void doAction(int actionState);

#if 0
		// declare/define an action function (with an action function attached)
		Action(int activeState, int actionType, string shortDesc,
			void *actionsClassInstance, ActionFunc action) :
			activeState(activeState), actionType(actionType), shortDesc(shortDesc),
			actionsClassInstance(actionsClassInstance), action(action){}
#endif

	    void define(void *actionsClassInstance, ActionFunc);
	};

	class Player
	{
	private:
		list<Action *> actions;
	public:
		void addAction(Action *action);
		bool isActionDefined(Action::ActionScope scope, int activeState, int actionType); // should have 1
		bool hasActionsDefined(Action::ActionScope scope, int activeState); // may have >1
		Action *getAction(Action::ActionScope scope, int activeState, int actionType);
	};


	// getStateFunc returns state of program, and only uses actions with the same state
	void setGSFunc(int (*getStateFunc)()); 
	void setGSLabels(const string *labels); 

	void addPlayer();

	/**
	 * Button/key Inputs
	 */

	void keyPress(unsigned char key, int x, int y);
	void keyRelease(unsigned char key, int x, int y);
	void keySpecialPress(int key, int x, int y);
	void keySpecialRelease(int key, int x, int y);
	void mousePress(int button, int mouseState, int x, int y);

	/**
	 * Motion Inputs
	 */

	// The x/y args will be passed to the member functions of the other classes
	// to be handled entirely; Input is only choosing which function to send to
    // based on the state. It is left up to the caller to ensure that no two
	// motion functions handle the same x/y area. It could be handled here,
	// but in practice it shouldn't be a problem, and it adds a little more
	// flexibility this way.

	void mouseMotion(int x, int y);
	void mousePassiveMotion(int x, int y);

	void addMouseMotionFunc(void *classInstance, int activeState, void (*mouseMotion)(void *classInstance, int x, int y));
	void addMousePassiveMotionFunc(void *classInstance, int activeState, void (*mouseMotion)(void *classInstance, int x, int y));
	void removeMotions(void *classInstance);

	/**
	 * Actions
	 */

	// determine actions program supports before instantiation of objects that use actions
	void declareAction(Action::ActionScope scope, int activeState, int actionType, string shortDesc); 
	// define an action function for a particular action
	void defineAction(Action::ActionScope scope, int activeState, int actionType, void *classInstance, Action::ActionFunc action);
	// convenience function; define every action for a given scope/state at once
	void defineActions(Action::ActionScope scope, int activeState, void *classInstance, Action::ActionFunc action);
	void removeActions(void *classInstance);
	void setActionLabels(int activeState, const string *labels);

	/**
	 * Binding
	 */

	// the Triforce constructor binds the default keys for the entire game
	// TODO: Eventually, a class that loads config files will handle overriding these bindings.
	void bindKey(Action action, unsigned char key);
	void bindKey(int player, Action::ActionScope scope, int activeState, int actionType, unsigned char key);
	void bindSpecialKey(Action action, int key);
	void bindSpecialKey(int player, Action::ActionScope scope, int activeState, int actionType, int key);
	void bindButton(Action action, int button);

} //Input