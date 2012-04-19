#include "buttons.h"

Buttons::~Buttons()
{
	for (buttons_t::iterator i = buttons.begin(); i != buttons.end(); i)
		delete *i;
}

void Buttons::display()
{

}

Button::Button(CBaseSprite *sprite, int xpos, int ypos)
{
	create(xpos, ypos, 0, 0, sprite);
}