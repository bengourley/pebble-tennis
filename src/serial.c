#include <pebble.h>
#include "serial.h"
#include "list.h"

list_t *serial_new() {
  return list_new();
}

void add_opponent_score(list_t *serial) {
  list_rpush(serial, list_node_new(OPPONENT_SCORE));
}

void add_player_score(list_t *serial) {
  list_rpush(serial, list_node_new(PLAYER_SCORE));
}

void undo(list_t *serial) {
  list_rpop(serial);
}
