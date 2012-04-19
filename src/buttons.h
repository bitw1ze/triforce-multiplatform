#pragma once

#include <list>
#include "gobjlib.h"
#include "globals.h"

using namespace GObjLib;
using namespace std;

class Buttons
{
protected:
	class Button : public GObject
	{
	protected:
		bool pressed;
	public:
		Button(CBaseSprite *sprite, int xpos, int ypos);
		//	void press();
		//	void unpress();
	};

	typedef list<Button *> Btns_t;
	typedef Btns_t::iterator BtnIter_t;
	Btns_t buttons;
	int curFrame, lastFrame;

public:
	Buttons();
	~Buttons();
	void display();
	void add(CBaseSprite * sprite, int xpos = 0, int ypos = 0);
};

