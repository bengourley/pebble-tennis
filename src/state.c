#include <pebble.h>
#include "serial.h"
#include "state.h"
#include "../deps/list/list.h"

State state_new() {
  return (State)
    { .player_score = 0
    , .opponent_score = 0
    , .player_games = 0
    , .opponent_games = 0
    , .player_sets = 0
    , .opponent_sets = 0
    , .is_tie_break = false
    , .is_complete = false
    , .num_sets = 3
    , .tie_break_at = SIX_ALL_EVERY_SET
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
  int *non_scorer_sets = is_player_score ? &s->opponent_sets : &s->player_sets;

  increment_point(s, scorer, non_scorer, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);

  return s;

}

void increment_point(State *s, int *scorer, int *non_scorer, int *scorer_games, int *non_scorer_games, int *scorer_sets, int *non_scorer_sets) {

  if (s->is_tie_break) {

    if (*scorer < 6) {
      *scorer = *scorer + 1;
      return;
    } else if (*scorer - *non_scorer < 1) {
     *scorer = *scorer + 1;
      return;
    } else {
      *scorer = LOVE;
      *non_scorer = LOVE;
      increment_game(s, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);
      return;
    }

  } else {

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
          increment_game(s, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);
        }
        break;
      case AD:
        *scorer = LOVE;
        *non_scorer = LOVE;
        increment_game(s, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);
        break;
    }

  }

}

void increment_game(State *s, int *scorer_games, int *non_scorer_games, int *scorer_sets, int *non_scorer_sets) {

  switch (s->tie_break_at) {

    case NEVER:
    case LAST_SET:
      if (*scorer_games < 5) {
        *scorer_games = *scorer_games + 1;
      } else if (*scorer_games - *non_scorer_games < 1) {
        *scorer_games = *scorer_games + 1;
      } else {
        increment_set(s, scorer_sets, non_scorer_sets);
      }
      break;

    case SIX_ALL_EVERY_SET:
    case SIX_ALL_EVERY_SET_LAST_SET_CHAMPIONSHIP_TIE_BREAK:
      if (s->is_tie_break) {
        s->is_tie_break = false;
        *scorer_games = 0;
        *non_scorer_games = 0;
        increment_set(s, scorer_sets, non_scorer_sets);
      } else if (*scorer_games < 5) {
        *scorer_games = *scorer_games + 1;
      } else if (*scorer_games - *non_scorer_games < 1) {
        *scorer_games = *scorer_games + 1;
        if (*scorer_games == 6 && *non_scorer_games == 6) {
          s->is_tie_break = true;
        }
      } else {
        *scorer_games = 0;
        *non_scorer_games = 0;
        increment_set(s, scorer_sets, non_scorer_sets);
      }
      break;

  }

}

void increment_set(State *s, int *scorer_sets, int *non_scorer_sets) {
  *scorer_sets = *scorer_sets + 1;
  if (*scorer_sets > s->num_sets - *scorer_sets) s->is_complete = true;
}

void debug_state(State *s) {

  char player_score[4];
  char opponent_score[4];

  if (!s->is_tie_break) {

    // Normal game, need to convert int score to 0, 30, 40, A

    switch (s->player_score) {
      case LOVE: snprintf(player_score, 3, "0"); break;
      case FIFTEEN: snprintf(player_score, 3, "15"); break;
      case THIRTY: snprintf(player_score, 3, "30"); break;
      case FORTY: snprintf(player_score, 3, "40"); break;
      case AD: snprintf(player_score, 3, "AD"); break;
    }

    switch (s->opponent_score) {
      case LOVE: snprintf(opponent_score, 3, "0"); break;
      case FIFTEEN: snprintf(opponent_score, 3, "15"); break;
      case THIRTY: snprintf(opponent_score, 3, "30"); break;
      case FORTY: snprintf(opponent_score, 3, "40"); break;
      case AD: snprintf(opponent_score, 3, "AD"); break;
    }

  } else {

    // Tie-break, use score value as is
    snprintf(player_score, 3, "%d", s->player_score);
    snprintf(opponent_score, 3, "%d", s->opponent_score);

  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s-%s, GAMES: %d-%d, SETS: %d-%d, Tie Break: %d", player_score, opponent_score, s->player_games, s->opponent_games, s->player_sets, s->opponent_sets, s->is_tie_break);
}
