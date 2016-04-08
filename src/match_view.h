#pragma once

#include <pebble.h>
#include "state.h"

Window *match_view_new();
void deinit_match();
void render(State *state);