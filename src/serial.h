#ifndef SERIAL_H
#define SERIAL_H

#include "../deps/list/list.h"

typedef enum {
  PLAYER_SCORE='P',
  OPPONENT_SCORE='O'
} POINTS;

list_t *serial_new();
void add_player_score(list_t *serial);
void add_opponent_score(list_t *serial);
void undo(list_t *serial);

#endif
