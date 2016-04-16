#include <pebble.h>

#include "state.h"
#include "serial.h"
#include "match_view.h"
#include "menu_view.h"

static Window *app_window;
static list_t *serial;

void start_match(ClickRecognizerRef recognizer, void *context) {
  serial = serial_new();
  State state = compute_state(serial);
  debug_state(&state);
  app_window = match_view_new(serial);
  window_stack_push(app_window, true);
}

void menu_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, start_match);
}

void init(void) {
  app_window = menu_view_new();
  window_stack_push(app_window, true);
  window_set_click_config_provider(app_window, menu_click_config_provider);
}

void deinit(void) {
//   deinit_match();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
