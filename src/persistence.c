#include "persistence.h"

static const uint8_t blank_byte = 0;
static const uint8_t mask = 128;

const char *byte_to_binary(int x) {
  static char b[9];
  b[0] = '\0';
  int z;
  for (z = 128; z > 0; z >>= 1) {
    strcat(b, ((x & z) == z) ? "1" : "0");
  }
  return b;
}

void load_settings(Settings *settings) {
  if (persist_exists(NUM_SETS)) settings->num_sets = persist_read_int(NUM_SETS);
  if (persist_exists(TIE_BREAKS)) settings->tie_breaks = persist_read_int(TIE_BREAKS);
  if (persist_exists(FINAL_SET)) settings->final_set = persist_read_int(FINAL_SET);
  if (persist_exists(SCOREBOARD)) settings->scoreboard = persist_read_int(SCOREBOARD);
}

void save_settings(Settings *settings) {
  persist_write_int(NUM_SETS, settings->num_sets);
  persist_write_int(TIE_BREAKS, settings->tie_breaks);
  persist_write_int(FINAL_SET, settings->final_set);
  persist_write_int(SCOREBOARD, settings->scoreboard);
}

void clear_match() {
  persist_delete(SERIALISED_MATCH);
  persist_delete(SERIALISED_MATCH_SIZE);
}

void save_match(list_t *serial) {
  int len = serial->len / 8;
  uint8_t buffer[len];
  list_node_t *node;
  list_iterator_t *it = list_iterator_new(serial, LIST_HEAD);
  int i = 0;
  int current_byte = -1;
  while ((node = list_iterator_next(it))) {
    if (i % 8 == 0) {
      ++current_byte;
      buffer[current_byte] = blank_byte;
    }
    if (*(char *) node->val == 'P') {
      buffer[current_byte] = buffer[current_byte] | mask >> i % 8;
    }
    ++i;
    if (i % 8 == 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "buffer[current_byte]=%s", byte_to_binary(buffer[current_byte]));
    }
  }
  list_iterator_destroy(it);

  persist_write_data(SERIALISED_MATCH, buffer, len);
  persist_write_int(SERIALISED_MATCH_SIZE, serial->len);

}

void load_match(list_t *serial) {
  if (!persist_exists(SERIALISED_MATCH_SIZE)) return;
  int len = persist_read_int(SERIALISED_MATCH_SIZE);
  uint8_t buffer[len / 8];
  persist_read_data(SERIALISED_MATCH, buffer, len);
  serial = serial_new();
  APP_LOG(APP_LOG_LEVEL_INFO, "read len %d", len);
  for (int i = 0; i < len; i++) {
    for (int z = 128; z > 0; z >>= 1) {
      APP_LOG(APP_LOG_LEVEL_INFO, "buffer[i] %d", buffer[i]);
      APP_LOG(APP_LOG_LEVEL_INFO, "is 1 %d", (buffer[i] & z) == z);
      if ((buffer[i] & z) == z) {
        add_player_points(serial);
      } else {
        add_opponent_points(serial);
      }
    }
  }
}

void clear_all() {
  persist_delete(NUM_SETS);
  persist_delete(TIE_BREAKS);
  persist_delete(FINAL_SET);
  persist_delete(FIRST_SERVER);
  persist_delete(SERIALISED_MATCH);
  persist_delete(SERIALISED_MATCH_SIZE);
  persist_delete(SCOREBOARD);
}
