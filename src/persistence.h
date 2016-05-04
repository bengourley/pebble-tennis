#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <pebble.h>
#include "state.h"

typedef enum {
  NUM_SETS,
  TIE_BREAKS,
  FINAL_SET,
  FIRST_SERVER,
  SERIALISED_MATCH,
  SERIALISED_MATCH_SIZE
} PERSIST_KEYS;

void load_settings(Settings *settings);
void save_settings(Settings *settings);
void load_match(list_t *serial);
void save_match(list_t *serial);
void clear_all();

#endif
