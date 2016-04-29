#include "match.h"
#include "../state.h"
#include "../serial.h"
#include "../persistence.h"

static Window *s_main_window;
static list_t *serial;
static Settings settings;

static TextLayer *sets_label;
static TextLayer *games_label;

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

// static uint8_t blank_byte = 0;
// static uint8_t mask = 128;

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

void opponent_score_click_handler(ClickRecognizerRef recognizer, void *context) {
  add_opponent_score(serial);
  State state = compute_state(serial, &settings);
  debug_state(&state);
  render(&state);
  if (state.is_complete) APP_LOG(APP_LOG_LEVEL_DEBUG, "Match complete!");
}

void player_score_click_handler(ClickRecognizerRef recognizer, void *context) {
  add_player_score(serial);
  State state = compute_state(serial, &settings);
  debug_state(&state);
  render(&state);
  if (state.is_complete) APP_LOG(APP_LOG_LEVEL_DEBUG, "Match complete!");
}

void undo_click_handler(ClickRecognizerRef recognizer, void *context) {
  undo(serial);
  State state = compute_state(serial, &settings);
  debug_state(&state);
  render(&state);
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, opponent_score_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, player_score_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, undo_click_handler);
}


// const char *byte_to_binary(int x) {
//   static char b[9];
//   b[0] = '\0';
//   int z;
//   for (z = 128; z > 0; z >>= 1) {
//     strcat(b, ((x & z) == z) ? "1" : "0");
//   }
//   return b;
// }

void save_match() {
  // int len = serial->len / 8;
  // uint8_t buffer[len];
  // list_node_t *node;
  // list_iterator_t *it = list_iterator_new(serial, LIST_HEAD);
  // int i = 0;
  // int current_byte = -1;
  // while ((node = list_iterator_next(it))) {
  //   if (i % 8 == 0) {
  //     ++current_byte;
  //     buffer[current_byte] = blank_byte;
  //   }
  //   if (*(char *) node->val == 'P') {
  //     buffer[current_byte] = buffer[current_byte] | mask >> i % 8;
  //   }
  //   ++i;
    // if (i % 8 == 0) {
    //   APP_LOG(APP_LOG_LEVEL_INFO, "buffer[current_byte]=%s", byte_to_binary(buffer[current_byte]));
    // }
  // }
  // list_iterator_destroy(it);
  // persist_write_data(SERIALISED_MATCH, buffer, len);
  // persist_write_int(SERIALISED_MATCH_SIZE, serial->len);
}

void load_match() {
  // if (!persist_exists(SERIALISED_MATCH_SIZE)) return;
  // int len = persist_read_int(SERIALISED_MATCH_SIZE);
  // uint8_t buffer[len / 8];
  // persist_read_data(SERIALISED_MATCH, buffer, len);
  // serial = serial_new();
  // APP_LOG(APP_LOG_LEVEL_INFO, "read len %d", len);
  // for (int i = 0; i < len; i++) {
  //   for (int z = 128; z > 0; z >>= 1) {
  //     APP_LOG(APP_LOG_LEVEL_INFO, "buffer[i] %d", buffer[i]);
  //     APP_LOG(APP_LOG_LEVEL_INFO, "is 1 %d", (buffer[i] & z) == z);
  //     if ((buffer[i] & z) == z) {
  //       add_player_score(serial);
  //     } else {
  //       add_opponent_score(serial);
  //     }
  //   }
  // }
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_frame(layer);
  // Set the line color
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorBlack);
  // Draw a line
  // graphics_draw_line(ctx, GPoint(bounds.size.w - 10, 10), GPoint(bounds.size.w - 10, bounds.size.h - 20));
  // Draw a line
  graphics_draw_line(ctx, GPoint(2, bounds.size.h / 2), GPoint(bounds.size.w - 2, bounds.size.h / 2));
  graphics_fill_circle(ctx, GPoint(bounds.size.w - 10, 1 + bounds.size.h / 4), 3);
}

static void draw_layout() {
  static Layer *s_canvas_layer;
  GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));
  // Create canvas layer
  s_canvas_layer = layer_create(bounds);
  // Assign the custom drawing procedure
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  // Add to Window
  layer_add_child(window_get_root_layer(s_main_window), s_canvas_layer);
}

static void window_load(Window *window) {

  load_match();
  if (!serial) serial = serial_new();
  State state = compute_state(serial, &settings);
  debug_state(&state);

  // Make the text
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Player sets
  player_sets = text_layer_create(GRect(10, -9 + (bounds.size.h * 0.75), 20, 18));
  text_layer_set_text(player_sets, "0");
  text_layer_set_font(player_sets, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(player_sets, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) player_sets);

  // Opponent sets
  opponent_sets = text_layer_create(GRect(10, -9 + (bounds.size.h / 4), 20, 18));
  text_layer_set_text(opponent_sets, "0");
  text_layer_set_font(opponent_sets, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(opponent_sets, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) opponent_sets);

  // Player games
  player_games = text_layer_create(GRect(42, -9 + (bounds.size.h * 0.75), 30, 18));
  text_layer_set_text(player_games, "0");
  text_layer_set_font(player_games, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(player_games, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) player_games);

  // Opponent games
  opponent_games = text_layer_create(GRect(42, -9 + (bounds.size.h / 4), 30, 18));
  text_layer_set_text(opponent_games, "0");
  text_layer_set_font(opponent_games, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(opponent_games, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) opponent_games);

  // Player score
  player_score = text_layer_create(GRect(bounds.size.w / 2, 25 + bounds.size.h / 2, bounds.size.w / 2, bounds.size.h / 2));
  text_layer_set_text(player_score, "0");
  text_layer_set_font(player_score, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(player_score, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) player_score);

  // Opponent score
  opponent_score = text_layer_create(GRect(bounds.size.w / 2, 25, bounds.size.w / 2, bounds.size.h / 2));
  text_layer_set_text(opponent_score, "0");
  text_layer_set_font(opponent_score, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(opponent_score, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) opponent_score);

  draw_layout();

  // Sets label
  sets_label = text_layer_create(GRect(5, -10 + (bounds.size.h / 2), 28, 20));
  text_layer_set_background_color(sets_label, GColorWhite);
  text_layer_set_text(sets_label, "SETS");
  text_layer_set_font(sets_label, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(sets_label, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) sets_label);

  // Games label
  games_label = text_layer_create(GRect(38, -10 + (bounds.size.h / 2), 42, 20));
  text_layer_set_background_color(games_label, GColorWhite);
  text_layer_set_text(games_label, "GAMES");
  text_layer_set_font(games_label, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(games_label, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) games_label);

  render(&state);

}

static void window_unload(Window *window) {
  save_match();
  window_destroy(window);
  s_main_window = NULL;
}

void match_window_push(Settings *s) {
  if (!s_main_window) {
    settings = *s;
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
    window_set_click_config_provider(s_main_window, click_config_provider);
  }
  window_stack_push(s_main_window, true);
}
