#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <pebble.h>

typedef enum {
  NUM_SETS,
  TIE_BREAKS,
  FINAL_SET,
  SERIALISED_MATCH,
  SERIALISED_MATCH_SIZE
} PERSIST_KEYS;

#endif
