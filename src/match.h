#pragma once

#include <pebble.h>

Window *init_match();
void deinit_match();

typedef enum {
	LOVE = 0,
	FIFTEEN = 1,
	THIRTY = 2,
	FORTY = 3,
	ADV = 4,
	WIN = 5
} tennisScores;