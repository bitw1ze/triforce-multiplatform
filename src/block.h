#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "2DGraphics.h"

/* The Block class abstracts operations on a single block, such as getting and setting the x and y values 
   and setting states */

class Block : public CObject {
/*
	class BlockSprite : public CBaseSprite {
	public:
		BlockSprite(int frames, int w, int h) : CBaseSprite(frames, w, h) {}
	};

protected:
	BlockSprite *blockSprite;
*/
public:
	// A block will only be displayed if it is enabled.
	bool enabled;

	Block() : CObject() { enabled = false; }
	void create(int x1, int y1, int xspeed1, int yspeed1, CBaseSprite *sprite, CTimer *timer);
	int getX() { float x, y; Getxy(x, y); return (int)x; }
	int getY() { float x, y; Getxy(x, y); return (int)y; }
	void setX(int x) { Setxy((float)x, getY()); }
	void setY(int y) { Setxy(getX(), (float)y); }
};

#endif