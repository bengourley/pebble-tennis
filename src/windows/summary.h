#ifndef WINDOWS_SUMMARY_H
#define WINDOWS_SUMMARY_H

#include <pebble.h>
#include "menu.h"
#include "../state.h"
#include "../layers/summary/pagination_dots.h"
#include "../layers/summary/overview.h"
#include "../layers/summary/points.h"

typedef enum {
  IN_UP,IN_DOWN,OUT_UP,OUT_DOWN
} Direction;

void summary_window_push(State state);

#endif
