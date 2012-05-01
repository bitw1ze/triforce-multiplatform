#pragma once

/*
 *  
 */

#include "2DGraphics.h"
#include "globals.h"
#include <list>

class input
{
	private:
		list<unsigned char> keysDown;
		enum action {UP, RIGHT, LEFT, DOWN, ACTIVATE};

	public:
		void pressReleaseSpecial(int key, int x, int y);
		void press(unsigned int key, int x, int y);
		void release(unsigned int key, int x, int y);
};