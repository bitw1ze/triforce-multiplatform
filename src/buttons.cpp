#include "buttons.h"

Buttons::Buttons(int viewportWidth, int viewportHeight)
{
	curFrame = 0;
	r = 3;
	g = 7;
	b = 23;
	vpWidth = viewportWidth;
	vpHeight = viewportHeight;
}

Buttons::~Buttons()
{
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		delete *button;
}

void Buttons::add(string btnFiles[3], int xpos, int ypos)
{
	// load image
	const int frameCount = 3;
    CBaseSprite * sprite = new CBaseSprite(frameCount, vpWidth, vpHeight);
	for (int frame = 0; frame < frameCount; ++frame)
		sprite->loadFrame(frame, themeDirectory + btnFiles[frame], r, g, b);
	sprite->loadGLTextures();

	// create button obj and push to container
	Button *b = new Button(sprite, xpos, ypos);
	b->sprite = sprite; // create public alias
	buttons.push_back(b);
}

void Buttons::display()
{

	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		if ((*button)->enabled)
			(*button)->draw((*button)->getFrameNum());
}

Buttons::Button::Button(CBaseSprite * sprite, int xpos, int ypos)
{
	unpress();
	create(xpos, ypos, 0, 0, sprite);
}

void Buttons::Button::hover()
{
	pressing = false;
	hovering = true;
}

void Buttons::Button::press()
{
	pressing = true;
	hovering = false;
}

void Buttons::Button::unpress()
{
	pressing = false;
	hovering = false;
}

int Buttons::Button::getFrameNum()
{
	if (hovering)
		return 1;
	else if (pressing)
		return 2;
	return 0;
}

Buttons::Button::~Button()
{
	delete sprite;
}