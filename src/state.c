#include <pebble.h>
#include "serial.h"
#include "state.h"
#include "list.h"

State state_new() {
  return (State)
    { .player_score = 0
    , .opponent_score = 0
    , .player_games = 0
    , .opponent_games = 0
    , .player_sets = 0
    , .opponent_sets = 0
    };
}

State compute_state(list_t *serial) {
  list_node_t *node;
  list_iterator_t *it = list_iterator_new(serial, LIST_HEAD);
  State state = state_new();
  while ((node = list_iterator_next(it))) next_state(&state, node->val);
  return state;
}

State *next_state(State *s, char *point) {

  bool is_player_score = strcmp(point, PLAYER_SCORE) == 0;
  int *scorer = is_player_score ? &s->player_score : &s->opponent_score;
  int *non_scorer = is_player_score ? &s->opponent_score : &s->player_score;
  int *scorer_games = is_player_score ? &s->player_games : &s->opponent_games;
  int *non_scorer_games = is_player_score ? &s->opponent_games : &s->player_games;
  int *scorer_sets = is_player_score ? &s->player_sets : &s->opponent_sets;
//   int *non_scorer_sets = is_player_score ? &s->opponent_sets : &s->player_sets;
  
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

void debug_state(State *s) {
  char *player_score = "";
  char *opponent_score = "";
  
  switch (s->player_score) {
    case LOVE: player_score = "0"; break;
    case FIFTEEN: player_score = "15"; break;
    case THIRTY: player_score = "30"; break;
    case FORTY: player_score = "40"; break;
    case AD: player_score = "AD"; break;
  }

  switch (s->opponent_score) {
    case LOVE: opponent_score = "0"; break;
    case FIFTEEN: opponent_score = "15"; break;
    case THIRTY: opponent_score = "30"; break;
    case FORTY: opponent_score = "40"; break;
    case AD: opponent_score = "AD"; break;
  }
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s-%s, GAMES: %d-%d, SETS: %d-%d", player_score, opponent_score, s->player_games, s->opponent_games, s->player_sets, s->opponent_sets);
}