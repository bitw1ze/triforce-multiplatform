#include "game.h"
#include "input.h"

void Cursor::declareActions()
{
	using namespace Input;
	using namespace PlayState;

	Action::ActionScope scope = Action::SCOPE_FIRST_PLAYER;
	Triforce::GameState state = Triforce::PLAY;

	declareAction(scope, state, UP, actionLabels[UP]);
	declareAction(scope, state, DOWN, actionLabels[DOWN]);
	declareAction(scope, state, LEFT, actionLabels[LEFT]);
	declareAction(scope, state, RIGHT, actionLabels[RIGHT]);
}

void Cursor::defineActions()
{
	using namespace Input;
	using namespace PlayState;

	Action::ActionScope scope = Action::SCOPE_FIRST_PLAYER;
	Triforce::GameState state = Triforce::PLAY;

	addMouseMotionFunc(this, state, mousePassiveMotion);
	addMousePassiveMotionFunc(this, state, mousePassiveMotion);
	defineAction(scope, state, UP, this, doAction);
	defineAction(scope, state, DOWN, this, doAction);
	defineAction(scope, state, LEFT, this, doAction);
	defineAction(scope, state, RIGHT, this, doAction);
}

void Cursor::doAction(void *cursorInstance, int actionState, int actionType)
{
	using namespace PlayState;

	Cursor *c = (Cursor *)cursorInstance;
	switch((enum Input::Action::ActionState)actionState)
	{
	case Input::Action::STATE_PRESS:
	//case Input::Action::STATE_HOLD:
		switch((enum Actions)actionType)
		{
		case UP:
			c->moveUp();
			break;
		case DOWN:
			c->moveDown();
			break;
		case LEFT:
			c->moveLeft();
			break;
		case RIGHT:
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
	lastMousePos.x = lastMousePos.y = 0;
	isAlignedToMouse = false;

	defineActions();
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
	if (getY() + 2 * GamePlay::blockLength <= grid->getY()) {
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

void Cursor::alignToMouse() {
	const float x_threshold = 0.49;
	if (!grid->containsPoint(lastMousePos) || isAlignedToMouse)
		return;

	if (getX() + 2*GamePlay::blockLength - x_threshold * GamePlay::blockLength < lastMousePos.x && col < ncols - 2)
		moveRight();
	else if (lastMousePos.x - x_threshold*GamePlay::blockLength < getX() && col > 0)
		moveLeft();
	else if (getY() > lastMousePos.y && row < nrows)
	    moveUp();
	else if (lastMousePos.y > getY() + GamePlay::blockLength && row > 0)
		moveDown(); 
}

void Cursor::mousePassiveMotion(void *cursorInstance, int x, int y) {
	Cursor *c = (Cursor *)cursorInstance;

	if (!c->grid->containsPoint(x, y))
		glutSetCursor(GLUT_CURSOR_INHERIT);
	else
		glutSetCursor(GLUT_CURSOR_LEFT_SIDE);

	if (c->lastMousePos.x == x && c->lastMousePos.y == y)
		c->isAlignedToMouse = true;
	else
	{
		c->isAlignedToMouse = false;
		c->lastMousePos.x = x;
		c->lastMousePos.y = y;
	}
}