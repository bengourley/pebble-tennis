#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include "serial.h"
#include "../deps/list/list.h"

typedef enum {
  LOVE=0,
  FIFTEEN=1,
  THIRTY=2,
  FORTY=3,
  AD=4
} SCORE_NAMES;

typedef struct {

  // Match state
  int player_points;
  int opponent_points;
  int player_games;
  int opponent_games;
  int player_sets;
  int opponent_sets;
  bool is_tie_break;
  bool is_complete;
  bool is_final_set;

  // Settings
  int num_sets;
  int tie_breaks;
  int final_set;
  int server;

  // Collected stats
  int match_start_time;
  int match_end_time;
  int player_total_points;
  int opponent_total_points;
  int player_break_points_faced;
  int player_break_points_conceded;
  int opponent_break_points_faced;
  int opponent_break_points_conceded;
  int **completed_sets;

} State;

typedef struct {
  int *points;
  int *games;
  int *sets;
  bool is_serving;
  bool is_player;
} Player;

typedef enum {
  PLAYER=0,
  OPPONENT=1
} SERVER;

typedef struct {
  int num_sets;
  int tie_breaks;
  int final_set;
  int first_server;
  int scoreboard;
} Settings;

typedef enum {
  YES=0,
  NO=1
} SWITCH_SETTING;

typedef enum {
  FINAL_SET_SIX_ALL_TIE_BREAK=0,
  FINAL_SET_NO_TIE_BREAK=1,
  FINAL_SET_CHAMPIONSHIP_TIE_BREAK=2
} FINAL_SET_SETTING;

State compute_state(list_t *serial, Settings *settings);
State state_new(Settings *settings);
void state_destroy(State *state);
void next_state(State *s, Point *point);
void debug_state(State *s);
void increment_point(State *s, Player *scorer, Player *non_scorer);
void increment_game(State *s, Player *scorer, Player *non_scorer);
void increment_set(State *s, Player *scorer, Player *non_scorer);

#endif
