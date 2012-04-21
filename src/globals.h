/* This file contains constants that need to be accessed by several modules */

#pragma once

typedef unsigned int uint32;
typedef unsigned long uint64;

namespace Globals {
	const uint32
		screen_w		= 1024,
		screen_h		= 768,
		nrows			= 12,
		ncols			= 6,
		nblocktypes		= 7;
	const string themeDirectory = "themes\\classic\\";
};