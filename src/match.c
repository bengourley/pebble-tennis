#include <pebble.h>
#include "match.h"

static Window *match_window;

static TextLayer *player_score;
static TextLayer *opponent_score;

static int player_score_int = 0;
static int oppenent_score_int = 0;

void display_score_update(TextLayer *t, int s) {
  switch (s) {
    case LOVE: text_layer_set_text(t, "0"); break;
    case FIFTEEN: text_layer_set_text(t, "15"); break;
    case THIRTY: text_layer_set_text(t, "30"); break;
    case FORTY: text_layer_set_text(t, "40"); break;
    case ADV: text_layer_set_text(t, "A"); break;
  }
}

void increment_player_score() {
  player_score_int++;
  display_score_update(player_score, player_score_int);
}

void increment_opponent_score() {
  oppenent_score_int++;
  display_score_update(opponent_score, oppenent_score_int);
}

void match_click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_UP, increment_opponent_score);
	window_single_click_subscribe(BUTTON_ID_DOWN, increment_player_score);
}

void deinit_match() {
  window_destroy(match_window);
}

Window *init_match() {
  match_window = window_create();

  // Make the text
  Layer *window_layer = window_get_root_layer(match_window);
  GRect bounds = layer_get_frame(window_layer);
  
  // Player score
  player_score = text_layer_create(GRect(0, 0, bounds.size.w / 2, bounds.size.h));
  text_layer_set_text(player_score, "0");
  text_layer_set_font(player_score, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(player_score, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) player_score);
  
  // Opponent score
  opponent_score = text_layer_create(GRect(bounds.size.w / 2, 0, bounds.size.w / 2, bounds.size.h));
  text_layer_set_text(opponent_score, "0");
  text_layer_set_font(opponent_score, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(opponent_score, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) opponent_score);
  
  // Add button handlers
  window_set_click_config_provider(match_window, match_click_config_provider);

  return match_window;
}