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
  int player_score;
  int opponent_score;
  int player_games;
  int opponent_games;
  int player_sets;
  int opponent_sets;
  bool is_tie_break;
  bool is_complete;
  bool is_final_set;
  int num_sets;
  int tie_breaks;
  int final_set;
  int server;
  int **completed_sets;
} State;

typedef enum {
  PLAYER=0,
  OPPONENT=1
} SERVER;

typedef struct {
  int num_sets;
  int tie_breaks;
  int final_set;
  int first_server;
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

State compute_state(list_t *serial, Settings* settings);
void state_destroy(State *state);
void next_state(State *s, char *point);
void debug_state(State *s);
void increment_point(State *s, bool is_player_score, int *scorer, int *non_scorer, int *scorer_games, int *non_scorer_games, int *scorer_sets, int *non_scorer_sets);
void increment_game(State *s, bool is_player_score, int *scorer_games, int *non_scorer_games, int *scorer_sets, int *non_scorer_sets);
void increment_set(State *s, bool is_player_score, int *scorer_games, int *non_scorer_games, int *scorer_sets, int *non_scorer_sets);

#endif
