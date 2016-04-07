#pragma once

typedef enum {
  PLAYER_SCORE,
  OPPONENT_SCORE,
} SCORE;

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

void add_player_score();
void add_opponent_score();
void undo_last_score();
void print_state(State s);
State next_state(State s, int point);