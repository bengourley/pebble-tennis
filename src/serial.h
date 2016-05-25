#ifndef SERIAL_H
#define SERIAL_H

#include "../deps/list/list.h"


typedef enum {
  PLAYER_SCORE=0,
  OPPONENT_SCORE=1
} SCORER;

typedef struct {
  int scorer;
  int timestamp;
} Point;

list_t *serial_new();
void add_player_points(list_t *serial);
void add_opponent_points(list_t *serial);
void undo(list_t *serial);
void serial_destroy(list_t *serial);

#endif
