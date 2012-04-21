#pragma once
#include "2DGraphics.h"

namespace GObjLib {
	class GObject : public CObject {
	protected:
		CBaseSprite *sprite;
	
	public:
		bool enabled;

		GObject() : CObject() { enabled = false; }
		void create(int x1, int y1, int xspeed1, int yspeed1, CBaseSprite *sprite);

		/* get and set properties */
		int getX() { float x, y; Getxy(x, y); return (int)x; }
		int getY() { float x, y; Getxy(x, y); return (int)y; }
		void setX(int x) { Setxy((float)x, getY()); }
		void setY(int y) { Setxy(getX(), (float)y); }
		void offsetX(int x);
		void offsetY(int y);
		void offsetXY(int x, int y);
		bool match(const GObject &right) const;
		bool GObject::match(const CBaseSprite *right) const;
	};
};