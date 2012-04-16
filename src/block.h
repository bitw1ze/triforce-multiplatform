#include "2DGraphics.h"

class Block : public CObject {
public:
	bool enabled;

	Block() : CObject() { enabled = false; }
	void create(int x1,int y1,int xspeed1,int yspeed1, CBaseSprite *sprite, CTimer *timer);
};