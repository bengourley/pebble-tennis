#include <pebble.h>
#include "windows/menu.h"
#include "persistence.h"

void init(void) {
  persist_delete(NUM_SETS);
  persist_delete(TIE_BREAKS);
  persist_delete(FINAL_SET);
  persist_delete(SERIALISED_MATCH);
  persist_delete(SERIALISED_MATCH_SIZE);
  menu_window_push();
}

void deinit(void) {
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
