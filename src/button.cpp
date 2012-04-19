#include "buttons.h"

void Buttons::display()
{

}

Button::Button(CBaseSprite *sprite, int xpos, int ypos)
{
	create(xpos, ypos, 0, 0, sprite);
}