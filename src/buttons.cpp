#include "buttons.h"

Buttons::Buttons(int viewportWidth, int viewportHeight) {
	curFrame = 0;
	r = 3;
	g = 7;
	b = 23;
	vpWidth = viewportWidth;
	vpHeight = viewportHeight;
}

Buttons::~Buttons() {
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		delete *button;
}

void Buttons::display() {
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		//if ((*button)->getState() == Block::enabled)
			(*button)->draw((*button)->getFrameNum());
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

	// create button obj
	Button *b = new Button(sprite, xpos, ypos);
	b->sprite = sprite; // create public alias
	b->actionClassInstance = classInstance; // set the callback function and its args
	b->action = action;
	b->actionArg = actionArg;

	// push button to container, but hover on the first button added
	if (buttons.empty())
	{
		b->hover();
		buttons.push_back(b);
		activeBtn = buttons.begin();
	}
	else
		buttons.push_back(b);
}

void Buttons::hoverPrev() {
	if (!(*activeBtn)->hovering) // currently hovering button is better than activeBtn
		for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
			if ((*button)->hovering)
				activeBtn = button;
	if (activeBtn == buttons.begin())
		activeBtn = buttons.end();
	--activeBtn;
	unhoverAll();
	(*activeBtn)->hover();
}

void Buttons::hoverNext() {
	if (!(*activeBtn)->hovering) // currently hovering button is better than activeBtn
		for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
			if ((*button)->hovering)
				activeBtn = button;
	++activeBtn;
	if (activeBtn == buttons.end())
		activeBtn = buttons.begin();
	unhoverAll();
	(*activeBtn)->hover();
}

void Buttons::unhoverAll() {
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		(*button)->unhover();
}

void Buttons::unpressAll() {
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		(*button)->pressing = false;
}

void Buttons::pressActive() {
	if ((*activeBtn)->hovering)
		(*activeBtn)->press();
	else
		for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
//			if ((*button)->enabled && (*button)->hovering)
				(*button)->press();
}

Buttons::Button * Buttons::getBtnUnderCursor(int x, int y) {
	float minX, minY, maxX, maxY;
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		//if ((*button)->enabled)
		{
			(*button)->Getxy(minX, minY);
			maxX = minX + (*button)->sprite->GetWidth();
			maxY = minY + (*button)->sprite->GetHeight();
			if (x >= minX && x <= maxX && y >= minY && y <= maxY)
				return (*button);
		}
	return (Button *)0;
}

void Buttons::clickDown(int x, int y) {
	Button * button = getBtnUnderCursor(x, y);
	if (button)
		button->pressing = true;
}

void Buttons::clickUp(int x, int y) {
	// disable pressing for all, since we don't necessarily know which was clicked down on
	unpressAll();
	Button * button = getBtnUnderCursor(x, y);
	if (button)
		button->press(); // mouse is still on btn; take action
}

void Buttons::passiveMouseHover(int x, int y) {
	float minX, minY, maxX, maxY;
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		//if ((*button)->enabled)
		{
			(*button)->Getxy(minX, minY);
			maxX = minX + (*button)->sprite->GetWidth();
			maxY = minY + (*button)->sprite->GetHeight();
			if (x >= minX && x <= maxX && y >= minY && y <= maxY)
			{
				unhoverAll();
				(*button)->hover();
			}
		}
}

Buttons::Button::Button(CBaseSprite * sprite, int xpos, int ypos) {
	pressing = false;
	hovering = false;
	init(sprite, xpos, ypos, 0, 0, NULL);
}

void Buttons::Button::unhover() {
	hovering = false;
}

void Buttons::Button::hover() {
	hovering = true;
}

void Buttons::Button::press() {
	action(actionClassInstance, actionArg);
}

int Buttons::Button::getFrameNum() const {
	if (pressing) // pressing state takes precedence
		return 2;
	else if (hovering)
		return 1;
	return 0;
}

Buttons::Button::~Button() {
	delete sprite;
}