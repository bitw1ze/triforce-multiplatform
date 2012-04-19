#pragma once

#include <list>
#include "gobjlib.h"
#include "globals.h"

using namespace GObjLib;

class Button;

class Buttons
{
protected:
	typedef list<Button *> buttons_t;
	buttons_t buttons;
public:
	~Buttons();
	void display();
	void add(Button *b);
};

class Button : public GObject
{
protected:
	bool pressed;
public:
	Button(CBaseSprite *sprite, int xpos = 0, int ypos = 0);
	void press();
	void unpress();
};