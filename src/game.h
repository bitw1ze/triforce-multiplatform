/*	game.h 

	Contains headers for all classes that are a part of actualy gameplay.
*/

#pragma once

#include <string>
#include <sstream>
#include <list>
#include <ctime>
#include <deque>
#include <vector>
#include "2DGraphics.h"
#include "globals.h"
#include "buttons.h"

using namespace std;
using namespace Globals;

class Cell;
class Combo;
class FallNode;
class Block;
class Grid;
class Cursor;
class GamePlay;
class HUD;

extern CTimer *mainTimer;

namespace PlayState {
	enum Actions {
		UP, DOWN, LEFT, RIGHT,
		SWAP, PUSH, PAUSE_TOGGLE,
		RETURN,
		_NUMBER_OF_ACTIONS
	};
    static const string actionLabels[_NUMBER_OF_ACTIONS] = {
		"Up", "Down", "Left", "Right",
		"Swap", "Push", "Pause toggle",
		"Return"
	};
}

struct Point {
	int x, y;
};

class Cell {
public:
	Cell() { row = 0; col = 0; }
	Cell(const Cell &src) { row = src.row; col = src.col; }
	Cell(int r, int c, bool e = true) { row = r, col = c; }
	friend bool operator ==(const Cell left, const Cell right) { 
		return left.row == right.row && left.col == right.col;
	}
	int row, col;
};

class Bonus : public CObject {
public:
	typedef enum { CHAIN, COMBO } BonusType;
	static const uint64 moveInterval;
	static const int moveSpeed, totalMove;
	CObject text;

	BonusType bonusType;
	int count;
	int row, col;
	uint64 lastMove;
	int offset;
	char str[5];

public:
	Bonus(const Cell &cell, int cnt, BonusType bt, Grid &g);
	Bonus(const Bonus &);
	void set(const Cell &cell, int cnt, BonusType bt, Grid &g);
	void clone(const Bonus &);
	virtual ~Bonus();

	bool update();
	void display();
};

class Combo {
public: 
	static int comboInterval;
	enum comboState { NONE, HORI, VERT, MULTI };
protected:
	Cell *_left, *_right, *_up, *_down, *_mid;
	int interval;
	uint64 startTime;
	comboState state;
	int chainCount;

public:
	Combo(int chains = 1);
	Combo(const Combo &combo);
	Combo & operator =(const Combo &combo);
	bool operator ==(const Combo &combo);
	void set(int chains);
	void clone(const Combo &combo);
	void init(Grid &grid);
	operator bool () const;

	Cell *left() const { return _left; }
	Cell *left(int r, int c);
	Cell *right() const { return _right; }
	Cell *right(int r, int c);
	Cell *down() const { return _down; }
	Cell *down(int r, int c);
	Cell *up() const { return _up; }
	Cell *up(int r, int c);
	Cell *mid() const { return _mid; }
	Cell *mid(int r, int c);

	comboState getState() const { return state; }
	void changeState(comboState st) { state = st; }

	const list<Cell> getList();

	int count() const;
	int getChainCount() const { return chainCount; }
	void activate();
	bool elapsed() const;
	bool update(Grid &grid);

	void printDebug();
};

class FallNode : public Cell {
public:
	uint64 lastFall;
	int numFalls;
	bool enabled;
	int chainCount;

public:
	FallNode();
	FallNode(int r, int c);
	FallNode(const FallNode &src);
	FallNode & operator =(const FallNode &src);
	void clone(const FallNode &src);
	void set();

	void init(Grid &grid, int chains);
	void cleanup(Grid &grid);
	bool update(Grid &grid);

	int getChainCount() const { return chainCount; };
};

class Fall : public list<FallNode> {
protected:
	int chainCount;

public:
	static int fallInterval;
	bool possibleChain;  // FIXME: put in protected section

	Fall(int chains = 1);
	Fall(const Cell &, int chains = 0);
	Fall(const list<FallNode> &, int chains = 1);
	void set(int chains);
	void clone(const Fall &src);
	Fall & operator =(const Fall &src);

	void adjustRow();
	bool isEnabled();
	void enable();

	void init(Grid &grid);
	void cleanup(Grid &grid);
	bool update(Grid &grid);

	int getChainCount() const { return chainCount; }
};

class Block : public CObject {
public: enum gameState { inactive, enabled, disabled, combo, fall };
protected:
	gameState state;
	int fallOffset;
	uint64 lastFrameTransition;
	static const uint64 frameTransitionInterval;
	int currentFrame;

public:
	Block();
	Block(const Block &block);
	Block & operator =(const Block &block);
	void set();
	void clone(const Block &src);
	~Block() {}

	friend bool swap(Block &left, Block &right);
	friend bool match(const Block &left, const Block &right, bool ignoreActive = false);

	void display();
	void composeFrame();
	void changeState(gameState gs);
	gameState getState() const { return state; }
	
	void fallDown();
	void resetFall();
	int getFallOffset() const;
	void setFallOffset(int f);

	static const int fallFactor;
};

class GamePlay {
public:
	enum gameState {play, pause, quit};
	static int blockLength;
	static int gridHeight;
	static int gridWidth;
	static void *font1;
	static float fcolor1[3], fcolor2[3];
	static const string gridBorderFile, bgFile, bonusFile;
	static CBaseSprite *blockSprites[nblocktypes], *cursorSprite, *gridBorderSprite, 
		*bonusSprite, *chainFontSprite, *comboFontSprite;
	// FIXME: once mouse buttons are working in Input, this should be protected
	Buttons * menuButtons;
	Grid *grid;
protected:
	/* bitmap files */
	static const int numCursorFiles;

	/* frame stuff */
	int current_frame;
	uint64 last_time;

	static BMPClass background;

	gameState state;
	HUD *hud;
	
public:
	static void declareActions();
	void defineActions();
	static void doAction(void *gridInstance, int actionState, int actionType);

	GamePlay();
	~GamePlay();
	void display();
	void composeFrame();
	void init();
	void processFrame();
	void loadImages();

	static int getWidth() { return background.getViewportWidth();} 
	static int getHeight() { return background.getViewportHeight();} 

	void changeState(gameState gs);
	static void changeStateWrapper(void *gamePlayInstance, int state);
	gameState getState() const;
};

/* Grid class abstracts all the operations on grid of blocks for a 
   single player */

class Grid {

public:
	Cursor *cursor;
	deque< vector<Block> > blocks; //i.e. blocks[row][col]
	enum gameState { play, combo, push };
	static const int forcedPushSpeed, forcedPushinterval;
protected:
	int	pushOffset, pushSpeed, pushInterval, pushAccelInterval,
		comboInterval,
		current_cursor_frame;
	float pushAccel;
	uint64 last_cursor_anim, timer_cursor_anim;
	uint64 lastPush, lastPushAccel, lastForcedPush;
	Point gridPos;
	CBaseSprite** blockSprites;
	gameState state;
	
	list<Combo> comboEvents;
	list<Fall> fallEvents;	
	list<Bonus> bonuses;

public:
	Grid();
	Grid(const Grid &g);
	void set();
	void init();
	void clone(const Grid &g);
	Grid & operator =(const Grid &g);
	virtual ~Grid();

	static void declareActions();
	void defineActions();
	static void doAction(void *gridInstance, int actionState, int actionType);

	void loadImages();
	void composeFrame();
	void display();
	void displayBonus();
	void updateEvents();

	void pushRow(int speed);
	void addRow();
	void swapBlocks();

	void changeState(gameState gs);
	gameState getState() const { return state; }

	/* set/get properties */
	int getYOffset() { return pushOffset; }
	int getX() { return gridPos.x; }
	int getY() { return gridPos.y; }
	int countEnabledRows() const;

	int downMatch(int r, int c, bool ignoreActive = false);
	int upMatch(int r, int c, bool ignoreActive = false);
	int leftMatch(int r, int c, bool ignoreActive = false);
	int rightMatch(int r, int c, bool ignoreActive = false);

	void incComboInterval(int interval);
	bool containsPoint(int x, int y);
	bool containsPoint(Point point);
	
	bool detectFall(const Combo & combo);
	bool detectFall(int r, int c, bool initialize = true);

	Combo detectCombo(Cell &cell, int chains, bool initialize);
	void initCombo(Combo &combo);
	void initBonus(const Combo &combo);

	void setBlockStates(list<Cell> &, Block::gameState gs);

	void printDebug();
};

/* The Cursor class controls the operations on the player's cursor, like moving
   it around. */

class Cursor : public CObject {
protected:
	int row, col,
		cursor_delta;
	Point lastMousePos;
	Grid *grid;
	static const string spriteFile;

	// for action holding
	static uint64 cursorMoveDelay, cursorMoveInitialDelay;
	uint64 cursorMoveInitial[PlayState::_NUMBER_OF_ACTIONS],
		   cursorMoveLast[PlayState::_NUMBER_OF_ACTIONS];

public:
	static void declareActions();
	void defineActions();
	void doActionPress(PlayState::Actions action);
	void doActionHold(PlayState::Actions action);
	static void doAction(void *cursorInstance, int actionState, int actionType);
	void alignToMouse();
	bool isAlignedToMouse;
	static void mousePassiveMotion(void *cursorInstance, int x, int y);

	Cursor(Grid *, CBaseSprite *);
	~Cursor();
	bool move(PlayState::Actions action, bool doDraw=true);
	bool moveUp(bool doDraw=true);
	bool moveDown(bool doDraw=true);
	bool moveLeft(bool doDraw=true);
	bool moveRight(bool doDraw=true);
	void setPos(int c, int r);
	int getRow() const { return row; }
	int getCol() const { return col; }
	void shiftRow();

	Point getMousePos() {return lastMousePos;}
};

class HUD {
protected:
	uint64 startTime;
	uint64 currentTime;
	int currentSeconds;
	int currentMinutes;
	float x, y;
	char timeStr[10];

protected:
	void calcTime();
	void init(int x, int y);

public:
	HUD(int x, int y);
	void composeFrame();
	void display();
};