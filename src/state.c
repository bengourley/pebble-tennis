#include <pebble.h>
#include "state.h"
#include "../deps/list/list.h"

static const char OPPONENT_SCORE = 'O';
static const char PLAYER_SCORE = 'P';

State state_new(Settings *settings) {

  // Allocate memory for tracking previous sets
  int **completed_sets = (int **) malloc(settings->num_sets * sizeof(int*));
  for (int i = 0; i < settings->num_sets; i++) {
    completed_sets[i] = (int *) malloc(4 * sizeof(int*));
    completed_sets[i][0] = 0;
    completed_sets[i][1] = 0;
    completed_sets[i][2] = 0;
    completed_sets[i][3] = 0;
  }

  return (State)
    { .player_score = 0
    , .opponent_score = 0
    , .player_games = 0
    , .opponent_games = 0
    , .player_sets = 0
    , .opponent_sets = 0
    , .is_tie_break = false
    , .is_complete = false
    , .is_final_set = settings->num_sets == 1
    , .num_sets = settings->num_sets
    , .tie_breaks = settings->tie_breaks
    , .final_set = settings->final_set
    , .server = settings->first_server
    , .completed_sets = completed_sets
    };
}

void state_destroy(State *s) {
  for (int i = 0; i < s->num_sets; i++) free(s->completed_sets[i]);
  free(s->completed_sets);
}

State compute_state(list_t *serial, Settings *settings) {
  list_node_t *node;
  list_iterator_t *it = list_iterator_new(serial, LIST_HEAD);
  State state = state_new(settings);
  while ((node = list_iterator_next(it))) next_state(&state, (char *) node->val);
  list_iterator_destroy(it);
  return state;
}

void next_state(State *s, char *point) {

  bool is_player_score = *point == PLAYER_SCORE;
  int *scorer = is_player_score ? &s->player_score : &s->opponent_score;
  int *non_scorer = is_player_score ? &s->opponent_score : &s->player_score;
  int *scorer_games = is_player_score ? &s->player_games : &s->opponent_games;
  int *non_scorer_games = is_player_score ? &s->opponent_games : &s->player_games;
  int *scorer_sets = is_player_score ? &s->player_sets : &s->opponent_sets;
  int *non_scorer_sets = is_player_score ? &s->opponent_sets : &s->player_sets;

  increment_point(s, is_player_score, scorer, non_scorer, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);

}

void toggle_server(State *s) {
  // Toggle the server
  if (s->server == PLAYER) {
    s->server = OPPONENT;
  } else if (s->server == OPPONENT) {
    s->server = PLAYER;
  }
}

void increment_point(State *s, bool is_player_score, int *scorer, int *non_scorer, int *scorer_games, int *non_scorer_games, int *scorer_sets, int *non_scorer_sets) {

  if (s->is_tie_break) {

    if ((*scorer + *non_scorer) % 2 == 0) {
      toggle_server(s);
    }

    if (*scorer < 6) {
      *scorer = *scorer + 1;
      return;
    } else if (*scorer - *non_scorer < 1) {
      *scorer = *scorer + 1;
      return;
    } else {
      // track the result of the tie break
      *scorer = *scorer + 1;
      int current_set = s->player_sets + s->opponent_sets;
      s->completed_sets[current_set][is_player_score ? 2 : 3] = *scorer;
      s->completed_sets[current_set][is_player_score ? 3 : 2] = *non_scorer;

      *scorer = LOVE;
      *non_scorer = LOVE;
      increment_game(s, is_player_score, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);
      return;
    }

  } else if (s->is_final_set && s->final_set == FINAL_SET_CHAMPIONSHIP_TIE_BREAK) {

    if ((*scorer + *non_scorer) % 2 == 0) {
      toggle_server(s);
    }

    if (*scorer < 9) {
      *scorer = *scorer + 1;
      return;
    } else if (*scorer - *non_scorer < 1) {
      *scorer = *scorer + 1;
      return;
    } else {

      // track the result of the tie break
      *scorer = *scorer + 1;
      int current_set = s->player_sets + s->opponent_sets;
      s->completed_sets[current_set][is_player_score ? 2 : 3] = *scorer;
      s->completed_sets[current_set][is_player_score ? 3 : 2] = *non_scorer;

      *scorer = LOVE;
      *non_scorer = LOVE;
      increment_game(s, is_player_score, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);
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
          increment_game(s, is_player_score, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);
        }
        break;
      case AD:
        *scorer = LOVE;
        *non_scorer = LOVE;
        increment_game(s, is_player_score, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);
        break;
    }

  }

}

void increment_game(State *s, bool is_player_score, int *scorer_games, int *non_scorer_games, int *scorer_sets, int *non_scorer_sets) {

  toggle_server(s);

  // If this was a championship tie break, one game wins it
  if (s->is_final_set && s->final_set == FINAL_SET_CHAMPIONSHIP_TIE_BREAK) {
    increment_set(s, is_player_score, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);
    return;
  }

  // If tie breaks are switched on, or this is the final set and a
  // tie break is required enter a tie break at 6 all.
  if (s->tie_breaks == YES || (s->is_final_set && s->final_set == FINAL_SET_SIX_ALL_TIE_BREAK)) {
    if (s->is_tie_break) {
      s->is_tie_break = false;
      increment_set(s, is_player_score, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);
    } else if (*scorer_games < 5) {
      *scorer_games = *scorer_games + 1;
    } else if (*scorer_games - *non_scorer_games < 1) {
      *scorer_games = *scorer_games + 1;
      if (*scorer_games == 6 && *non_scorer_games == 6) {
        s->is_tie_break = true;
      }
    } else {
      increment_set(s, is_player_score, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);
    }
    return;
  }

  // Long set mode, keep going 'til 6 or 2 clear games
  if (*scorer_games < 5) {
    *scorer_games = *scorer_games + 1;
  } else if (*scorer_games - *non_scorer_games < 1) {
    *scorer_games = *scorer_games + 1;
  } else {
    increment_set(s, is_player_score, scorer_games, non_scorer_games, scorer_sets, non_scorer_sets);
  }

}

void increment_set(State *s, bool is_player_score, int *scorer_games, int *non_scorer_games, int *scorer_sets, int *non_scorer_sets) {
  *scorer_games = *scorer_games + 1;
  int current_set = s->player_sets + s->opponent_sets;
  s->completed_sets[current_set][is_player_score ? 0 : 1] = *scorer_games;
  s->completed_sets[current_set][is_player_score ? 1 : 0] = *non_scorer_games;
  *scorer_games = 0;
  *non_scorer_games = 0;
  *scorer_sets = *scorer_sets + 1;
  s->is_final_set = *scorer_sets + *non_scorer_sets == s->num_sets - 1;
  if (*scorer_sets > s->num_sets - *scorer_sets) s->is_complete = true;
}

void debug_state(State *s) {

  char player_score[4];
  char opponent_score[4];

  if (!s->is_tie_break && (!s->is_final_set && !s->final_set == FINAL_SET_CHAMPIONSHIP_TIE_BREAK)) {

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

  for (int i = 0; i < s->num_sets; i++) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Set %d: %d-%d", i, s->completed_sets[i][0], s->completed_sets[i][1]);
    if (s->completed_sets[i][2] != 0 || s->completed_sets[i][3] != 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Tie break: %d-%d", s->completed_sets[i][2], s->completed_sets[i][3]);
    }
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG
    , "%d set match, %s-%s, GAMES: %d-%d, SETS: %d-%d, Tie Break: %d, Server: %d, Final set: %d"
    , s->num_sets, player_score, opponent_score, s->player_games
    , s->opponent_games, s->player_sets, s->opponent_sets, s->is_tie_break
    , s->server, s->is_final_set);
}
