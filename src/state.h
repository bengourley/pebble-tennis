#pragma once

#include "list.h"

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
} State;

State compute_state(list_t *serial);
State *next_state(State *s, char *point);
void debug_state(State *s);