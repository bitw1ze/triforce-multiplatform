#pragma once

#include <list>
#include "2DGraphics.h"
#include "globals.h"

using namespace Globals;
using namespace std;

class Buttons
{
protected:
	class Button : public CObject
	{
	public:
		bool hovering;
		bool pressing;

		// callback function to activate upon button press
		void * actionClassInstance;
		int actionArg;
		void (*action)(void *classInstance, int actionArg);

		Button(CBaseSprite *sprite, int xpos, int ypos);
		~Button();
		CBaseSprite * sprite; // a *public* alias to the sprite ptr used by CObject
		void unhover();
		void hover();
		void activate();
		void display();
		int getFrameNum() const;
	};

	typedef list<Button *> Btns_t;
	typedef Btns_t::iterator BtnIter_t;
	Btns_t buttons;
	BtnIter_t currentBtn; // button selected by keyboard (could be unhilighted and still active)

	int r, g, b, // KISS: same color filters to be used by every button
		vpWidth, vpHeight,
		curFrame, lastFrame;

public:
	Buttons(int viewportWidth, int viewportHeight);
	~Buttons();
	void display();
	void add(void *classInstance, int actionArg,
	         void (*action)(void *classInstance, int actionArg), 
 	         string btnFiles[3], int xpos = 0, int ypos = 0);
	void hoverPrev();
	void hoverNext();
	void unhoverAll();
	void unpressAll();
	void pressActive();
	Button * getBtnUnderCursor(int x, int y);
	void clickDown(int x, int y);
	void clickUp(int x, int y);
	void passiveMouseHover(int x, int y);
};

