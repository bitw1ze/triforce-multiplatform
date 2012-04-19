#include "gobjlib.h"

using namespace GObjLib;

void GObject::create(int x1,int y1,int xspeed1,int yspeed1, CBaseSprite *spr) { 
	__super::create(x1, y1, xspeed1, yspeed1, spr, NULL); 
	enabled = true;
	sprite = spr;
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

bool GObject::match(const GObject &right) const {
	return ( sprite == right.sprite );
}

bool GObject::match(const CBaseSprite *right) const {
	return ( sprite == right );
}