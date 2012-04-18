#include "2DGraphics.h"

namespace BetterLib {
	class GObject : public CObject {
	
	public:
		bool enabled;

		GObject() : CObject() { enabled = false; }
		void GObject::create(int x1, int y1, int xspeed1, int yspeed1, CBaseSprite *sprite, CTimer *timer);
		int getX() { float x, y; Getxy(x, y); return (int)x; }
		int getY() { float x, y; Getxy(x, y); return (int)y; }
		void setX(int x) { Setxy((float)x, getY()); }
		void setY(int y) { Setxy(getX(), (float)y); }
	};
};