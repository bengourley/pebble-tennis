#include <pebble.h>
#include "windows/menu.h"

void init(void) {
  menu_window_push();
}

void deinit(void) {
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
