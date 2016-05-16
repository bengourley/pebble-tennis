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
//   for (int i = 0; i < 30; i++) add_player_score(serial);
//   match_window_push(&settings, serial);
// }

// Summary screen test

// #include "windows/summary.h"
//
// void init(void) {
//   State state = (State)
//     { .player_score = 0
//     , .opponent_score = 0
//     , .player_games = 0
//     , .opponent_games = 0
//     , .player_sets = 2
//     , .opponent_sets = 1
//     , .is_tie_break = false
//     , .is_complete = true
//     , .is_final_set = true
//     , .num_sets = 3
//     , .tie_breaks = YES
//     , .final_set = FINAL_SET_SIX_ALL_TIE_BREAK
//     , .server = PLAYER
//     };
//   summary_window_push(&state);
// }


int main(void) {
  init();
  app_event_loop();
  deinit();
}
