#pragma once

#include <list>
#include "gobjlib.h"
#include "globals.h"

using namespace Globals;
using namespace GObjLib;
using namespace std;

class Buttons
{
protected:
	class Button : public GObject
	{
	protected:
		bool hovering;
		bool pressing;

	public:
		Button(CBaseSprite *sprite, int xpos, int ypos);
		~Button();
		CBaseSprite * sprite; // a *public* alias to the sprite ptr used by CObject
		void hover();
		void press();
		void unpress();
		void display();
		int getFrameNum();
	};

	typedef list<Button *> Btns_t;
	typedef Btns_t::iterator BtnIter_t;
	Btns_t buttons;

	int r, g, b, // KISS: same color filters to be used by every button
		vpWidth, vpHeight,
		curFrame, lastFrame;

public:
	Buttons(int viewportWidth, int viewportHeight);
	~Buttons();
	void display();
	void add(string btnFiles[3], int xpos = 0, int ypos = 0);
};

