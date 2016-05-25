#include <pebble.h>
#include "serial.h"

list_t *serial_new() {
  list_t *serial = list_new();
  serial->free = *free;
  return serial;
}

void add_opponent_points(list_t *serial) {
  Point *point = (Point *) malloc(sizeof(Point));
  point->scorer = OPPONENT_SCORE;
  point->timestamp = time(NULL);
  list_rpush(serial, list_node_new((void *) point));
}

void add_player_points(list_t *serial) {
  Point *point = (Point *) malloc(sizeof(Point));
  point->scorer = PLAYER_SCORE;
  point->timestamp = time(NULL);
  list_rpush(serial, list_node_new((void *) point));
}

void undo(list_t *serial) {
  free(list_rpop(serial));
}

void serial_destroy(list_t *serial) {
  list_destroy(serial);
}
