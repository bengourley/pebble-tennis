#pragma once

#include <stdbool.h>
#include "../deps/list/list.h"

typedef enum {
  LOVE=0,
  FIFTEEN=1,
  THIRTY=2,
  FORTY=3,
  AD=4
} SCORE_NAMES;

typedef enum {
  NEVER,
  SIX_ALL_EVERY_SET,
  SIX_ALL_EVERY_SET_LAST_SET_CHAMPIONSHIP_TIE_BREAK,
  LAST_SET
} TIE_BREAK_OPTIONS;

typedef struct {
  int player_score;
  int opponent_score;
  int player_games;
  int opponent_games;
  int player_sets;
  int opponent_sets;
  bool is_tie_break;
  bool is_complete;
  int num_sets;
  int tie_break_at;
} State;

State compute_state(list_t *serial);
State *next_state(State *s, char *point);
void debug_state(State *s);
void increment_point(State *s, int *scorer, int *non_scorer, int *scorer_games, int *non_scorer_games, int *scorer_sets, int *non_scorer_sets);
void increment_game(State *s, int *scorer_games, int *non_scorer_games, int *scorer_sets, int *non_scorer_sets);
void increment_set(State *s, int *scorer_sets, int *non_scorer_sets);
