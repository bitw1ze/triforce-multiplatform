#include "buttons.h"

Buttons::Buttons()
{
	curFrame = 0;
	lastFrame = 1;
}

Buttons::~Buttons()
{
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		delete *button;
}

void Buttons::add(CBaseSprite * sprite, int xpos, int ypos)
{
	buttons.push_back(new Button(sprite, xpos, ypos));
}

void Buttons::display()
{
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		if ((*button)->enabled)
			(*button)->draw(curFrame);
	if (++curFrame > lastFrame)
		curFrame = 0;
}

Buttons::Button::Button(CBaseSprite * sprite, int xpos, int ypos)
{
	pressed = false;
	create(xpos, ypos, 0, 0, sprite);
}
