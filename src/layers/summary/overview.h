#ifndef LAYERS_SUMMARY_OVERVIEW_H
#define LAYERS_SUMMARY_OVERVIEW_H

#include <pebble.h>
#include "../../state.h"

Layer *overview_create(GRect bounds, State *state);

typedef struct {
  State *state;
} OverviewData;

#endif
