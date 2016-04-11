#include <pebble.h>
#include "match_view.h"
#include "state.h"

static Window *match_window;

static TextLayer *player_score;
char player_points_str[4];
static TextLayer *opponent_score;
char opponent_points_str[4];

static TextLayer *player_games;
char player_games_str[2];
static TextLayer *opponent_games;
char opponent_games_str[2];

static TextLayer *player_sets;
char player_sets_str[2];
static TextLayer *opponent_sets;
char opponent_sets_str[2];

void display_score_update(TextLayer *t, char * str, int s, bool is_tie_break) {
  if (is_tie_break) {
    snprintf(str, 3, "%d", s);
    APP_LOG(APP_LOG_LEVEL_INFO, "%s", str);
    text_layer_set_text(t, str);
  } else {
    switch (s) {
      case LOVE: text_layer_set_text(t, "0"); break;
      case FIFTEEN: text_layer_set_text(t, "15"); break;
      case THIRTY: text_layer_set_text(t, "30"); break;
      case FORTY: text_layer_set_text(t, "40"); break;
      case AD: text_layer_set_text(t, "A"); break;
    }
  }
}

void display_digit_update(TextLayer *t, int s, char *str) {
  snprintf(str, sizeof(str), "%d", s);
  text_layer_set_text(t, str);
}

void render(State *state) {
  display_score_update(player_score, player_points_str, state->player_score, state->is_tie_break);
  display_digit_update(player_games, state->player_games, player_games_str);
  display_digit_update(player_sets, state->player_sets, player_sets_str);
  display_score_update(opponent_score, opponent_points_str, state->opponent_score, state->is_tie_break);
  display_digit_update(opponent_games, state->opponent_games, opponent_games_str);
  display_digit_update(opponent_sets, state->opponent_sets, opponent_sets_str);
}

void deinit_match() {
  window_destroy(match_window);
}

Window *match_view_new() {
  match_window = window_create();

  // Make the text
  Layer *window_layer = window_get_root_layer(match_window);
  GRect bounds = layer_get_frame(window_layer);
  
  // Player score
  player_score = text_layer_create(GRect(0, 20, bounds.size.w / 2, bounds.size.h / 2));
  text_layer_set_text(player_score, "0");
  text_layer_set_font(player_score, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(player_score, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) player_score);
  
  // Opponent score
  opponent_score = text_layer_create(GRect(bounds.size.w / 2, 20, bounds.size.w / 2, bounds.size.h / 2));
  text_layer_set_text(opponent_score, "0");
  text_layer_set_font(opponent_score, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(opponent_score, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) opponent_score);

  // Player games
  player_games = text_layer_create(GRect(0, bounds.size.h / 2, bounds.size.w / 2, bounds.size.h / 4));
  text_layer_set_text(player_games, "0");
  text_layer_set_font(player_games, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(player_games, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) player_games);

  // Opponent games
  opponent_games = text_layer_create(GRect(bounds.size.w / 2, bounds.size.h / 2, bounds.size.w / 2, bounds.size.h / 4));
  text_layer_set_text(opponent_games, "0");
  text_layer_set_font(opponent_games, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(opponent_games, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) opponent_games);

  // Player games
  player_sets = text_layer_create(GRect(0, (bounds.size.h / 4) * 3, bounds.size.w / 2, bounds.size.h / 4));
  text_layer_set_text(player_sets, "0");
  text_layer_set_font(player_sets, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(player_sets, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) player_sets);

  // Opponent games
  opponent_sets = text_layer_create(GRect(bounds.size.w / 2, (bounds.size.h / 4) * 3, bounds.size.w / 2, bounds.size.h / 4));
  text_layer_set_text(opponent_sets, "0");
  text_layer_set_font(opponent_sets, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(opponent_sets, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) opponent_sets);

  return match_window;
}