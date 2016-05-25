#ifndef LAYERS_SUMMARY_PAGINATION_DOTS_H
#define LAYERS_SUMMARY_PAGINATION_DOTS_H

#include <pebble.h>

typedef struct {
  int index;
  int num_pages;
} PaginationLayerData;

Layer *pagination_dots_create(GRect bounds, int num_pages);
void pagination_dots_set_index(Layer* layer, int index);

#endif
