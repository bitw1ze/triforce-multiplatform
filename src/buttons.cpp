#include "buttons.h"

Buttons::Buttons(int viewportWidth, int viewportHeight) {
	curFrame = 0;

	r = 254;
	g = 0;
	b = 254;
	vpWidth = viewportWidth;
	vpHeight = viewportHeight;
}

Buttons::~Buttons() {
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		delete *button;
}

void Buttons::display() {
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
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
			      const string btnFiles[3], int xpos, int ypos)
{
	string cwd = themeDirectory + "buttons\\";
	// load image
	const int frameCount = 3;
    CBaseSprite * sprite = new CBaseSprite(frameCount, vpWidth, vpHeight);
	for (int frame = 0; frame < frameCount; ++frame)
		sprite->loadFrame(frame, cwd + btnFiles[frame], r, g, b);
	sprite->loadGLTextures();

	// create button obj
	Button *b = new Button(sprite, xpos, ypos);
	b->actionClassInstance = classInstance; // set the callback function and its args
	b->action = action;
	b->actionArg = actionArg;

	// push button to container, but hover on the first button added
	if (buttons.empty())
	{
		b->hover();
		buttons.push_back(b);
		currentBtn = buttons.begin();
	}
	else
		buttons.push_back(b);
}

void Buttons::hoverPrev() {
	if (!(*currentBtn)->hovering) // prefer to use a hovering button over the currentBtn
		for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
			if ((*button)->hovering)
				currentBtn = button;
	if (currentBtn == buttons.begin())
		currentBtn = buttons.end();
	--currentBtn;
	unhoverAll();
	unpressAll();
	(*currentBtn)->hover();
}

void Buttons::hoverNext() {
	if (!(*currentBtn)->hovering) // prefer to use a hovering button over the currentBtn
		for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
			if ((*button)->hovering)
				currentBtn = button;
	++currentBtn;
	if (currentBtn == buttons.end())
		currentBtn = buttons.begin();
	unhoverAll();
	unpressAll();
	(*currentBtn)->hover();
}

void Buttons::pressCurrent() {
	if ((*currentBtn)->hovering)
		(*currentBtn)->pressing = true;
}

void Buttons::unpressCurrent() {
	(*currentBtn)->pressing = false;
}

void Buttons::unhoverAll() {
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		(*button)->unhover();
}

void Buttons::unpressAll() {
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		(*button)->pressing = false;
}

void Buttons::activateCurrent() {
	if ((*currentBtn)->hovering && (*currentBtn)->pressing)
		(*currentBtn)->activate();
	else
		for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
			if ((*button)->hovering && (*button)->pressing)
				(*button)->activate();
	unpressCurrent();
}

Buttons::Button * Buttons::getBtnUnderCursor(int x, int y) {
	float minX, minY, maxX, maxY;
	CBaseSprite * sprite;
	for (BtnIter_t button = buttons.begin(); button != buttons.end(); ++button)
		{
			(*button)->Getxy(minX, minY);
			sprite = (*button)->getSprite();
			maxX = minX + sprite->GetWidth();
			maxY = minY + sprite->GetHeight();
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
	Button * button = getBtnUnderCursor(x, y);
	if (button && button->pressing)
		button->activate(); // mouse is still on btn; take action
	/* Disable pressing for all, since we don't necessarily know which was clicked
	 * down on. This is because the button clicked down on can be different than the
	 * one released on.
	 */
	unpressAll();
}

void Buttons::mousePassiveMotion(void *buttonsInstance, int x, int y) {
	Buttons * b = (Buttons *)buttonsInstance;
	Button * button = b->getBtnUnderCursor(x, y);
	b->unhoverAll();
	if (button)
		button->hover();
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

void Buttons::Button::activate() {
	action(actionClassInstance, actionArg);
}

int Buttons::Button::getFrameNum() const {
	if (pressing) // pressing state takes precedence over hovering
		return 2;
	else if (hovering)
		return 1;
	return 0;
}