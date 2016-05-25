#include "summary.h"

static Window *s_main_window;
static State final_state;

static TextLayer *result;
static TextLayer *match_score;
static char match_score_str[6];

static Layer *pagination;
static int current_page;
static const int num_pages = 2;
const int duration_ms = 100;

static Layer *pages[5];
static Layer *overview;
static Layer *points;

static void window_load(Window *window) {

  current_page = 0;

  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_frame(window_layer);
  GRect page_bounds = { .origin = GPoint(0, 168), .size = window_bounds.size };

  pagination = pagination_dots_create(window_bounds, num_pages);
  layer_add_child(window_layer, pagination);

  overview = overview_create(window_bounds, &final_state);
  pages[0] = overview;
  layer_add_child(window_layer, overview);

  points = points_create(window_bounds, &final_state);
  pages[1] = points;
  layer_add_child(window_layer, points);
  layer_set_bounds(points, (GRect) { .origin = GPoint(0, 168), .size = layer_get_bounds(points).size });

}

static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
  property_animation_destroy((PropertyAnimation *) animation);
}

static void animate(Layer *layer, Direction direction) {
  GPoint start;
  GPoint finish;
  switch (direction) {
    case IN_UP:
      start = GPoint(0, 168);
      finish = GPoint(0, 0);
      break;
    case IN_DOWN:
      start = GPoint(0, -168);
      finish = GPoint(0, 0);
      break;
    case OUT_UP:
      start = GPoint(0, 0);
      finish = GPoint(0, -168);
      break;
    case OUT_DOWN:
      start = GPoint(0, 0);
      finish = GPoint(0, 168);
      break;
  }
  PropertyAnimation *prop_anim = property_animation_create_bounds_origin(layer, &start, &finish);
  Animation *anim = property_animation_get_animation(prop_anim);
  animation_set_curve(anim, AnimationCurveEaseOut);
  animation_set_duration(anim, duration_ms);
  animation_set_handlers(anim, (AnimationHandlers) {
    .stopped = anim_stopped_handler
  }, NULL);
  animation_schedule(anim);
}

static void menu_click_handler(ClickRecognizerRef recognize, void *context) {
  window_stack_pop(true);
  menu_window_push();
}

static void previous_page_handler(ClickRecognizerRef recognize, void *context) {
  if (current_page == 0) return;
  int previous_page = current_page;
  pagination_dots_set_index(pagination, --current_page);
  animate(pages[current_page], IN_DOWN);
  animate(pages[previous_page], OUT_DOWN);
}

static void next_page_handler(ClickRecognizerRef recognize, void *context) {
  if (current_page == num_pages - 1) return;
  int previous_page = current_page;
  pagination_dots_set_index(pagination, ++current_page);
  animate(pages[current_page], IN_UP);
  animate(pages[previous_page], OUT_UP);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_BACK, menu_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, previous_page_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, next_page_handler);
}

static void window_unload(Window *window) {
  text_layer_destroy(result);
  text_layer_destroy(match_score);
  state_destroy(&final_state);
  window_destroy(window);
  layer_destroy(pagination);
  for (int i = 0; i < num_pages; i++) layer_destroy(pages[i]);
  s_main_window = NULL;
}

void summary_window_push(State s) {
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
