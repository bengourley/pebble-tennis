#include <pebble.h>
#include "windows/menu.h"
#include "persistence.h"

void init(void) {
  // clear_all();
  menu_window_push();
}

void deinit(void) {
}

// Match screen test

// #include "windows/match.h"
//
// void init(void) {
//   Settings settings = (Settings)
//     { .num_sets = 3
//     , .tie_breaks = YES
//     , .final_set = FINAL_SET_SIX_ALL_TIE_BREAK
//     , .first_server = PLAYER
//   };
//   list_t *serial = list_new();
//   for (int i = 0; i < 30; i++) add_player_points(serial);
//   match_window_push(&settings, serial);
// }

// Summary screen test

// #include "windows/summary.h"
// #include "state.h"
//
// static Settings s =
//   { .num_sets = 5
//   , .tie_breaks = YES
//   , .final_set = FINAL_SET_SIX_ALL_TIE_BREAK
//   , .first_server = PLAYER
//   };
//
// void init(void) {
//   State state = state_new(&s);
//   state.player_sets = 3;
//   state.opponent_sets = 2;
//   state.completed_sets[0][0] = 7;
//   state.completed_sets[0][1] = 6;
//   state.completed_sets[0][2] = 3;
//   state.completed_sets[0][3] = 7;
//   state.completed_sets[1][0] = 4;
//   state.completed_sets[1][1] = 6;
//   state.completed_sets[2][0] = 6;
//   state.completed_sets[2][1] = 1;
//   state.completed_sets[3][0] = 3;
//   state.completed_sets[3][1] = 6;
//   state.completed_sets[4][0] = 6;
//   state.completed_sets[4][1] = 1;
//   state.match_start_time = time(NULL);
//   state.match_end_time = state.match_start_time + (1.2 * (60 * 60));
//   state.player_total_points = 67;
//   state.opponent_total_points = 234;
//   summary_window_push(state);
// }


int main(void) {
  init();
  app_event_loop();
  deinit();
}
