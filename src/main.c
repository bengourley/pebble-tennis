#include <pebble.h>
#include "windows/menu.h"
#include "persistence.h"

void init(void) {
  // clear_all();
  menu_window_push();
}

void deinit(void) {
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
