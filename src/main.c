#include <pebble.h>
#include "windows/menu.h"
#include "persistence.h"

void init(void) {
  // clear_all();

  // Largest expected inbox and outbox message sizes
  const uint32_t inbox_size = 64;
  const uint32_t outbox_size = 256;

  // Open AppMessage
  app_message_open(inbox_size, outbox_size);

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
//   for (int j = 0; j < 18; j++) add_opponent_points(serial);
//   for (int k = 0; k < 3; k++) add_player_points(serial);
//   match_window_push(&settings, serial);
// }

// Summary screen test
//
// #include "windows/summary.h"
// #include "state.h"
//
// static Settings s =
//   { .num_sets = 3
//   , .tie_breaks = YES
//   , .final_set = FINAL_SET_CHAMPIONSHIP_TIE_BREAK
//   , .first_server = PLAYER
//   };
//
// void init(void) {
//   State state = state_new(&s);
//   state.player_sets = 2;
//   state.opponent_sets = 1;
//   state.completed_sets[0][0] = 7;
//   state.completed_sets[0][1] = 6;
//   state.completed_sets[0][2] = 7;
//   state.completed_sets[0][3] = 3;
//   state.completed_sets[1][0] = 4;
//   state.completed_sets[1][1] = 6;
//   state.completed_sets[2][0] = 6;
//   state.completed_sets[2][1] = 3;
//   state.match_start_time = time(NULL);
//   state.match_end_time = state.match_start_time + (1.2 * (60 * 60));
//   state.player_total_points = 84;
//   state.opponent_total_points = 67;
//   state.player_break_points_faced = 8;
//   state.player_break_points_conceded = 2;
//   state.opponent_break_points_faced = 5;
//   state.opponent_break_points_conceded= 4;
//   summary_window_push(state);
// }

int main(void) {
  init();
  app_event_loop();
  deinit();
}
