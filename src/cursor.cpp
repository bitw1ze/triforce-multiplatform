#include "game.h"
#include "input.h"

const string Cursor::actionLabels[Cursor::_NUMBER_OF_ACTIONS] = {
	"Up",
	"Down",
    "Left",
    "Right"};

void Cursor::declareActions()
{
	Input::Action::ActionScope scope = Input::Action::SCOPE_FIRST_PLAYER;
	// FIXME: I have a hunch that SCOPE_CURRENT_PLAYER stuff is broken, so
    //	      comment it out and use SCOPE_FIRST_PLAYER for now
	//Input::Action::ActionScope scope = Input::Action::SCOPE_CURRENT_PLAYER;
	Triforce::GameState state = Triforce::PLAY;

	Input::declareAction(scope, state, ACT_UP, actionLabels[ACT_UP]);
	Input::declareAction(scope, state, ACT_DOWN, actionLabels[ACT_DOWN]);
	Input::declareAction(scope, state, ACT_LEFT, actionLabels[ACT_LEFT]);
	Input::declareAction(scope, state, ACT_RIGHT, actionLabels[ACT_RIGHT]);
}

void Cursor::doAction(void *cursorInstance, int actionState, int actionType)
{
	Cursor *c = (Cursor *)cursorInstance;
	switch((enum Input::Action::ActionState)actionState)
	{
	case Input::Action::STATE_PRESS:
	case Input::Action::STATE_HOLD:
		switch((enum Actions)actionType)
		{
		case ACT_UP:
			c->moveUp();
			break;
		case ACT_DOWN:
			c->moveDown();
			break;
		case ACT_LEFT:
			c->moveLeft();
			break;
		case ACT_RIGHT:
			c->moveRight();
			break;
		}
	// case Input::Action::STATE_RELEASE:
	}
}

Cursor::Cursor(Grid *gr, CBaseSprite *sprite) {
	grid = gr;

	cursor_delta = sprite->GetHeight();
	row = nrows / 2;
	col = ncols / 2;
	init(sprite);
	setPos(0, 0);

	Input::addMouseMotionFunc(this, Triforce::PLAY, mousePassiveMotion);
	Input::addMousePassiveMotionFunc(this, Triforce::PLAY, mousePassiveMotion);
	//Input::defineActions(Input::Action::SCOPE_CURRENT_PLAYER, Triforce::PLAY, this, doAction);
	Input::defineActions(Input::Action::SCOPE_FIRST_PLAYER, Triforce::PLAY, this, doAction);
}

Cursor::~Cursor() {
	Input::removeMotions(this);
}

bool Cursor::moveLeft(bool doDraw) {
	if (col > 0) {
		--col;
		offsetX( -cursor_delta );
		if (doDraw)
			draw(0);
		return true;
	}
	return false;
}

bool Cursor::moveRight(bool doDraw) {
	if (col < ncols - 2) {
		++col;
		offsetX( cursor_delta );
		if (doDraw)
			draw(0);
		return true;
	}
	return false;
}

bool Cursor::moveDown(bool doDraw) {
	if (getY() + 2*grid->getBlockHeight() <= grid->getY()) {
		--row;
		offsetY( cursor_delta );
		if (doDraw)
			draw(0);
		return true;
	}
	return false;
}

bool Cursor::moveUp(bool doDraw) {
	if (row < nrows) {
		++row;
		offsetY( -cursor_delta );
		if (doDraw)
			draw(0);
		return true;
	}
	return false;
}

void Cursor::setPos(int c, int r) {
	row = r;
	col = c;
	Setxy( grid->getX() + col * cursor_delta, grid->getY() - (row * cursor_delta));
}

void Cursor::shiftRow() {
	if (row < nrows - 1)
		++row;
	else if (row == nrows - 1)
		setPos(getCol(), nrows - 1);
	else
		setPos(getCol(), nrows);
}

void Cursor::mousePassiveMotion(void *cursorInstance, int x, int y) {
	Cursor *c = (Cursor *)cursorInstance;
	const float x_threshold = 0.49;

	if (!c->grid->containsPoint(x, y))
	{
		glutSetCursor(GLUT_CURSOR_INHERIT);
		return;
	}

	// move cursor inside grid
	glutSetCursor(GLUT_CURSOR_LEFT_SIDE);
	if (c->getX() + 2*c->grid->getBlockWidth() - x_threshold*c->grid->getBlockWidth() < x && c->col < ncols - 2)
		c->moveRight();
	else if (x - x_threshold*c->grid->getBlockWidth() < c->getX() && c->col > 0)
		c->moveLeft();
	else if (c->getY() > y && c->row < nrows)
	    c->moveUp();
	else if (y > c->getY() + c->grid->getBlockHeight() && c->row > 0)
		c->moveDown(); 
}