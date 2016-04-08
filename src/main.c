#include <pebble.h>

#include "state.h"
#include "serial.h"
#include "match_view.h"

static Window *app_window;
static list_t *serial;

void opponent_score_click_handler() {
  add_opponent_score(serial);
  State state = compute_state(serial);
  debug_state(&state);
  render(&state);
}

void player_score_click_handler() {
  add_player_score(serial);
  State state = compute_state(serial);
  debug_state(&state);
  render(&state);
}

void undo_click_handler() {
  undo(serial);
  State state = compute_state(serial);
  debug_state(&state);
  render(&state);
}

void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_UP, opponent_score_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, player_score_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, undo_click_handler);
}

void init(void) {
  serial = serial_new();
  State state = compute_state(serial);
  debug_state(&state);
  app_window = match_view_new();
//   app_window = window_create();
  window_stack_push(app_window, true);
  window_set_click_config_provider(app_window, click_config_provider);
}

void deinit(void) {
//   deinit_match();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}