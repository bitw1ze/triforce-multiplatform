#pragma once

/*
 *  
 */

#include "2DGraphics.h"
#include "globals.h"
#include <list>

class Input
{
private:
	list<unsigned char> keysDown;
	list<unsigned char> keysSpecialDown;
	enum action {UP, RIGHT, LEFT, DOWN, ACTIVATE};

public:
	void keyPress(unsigned char key, int x, int y);
	void keyRelease(unsigned char key, int x, int y);

	void keySpecialPress(int key, int x, int y);
	void keySpecialRelease(int key, int x, int y);

	void mousePress(int button, int mouseState, int x, int y);
	void mouseMove(int x, int y);
	void mousePassiveMove(int x, int y);
};