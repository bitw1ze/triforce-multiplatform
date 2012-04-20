#include "betterlib.h"

using namespace BetterLib;

void GObject::create(int x1,int y1,int xspeed1,int yspeed1, CBaseSprite *sprite, CTimer *timer) { 
	__super::create(x1, y1, xspeed1, yspeed1, sprite, timer); 
	enabled = true;
}

void GObject::offsetX(int x) {
	this->setX( getX() + x );
}

void GObject::offsetY(int y) {
	this->setY( getY() + y );
}

void GObject::offsetXY(int x, int y) {
	this->Setxy( getX() + x, y + getY() );
}