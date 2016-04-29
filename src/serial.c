#include <pebble.h>
#include "serial.h"
#include "../deps/list/list.h"

list_t *serial_new() {
  return list_new();
}

void add_opponent_score(list_t *serial) {
  char *c = (char *) malloc(sizeof(char));
  *c = OPPONENT_SCORE;
  list_rpush(serial, list_node_new((void *) c));
}

void add_player_score(list_t *serial) {
  char *c = (char *) malloc(sizeof(char));
  *c = PLAYER_SCORE;
  list_rpush(serial, list_node_new((void *) c));
}

void undo(list_t *serial) {
  free(list_rpop(serial));
}
