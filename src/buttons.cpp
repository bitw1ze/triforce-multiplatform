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

/**
 * Uses for each element of btnFiles:
 *     regular: btnFiles[0]
 *     hover: btnFiles[1]
 *     pressed: btnFiles[2]
 */
void Buttons::add(void *classInstance, int actionArg,
				  void (*action)(void *classInstance, int actionArg), 
			      string btnFiles[3], int xpos, int ypos)
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
	b->actionClassInstance; // set the callback function and its args
	b->action = action;
	b->actionArg = actionArg;

	// hover on the first button added
	if (buttons.empty())
	{
		b->hover();
		buttons.push_back(b);
		activeBtn = buttons.begin();
	}
	else
		buttons.push_back(b);
}

void Buttons::hoverNext()
{
	(*activeBtn)->unhover();
	if (activeBtn == buttons.end())
		activeBtn = buttons.begin();
	else
		++activeBtn;
	(*activeBtn)->hover();
}

void Buttons::pressActive()
{
	(*activeBtn)->press();
}

void Buttons::display()
{
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		if ((*button)->enabled)
			(*button)->draw((*button)->getFrameNum());
}

Buttons::Button::Button(CBaseSprite * sprite, int xpos, int ypos)
{
	pressing = false;
	hovering = false;
	create(xpos, ypos, 0, 0, sprite);
}

void Buttons::Button::unhover()
{
	hovering = false;
}

void Buttons::Button::hover()
{
	hovering = true;
}

void Buttons::Button::press()
{
	pressing = true;
}

void Buttons::Button::unpress()
{
	// activate button once it has been pressed/released and is still active
	if (pressing && hovering)
		action(actionClassInstance, actionArg);

	pressing = false;
}

int Buttons::Button::getFrameNum() const
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