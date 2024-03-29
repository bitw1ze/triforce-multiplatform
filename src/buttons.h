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
		bool enabled;

		Button(CBaseSprite *sprite, int xpos, int ypos);
		void unhover();
		void hover();
		void activate();
		void display();
		int getFrameNum() const;

		// callback function to activate upon button press
		void * actionClassInstance;
		int actionArg;
		void (*action)(void *classInstance, int actionArg);
		void disable() {enabled = hovering = pressing = false;}
		void enable() {enabled = true;}
	};

	typedef list<Button *> Btns_t;
	typedef Btns_t::iterator BtnIter_t;
	Btns_t buttons;

	// track button selected by keyboard (could be unhilighted and still active)
	BtnIter_t currentBtn; 

	int r, g, b, // KISS: same color filters to be used by every button
		vpWidth, vpHeight,
		curFrame, lastFrame;

public:
	static void mousePassiveMotion(void *buttonsInstance, int x, int y);

	Buttons(int viewportWidth, int viewportHeight);
	~Buttons();
	void display();
	void add(void *classInstance, int actionArg,
	         void (*action)(void *classInstance, int actionArg), 
 	         const string btnFiles[3], int xpos = 0, int ypos = 0);
	void unhoverAll();
	void unpressAll();
	void enable(int actionArg);
	void disable(int actionArg);
	bool areAllDisabled();

	//keyboard
	void hoverPrev();
	void hoverNext();
	void pressCurrent();
	void unpressCurrent();
	void activateCurrent();

	// mouse
	Button * getBtnUnderCursor(int x, int y);
	void clickDown(int x, int y);
	void clickUp(int x, int y);
};

