#include "input.h"

int (* Input::getState)() = NULL;

void Input::doAction(int actionType)
{
	int activeState = getState();
	// FIXME: incomplete
}

void Input::keySpecialPress(int key, int x, int y)
{
	
}

void Input::keySpecialRelease(int key, int x, int y)
{
}

void Input::keyPress(unsigned char key, int x, int y)
{
}

void Input::keyRelease(unsigned char key, int x, int y)
{
}

void Input::mousePress(int button, int mouseState, int x, int y)
{
}

/**
 *  Activated when mouse moves while a mouse button IS held down.
 */
void Input::mouseMove(int x, int y)
{
}

/**
 *  Activated when mouse moves while a mouse button ISN'T held down.
 */
void Input::mousePassiveMove(int x, int y)
{
}