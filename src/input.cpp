#include "input.h"

namespace Input
{

namespace
{
	int (*getState)() = NULL; // used to determine which actions are currently valid for Triforce
	list<void (*)(int x, int y)> mouseMotionFuncs;
	list<void (*)(int x, int y)> mousePassiveMotionFuncs;
	list<Action> actions;

	// keys that are currently being held down
	list<unsigned char> keysDown;
	list<unsigned char> keysSpecialDown;
} // unnamed

void setGSFunc(int (*getStateFunc)())
{
	getState = getStateFunc;
}

void registerAction(void *classInstance, void (*action)(void *, int),
                    int actionType, string shortDesc)
{
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
void mouseMove(int x, int y)
{
}

/**
 *  Activated when mouse moves while a mouse button ISN'T held down.
 */
void mousePassiveMove(int x, int y)
{
}

} // Input