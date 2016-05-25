#ifndef LAYERS_SUMMARY_POINTS_H
#define LAYERS_SUMMARY_POINTS_H

#include <pebble.h>
#include "../../state.h"

Layer *points_create(GRect bounds, State *state);

typedef struct {
  State *state;
} PointsData;

#endif
