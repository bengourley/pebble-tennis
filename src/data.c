#include <pebble.h>
#include "data.h"

static int points[512];
static int next_score_index = 0;

State next_state(State s, int point) {

  int *scorer = point == PLAYER_SCORE ? &s.player_score : &s.opponent_score;
  int *non_scorer = point == PLAYER_SCORE ? &s.opponent_score : &s.player_score;
  int *scorer_games = point == PLAYER_SCORE ? &s.player_games : &s.opponent_games;
  int *non_scorer_games = point == PLAYER_SCORE ? &s.opponent_games : &s.player_games;
  int *scorer_sets = point == PLAYER_SCORE ? &s.player_sets : &s.opponent_sets;
  int *non_scorer_sets = point == PLAYER_SCORE ? &s.opponent_sets : &s.player_sets;
  
  switch (*scorer) {
    case LOVE: *scorer = FIFTEEN; break;
    case FIFTEEN: *scorer = THIRTY; break;
    case THIRTY: *scorer = FORTY; break;
    case FORTY:
      if (*non_scorer == AD) {
        *non_scorer = FORTY;
      } else if (*non_scorer == FORTY) {
        *scorer = AD;
      } else {
        *scorer = LOVE;
        *non_scorer = LOVE;
        
        // Compute games
        if (*scorer_games < 5 || (*scorer_games == 5 && *non_scorer_games == 5)) {
          // just add one
          *scorer_games = *scorer_games + 1;
        } else if (*scorer_games == 5 && *non_scorer_games < 5) {
          *scorer_games = 0;
          *non_scorer_games = 0;
          *scorer_sets = *scorer_sets + 1;
        } else {
          *scorer_games = 0;
          *non_scorer_games = 0;
          *scorer_sets = *scorer_sets + 1;
        }

      }
      break;
    case AD:
      *scorer = LOVE;
      *non_scorer = LOVE;
      
      // Compute games
      if (*scorer_games < 5 || (*scorer_games == 5 && *non_scorer_games == 5)) {
        // just add one
        *scorer_games = *scorer_games + 1;
      } else if (*scorer_games == 5 && *non_scorer_games < 5) {
        *scorer_games = 0;
        *non_scorer_games = 0;
        *scorer_sets = *scorer_sets + 1;
      } else {
        *scorer_games = 0;
        *non_scorer_games = 0;
        *scorer_sets = *scorer_sets + 1;
      }

      break;
  }
  return s;

}

State get_state() {

  int player_score = LOVE;
  int opponent_score = LOVE;

  int player_games = 0;
  int opponent_games = 0;
  
  int player_sets = 0;
  int opponent_sets = 0;
  
  State s =
    { player_score
    , opponent_score
    , player_games
    , opponent_games
    , player_sets
    , opponent_sets
    };

  for (int i = 0; i < next_score_index; i++) s = next_state(s, points[i]);

  return s;

}

void add_player_score() {
  points[next_score_index] = PLAYER_SCORE;
  next_score_index++;
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Adding player score");
  print_state(get_state());
}

void add_opponent_score() {
  points[next_score_index] = OPPONENT_SCORE;
  next_score_index++;
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Adding opponent score");
  print_state(get_state());
}

void undo_last_score() {
  if (next_score_index == 0) return;
  next_score_index--;
  print_state(get_state());
}

void print_state(State s) {
  char *player_score = "";
  char *opponent_score = "";
  
  switch (s.player_score) {
    case LOVE: player_score = "0"; break;
    case FIFTEEN: player_score = "15"; break;
    case THIRTY: player_score = "30"; break;
    case FORTY: player_score = "40"; break;
    case AD: player_score = "AD"; break;
  }

  switch (s.opponent_score) {
    case LOVE: opponent_score = "0"; break;
    case FIFTEEN: opponent_score = "15"; break;
    case THIRTY: opponent_score = "30"; break;
    case FORTY: opponent_score = "40"; break;
    case AD: opponent_score = "AD"; break;
  }
  
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s-%s, GAMES: %d-%d, SETS: %d-%d", player_score, opponent_score, s.player_games, s.opponent_games, s.player_sets, s.opponent_sets);
}