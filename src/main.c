#include <pebble.h>

#include "data.h"

static Window *app_window;

void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_UP, add_opponent_score);
	window_single_click_subscribe(BUTTON_ID_DOWN, add_player_score);
  window_single_click_subscribe(BUTTON_ID_SELECT, undo_last_score);
}

void init(void) {
  app_window = window_create();
  window_stack_push(app_window, true);
  window_set_click_config_provider(app_window, click_config_provider);
}

void deinit(void) {
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}