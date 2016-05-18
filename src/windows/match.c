#include "match.h"

static list_t *serial;
static Window *s_main_window;
static Settings settings;

static TextLayer *sets_label;
static TextLayer *games_label;

static TextLayer *player_score;
static char player_points_str[4];
static TextLayer *opponent_score;
static char opponent_points_str[4];

static TextLayer *player_sets;
static char player_sets_str[2];
static TextLayer *opponent_sets;
static char opponent_sets_str[2];

static Layer *layout_layer;
static Layer *server_marker_layer;

static int server;

static void display_digit_update(TextLayer *t, int s, char *str) {
  snprintf(str, sizeof(str), "%d", s);
  text_layer_set_text(t, str);
}

static void render(State *state) {
  display_digit_update(player_score, state->player_score, player_points_str);
  display_digit_update(player_sets, state->player_sets, player_sets_str);
  display_digit_update(opponent_score, state->opponent_score, opponent_points_str);
  display_digit_update(opponent_sets, state->opponent_sets, opponent_sets_str);
  server = state->server;
  draw_server_marker();
}

static void show_summary() {
  window_stack_pop_all(true);
  State state = compute_state(serial, &settings);
  summary_window_push(&state);
}

static void opponent_score_click_handler(ClickRecognizerRef recognizer, void *context) {
  add_opponent_score(serial);
  State state = compute_state(serial, &settings);
  debug_state(&state);
  render(&state);
  if (state.is_complete) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Match complete!");
    show_summary();
  }
}

static void player_score_click_handler(ClickRecognizerRef recognizer, void *context) {
  add_player_score(serial);
  State state = compute_state(serial, &settings);
  debug_state(&state);
  render(&state);
  if (state.is_complete) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Match complete!");
    show_summary();
  }
}

static void undo_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (serial->len > 0) {
    // If there are scores, middle button rewinds
    undo(serial);
  } else {
    // If there is no score yet, middle button toggles first server
    if (settings.first_server == PLAYER) {
      settings.first_server = OPPONENT;
    } else {
      settings.first_server = PLAYER;
    }
  }
  State state = compute_state(serial, &settings);
  debug_state(&state);
  render(&state);
}

static void menu_click_handler(ClickRecognizerRef recognize, void *context) {
  in_play_menu_window_push(serial);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, opponent_score_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, player_score_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, undo_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, menu_click_handler);
}

static void layout_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_frame(layer);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, GPoint(2, bounds.size.h / 2), GPoint(bounds.size.w - 2, bounds.size.h / 2));
}

static void draw_layout() {
  GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));
  layout_layer = layer_create(bounds);
  layer_set_update_proc(layout_layer, layout_update_proc);
  layer_add_child(window_get_root_layer(s_main_window), layout_layer);
}

static void server_marker_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_frame(layer);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorBlack);
  if (server == PLAYER) {
    graphics_fill_circle(ctx, GPoint(bounds.size.w - 8, bounds.size.h * 0.75), 3);
  } else {
    graphics_fill_circle(ctx, GPoint(bounds.size.w - 8, bounds.size.h / 4), 3);
  }
}

void draw_server_marker() {
  if (server_marker_layer) {
    layer_mark_dirty(server_marker_layer);
  } else {
    GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));
    server_marker_layer = layer_create(bounds);
    layer_set_update_proc(server_marker_layer, server_marker_update_proc);
    layer_add_child(window_get_root_layer(s_main_window), server_marker_layer);
  }
}

static void window_load(Window *window) {

  // if (!serial) serial = serial_new();
  State state = compute_state(serial, &settings);
  debug_state(&state);

  // Make the text
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Player sets
  player_sets = text_layer_create(GRect(13, -19 + (bounds.size.h * 0.75), 20, 28));
  text_layer_set_text(player_sets, "0");
  text_layer_set_font(player_sets, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(player_sets, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) player_sets);

  // Opponent sets
  opponent_sets = text_layer_create(GRect(13, -19 + (bounds.size.h / 4), 20, 28));
  text_layer_set_text(opponent_sets, "0");
  text_layer_set_font(opponent_sets, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(opponent_sets, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) opponent_sets);
  
  // Player score
  player_score = text_layer_create(GRect(bounds.size.w / 2, 14 + bounds.size.h / 2, -10 + (bounds.size.w / 2), bounds.size.h / 2));
  text_layer_set_text(player_score, "0");
  text_layer_set_font(player_score, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(player_score, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) player_score);

  // Opponent score
  opponent_score = text_layer_create(GRect(bounds.size.w / 2, 14, -10 + (bounds.size.w / 2), bounds.size.h / 2));
  text_layer_set_text(opponent_score, "0");
  text_layer_set_font(opponent_score, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(opponent_score, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) opponent_score);

  draw_layout();

  // Sets label
  sets_label = text_layer_create(GRect(8, -10 + (bounds.size.h / 2), 28, 20));
  text_layer_set_background_color(sets_label, GColorWhite);
  text_layer_set_text(sets_label, "SETS");
  text_layer_set_font(sets_label, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(sets_label, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) sets_label);

  render(&state);

}

static void window_unload(Window *window) {

  list_destroy(serial);

  text_layer_destroy(sets_label);
  text_layer_destroy(games_label);
  text_layer_destroy(player_score);
  text_layer_destroy(opponent_score);
  text_layer_destroy(player_sets);
  text_layer_destroy(opponent_sets);

  layer_destroy(layout_layer);
  layout_layer = NULL;

  layer_destroy(server_marker_layer);
  server_marker_layer = NULL;

  window_destroy(window);
  s_main_window = NULL;

}

void match_window_push(Settings *s, list_t *srl) {
  if (!s_main_window) {
    settings = *s;
    serial = srl;
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
    window_set_click_config_provider(s_main_window, click_config_provider);
  }
  window_stack_push(s_main_window, true);
}
