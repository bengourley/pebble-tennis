#include "summary.h"

static Window *s_main_window;
static State *final_state;

static TextLayer *result;
static TextLayer *match_score;
static char match_score_str[6];

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  result = text_layer_create(GRect(10, 10, bounds.size.w - 20, 38));
  text_layer_set_text(result, final_state->player_sets > final_state->opponent_sets ? "You win!" : "You lose");
  text_layer_set_background_color(result, GColorBlack);
  text_layer_set_text_color(result, GColorWhite);
  text_layer_set_font(result, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(result, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) result);

  match_score = text_layer_create(GRect(0, 75, bounds.size.w, 50));
  snprintf(match_score_str, sizeof(match_score_str), "%d - %d", final_state->player_sets, final_state->opponent_sets);
  text_layer_set_text(match_score, match_score_str);
  text_layer_set_font(match_score, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(match_score, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) match_score);

}

static void menu_click_handler(ClickRecognizerRef recognize, void *context) {
  window_stack_pop(true);
  menu_window_push();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_BACK, menu_click_handler);
}

static void window_unload(Window *window) {
  window_destroy(window);
  s_main_window = NULL;
}

void summary_window_push(State *s) {
  final_state = s;
  if (!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
    window_set_click_config_provider(s_main_window, click_config_provider);
  }
  window_stack_push(s_main_window, true);
}
