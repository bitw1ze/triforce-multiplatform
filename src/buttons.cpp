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

void Buttons::add(string btnFile, int xpos, int ypos)
{
	// load image
	int frameCount = 1, frame = 0;
    CBaseSprite * sprite = new CBaseSprite(frameCount, vpWidth, vpHeight);
    sprite->loadFrame(frame, themeDirectory + btnFile, r, g, b);
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
			(*button)->draw(curFrame);
	if (++curFrame > lastFrame)
		curFrame = 0;
}

Buttons::Button::~Button()
{
	delete sprite;
}

Buttons::Button::Button(CBaseSprite * sprite, int xpos, int ypos)
{
	pressed = false;
	create(xpos, ypos, 0, 0, sprite);
}