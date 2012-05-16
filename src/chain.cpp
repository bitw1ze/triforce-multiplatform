#include "game.h"

Chain::Chain(int cnt) {
	count = cnt;
}

bool Chain::update() {
	return mainTimer->elapsed(lastActivation, displayDuration);
}

void Chain::activate() {
	lastActivation = mainTimer->time();
}