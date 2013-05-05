#include "game.h"
#include <cstdio>
#include <string>

HUD::HUD(int _x, int _y) {
	init(_x, _y);
}

void HUD::init(int _x = 0, int _y = 0) {
	startTime = mainTimer->time();
	x = (float)_x / screen_w;
	y = (float)_y / screen_h;
}

void HUD::composeFrame() {
	calcTime();
}

void HUD::calcTime() {
	currentTime = (mainTimer->time() - startTime) / 1000;
	currentSeconds = (int)(currentTime % 60);
	currentMinutes = (int)(currentTime / 60);
}

void HUD::display() {
	snprintf(timeStr, 10, "%02d:%02d\0", currentMinutes, currentSeconds);
	textPrintf(x, y, (void *)GamePlay::font1, timeStr, GamePlay::fcolor1);
}
