#include <pebble.h>
#include "state.h"
#include "../deps/list/list.h"

State state_new(Settings *settings) {

  // Allocate memory for tracking previous sets
  int **completed_sets = (int **) malloc(settings->num_sets * sizeof(int*));
  for (int i = 0; i < settings->num_sets; i++) {
    completed_sets[i] = (int *) malloc(4 * sizeof(int*));
    completed_sets[i][0] = 0;
    completed_sets[i][1] = 0;
    completed_sets[i][2] = -1;
    completed_sets[i][3] = -1;
  }

  return (State)
    { .player_points = 0
    , .opponent_points = 0
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

    // Collected stats
    , .match_start_time = -1
    , .match_end_time = -1
    , .player_total_points = 0
    , .opponent_total_points = 0
    , .player_break_points_faced = 0
    , .player_break_points_conceded = 0
    , .opponent_break_points_faced = 0
    , .opponent_break_points_conceded = 0
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
  while ((node = list_iterator_next(it))) next_state(&state, (Point *) node->val);
  list_iterator_destroy(it);
  return state;
}

void next_state(State *s, Point *point) {

  if (s->match_start_time == -1) s->match_start_time = point->timestamp;
  s->match_end_time = point->timestamp;

  bool is_player_point = point->scorer == PLAYER_SCORE;

  Player scorer =
    { .points = is_player_point ? &s->player_points : &s->opponent_points
    , .games = is_player_point ? &s->player_games : &s->opponent_games
    , .sets = is_player_point ? &s->player_sets : &s->opponent_sets
    , .is_serving = is_player_point ? s->server == PLAYER : s->server == OPPONENT
    , .is_player = is_player_point
    };

  Player non_scorer =
    { .points = is_player_point ? &s->opponent_points : &s->player_points
    , .games = is_player_point ? &s->opponent_games : &s->player_games
    , .sets = is_player_point ? &s->opponent_sets : &s->player_sets
    , .is_serving = is_player_point ? s->server == OPPONENT : s->server == PLAYER
    , .is_player = !is_player_point
    };

  increment_point(s, &scorer, &non_scorer);

}

void toggle_server(State *s) {
  // Toggle the server
  if (s->server == PLAYER) {
    s->server = OPPONENT;
  } else if (s->server == OPPONENT) {
    s->server = PLAYER;
  }
}

void increment_point(State *s, Player *scorer, Player *non_scorer) {

  scorer->is_player ? s->player_total_points++ : s->opponent_total_points++;

  if (s->is_tie_break) {

    if ((*scorer->points + *non_scorer->points) % 2 == 0) toggle_server(s);

    if (*scorer->points < 6) {
      *scorer->points = *scorer->points + 1;
      return;
    } else if (*scorer->points - *non_scorer->points < 1) {
      *scorer->points = *scorer->points + 1;
      return;
    } else {
      // track the result of the tie break
      *scorer->points = *scorer->points + 1;
      int current_set = s->player_sets + s->opponent_sets;
      s->completed_sets[current_set][scorer->is_player ? 2 : 3] = *scorer->points;
      s->completed_sets[current_set][scorer->is_player ? 3 : 2] = *non_scorer->points;
      increment_game(s, scorer, non_scorer);
      return;
    }

  } else if (s->is_final_set && s->final_set == FINAL_SET_CHAMPIONSHIP_TIE_BREAK) {

    if ((*scorer->points + *non_scorer->points) % 2 == 0) toggle_server(s);

    if (*scorer->points < 9) {
      *scorer->points = *scorer->points + 1;
      return;
    } else if (*scorer->points - *non_scorer->points < 1) {
      *scorer->points = *scorer->points + 1;
      return;
    } else {
      // track the result of the tie break
      *scorer->points = *scorer->points + 1;
      int current_set = s->player_sets + s->opponent_sets;
      s->completed_sets[current_set][scorer->is_player ? 2 : 3] = *scorer->points;
      s->completed_sets[current_set][scorer->is_player ? 3 : 2] = *non_scorer->points;
      increment_game(s, scorer, non_scorer);
      return;
    }

  } else {

    switch (*scorer->points) {
      case LOVE: *scorer->points = FIFTEEN; break;
      case FIFTEEN: *scorer->points = THIRTY; break;
      case THIRTY:
        *scorer->points = FORTY;
        break;
      case FORTY:
        if (*non_scorer->points == AD) {
          *non_scorer->points = FORTY;
        } else if (*non_scorer->points == FORTY) {
          *scorer->points = AD;
        } else {
          if (!scorer->is_serving) {
            // Break of serve
            scorer->is_player ? s->opponent_break_points_conceded++ : s->player_break_points_conceded++;
          }
          increment_game(s, scorer, non_scorer);
        }
        break;
      case AD:
        if (!scorer->is_serving) {
          // Break of serve
          scorer->is_player ? s->opponent_break_points_conceded++ : s->player_break_points_conceded++;
        }
        increment_game(s, scorer, non_scorer);
        break;

      }

      bool is_break_point_against_scorer = scorer->is_serving
        && *non_scorer->points == FORTY
        && *scorer->points < FORTY;

      bool is_break_point_against_non_scorer = non_scorer->is_serving
        && (*scorer->points == AD || (*scorer->points == FORTY && *non_scorer->points < FORTY));

      if (is_break_point_against_scorer) {
        // This is a break point against scorer
        non_scorer->is_player ? s->opponent_break_points_faced++ : s->player_break_points_faced++;
      }

      if (is_break_point_against_non_scorer) {
        // This is a break point against scorer
        scorer->is_player ? s->opponent_break_points_faced++ : s->player_break_points_faced++;
      }

  }

}

void increment_game(State *s, Player *scorer, Player *non_scorer) {

  *scorer->points = LOVE;
  *non_scorer->points = LOVE;

  toggle_server(s);

  // If this was a championship tie break, one game wins it
  if (s->is_final_set && s->final_set == FINAL_SET_CHAMPIONSHIP_TIE_BREAK) {
    increment_set(s, scorer, non_scorer);
    return;
  }

  // If tie breaks are switched on, or this is the final set and a
  // tie break is required enter a tie break at 6 all.
  if (s->tie_breaks == YES || (s->is_final_set && s->final_set == FINAL_SET_SIX_ALL_TIE_BREAK)) {
    if (s->is_tie_break) {
      s->is_tie_break = false;
      increment_set(s, scorer, non_scorer);
    } else if (*scorer->games < 5) {
      *scorer->games = *scorer->games + 1;
    } else if (*scorer->games - *non_scorer->games < 1) {
      *scorer->games = *scorer->games + 1;
      if (*scorer->games == 6 && *non_scorer->games == 6) {
        s->is_tie_break = true;
      }
    } else {
      increment_set(s, scorer, non_scorer);
    }
    return;
  }

  // Long set mode, keep going 'til 6 or 2 clear games
  if (*scorer->games < 5) {
    *scorer->games = *scorer->games + 1;
  } else if (*scorer->games - *non_scorer->games < 1) {
    *scorer->games = *scorer->games + 1;
  } else {
    increment_set(s, scorer, non_scorer);
  }

}

void increment_set(State *s, Player *scorer, Player *non_scorer) {
  *scorer->games = *scorer->games + 1;
  int current_set = s->player_sets + s->opponent_sets;
  s->completed_sets[current_set][scorer->is_player ? 0 : 1] = *scorer->games;
  s->completed_sets[current_set][scorer->is_player ? 1 : 0] = *non_scorer->games;
  *scorer->games = 0;
  *non_scorer->games = 0;
  *scorer->sets = *scorer->sets + 1;
  s->is_final_set = *scorer->sets + *non_scorer->sets == s->num_sets - 1;
  if (*scorer->sets > s->num_sets - *scorer->sets) s->is_complete = true;
}

void debug_state(State *s) {

  char player_points[4];
  char opponent_points[4];

  if (!s->is_tie_break && (!s->is_final_set && !s->final_set == FINAL_SET_CHAMPIONSHIP_TIE_BREAK)) {

    // Normal game, need to convert int score to 0, 30, 40, A

    switch (s->player_points) {
      case LOVE: snprintf(player_points, 3, "0"); break;
      case FIFTEEN: snprintf(player_points, 3, "15"); break;
      case THIRTY: snprintf(player_points, 3, "30"); break;
      case FORTY: snprintf(player_points, 3, "40"); break;
      case AD: snprintf(player_points, 3, "AD"); break;
    }

    switch (s->opponent_points) {
      case LOVE: snprintf(opponent_points, 3, "0"); break;
      case FIFTEEN: snprintf(opponent_points, 3, "15"); break;
      case THIRTY: snprintf(opponent_points, 3, "30"); break;
      case FORTY: snprintf(opponent_points, 3, "40"); break;
      case AD: snprintf(opponent_points, 3, "AD"); break;
    }

  } else {

    // Tie-break, use score value as is
    snprintf(player_points, 3, "%d", s->player_points);
    snprintf(opponent_points, 3, "%d", s->opponent_points);

  }


  APP_LOG(APP_LOG_LEVEL_INFO, "%d set match", s->num_sets);

  APP_LOG(APP_LOG_LEVEL_INFO, "%s-%s, GAMES: %d-%d, SETS: %d-%d"
    , player_points, opponent_points
    , s->player_games, s->opponent_games
    , s->player_sets, s->opponent_sets);

  APP_LOG(APP_LOG_LEVEL_INFO, "Tie Break: %d, Server: %d, Final set: %d"
    , s->is_tie_break, s->server, s->is_final_set);

  APP_LOG(APP_LOG_LEVEL_INFO, "Break points. Player: %d/%d, Opponent: %d/%d"
    , s->player_break_points_conceded, s->player_break_points_faced
    , s->opponent_break_points_conceded, s->opponent_break_points_faced);

  APP_LOG(APP_LOG_LEVEL_INFO, "Total points. Player: %d, Opponent: %d"
    , s->player_total_points, s->opponent_total_points);

  for (int i = 0; i < s->num_sets; i++) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Set %d: %d-%d", i, s->completed_sets[i][0], s->completed_sets[i][1]);
    if (s->completed_sets[i][2] != 0 || s->completed_sets[i][3] != 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Tie break: %d-%d", s->completed_sets[i][2], s->completed_sets[i][3]);
    }
  }

}
