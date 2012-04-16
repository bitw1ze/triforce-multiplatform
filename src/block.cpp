#include "block.h"

void Block::create(int x1,int y1,int xspeed1,int yspeed1, CBaseSprite *sprite, CTimer *timer) { 
	CObject::create(x1, y1, xspeed1, yspeed1, sprite, timer); 
	//blockSprite = sprite;
	enabled = true;
}