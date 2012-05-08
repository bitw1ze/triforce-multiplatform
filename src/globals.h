/* This file contains constants that need to be accessed by several modules */

#pragma once

typedef unsigned int uint32;
typedef unsigned long uint64;

// FIXME: these should be moved to Input at some point
#define ESC 27
#define SPACE 32
#define ENTER 13

namespace Globals {
	const uint32
		screen_w		= 1024,
		screen_h		= 720,
		nrows			= 12,
		ncols			= 6,
		nblocktypes		= 7;
	const string themeDirectory = "themes\\classic\\";
};