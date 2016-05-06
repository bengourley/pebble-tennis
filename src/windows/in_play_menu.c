#include "in_play_menu.h"

static Window *s_main_window;
static list_t *serial;

static SimpleMenuLayer *in_play_menu_layer;
static SimpleMenuSection in_play_menu_sections[1];
static SimpleMenuItem in_play_menu_items[3];

static void resume() {
  window_stack_pop(true);
}

static void discard() {
  window_stack_pop_all(true);
  menu_window_push();
}

// static void save() {
//   save_match(serial);
//   window_stack_pop_all(true);
// }

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  in_play_menu_sections[0] = (SimpleMenuSection) {
    .num_items = 2,
    .items = in_play_menu_items
  };

  // in_play_menu_items[0] = (SimpleMenuItem) {
  //   .title = "Save and quit",
  //   .callback = save
  // };

  in_play_menu_items[0] = (SimpleMenuItem) {
    .title = "Resume match",
    .callback = resume
  };

  in_play_menu_items[1] = (SimpleMenuItem) {
    .title = "Discard match",
    .callback = discard
  };

  in_play_menu_layer = simple_menu_layer_create(bounds, window, in_play_menu_sections, 1, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(in_play_menu_layer));

}

static void window_unload(Window *window) {
  window_destroy(window);
  s_main_window = NULL;
}

void in_play_menu_window_push(list_t *srl) {
  serial = srl;
  if (!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
  }
  window_stack_push(s_main_window, true);
}
