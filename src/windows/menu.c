#include "menu.h"

static Window *s_main_window;

static SimpleMenuLayer *main_menu_layer;
static SimpleMenuSection main_menu_sections[2];
static SimpleMenuItem main_menu_items[1];
static SimpleMenuItem main_menu_item_options[4];

static const char *final_set_options[] = { "Tie break at 6-6", "No tie break", "Championship tie break" };
static const char *switch_options[] = { "Yes", "No" };

static Settings settings;

void toggle_switch_setting(int *setting) {
  *setting ^= 1;
}

const char *num_sets_to_string(int n) {
  switch (n) {
    case 1: return "1";
    case 3: return "3";
    case 5: return "5";
    default: return "?";
  }
}

void cycle_match_type() {
  switch (settings.num_sets) {
    case 1:
    case 3:
      settings.num_sets = settings.num_sets + 2;
      break;
    default:
      settings.num_sets = 1;
      break;
  }
  main_menu_item_options[0].subtitle = num_sets_to_string(settings.num_sets);
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}

void cycle_final_set_setting() {
  if (settings.final_set < 2) {
    ++settings.final_set;
  } else {
    settings.final_set = 0;
  }
  main_menu_item_options[2].subtitle = final_set_options[settings.final_set];
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}

void cycle_tie_breaks_setting() {
  toggle_switch_setting(&settings.tie_breaks);
  main_menu_item_options[1].subtitle = switch_options[settings.tie_breaks];
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}

void start_match() {
  window_stack_pop(false);
  match_window_push(&settings, list_new());
  save_settings(&settings);
}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  settings = (Settings)
    { .num_sets = 3
    , .first_server = PLAYER
    };

  load_settings(&settings);

  main_menu_sections[0] = (SimpleMenuSection) {
    .num_items = 1,
    .items = main_menu_items
  };

  main_menu_items[0] = (SimpleMenuItem) {
    .title = "Start Match",
    .callback = start_match
  };

  main_menu_sections[1] = (SimpleMenuSection) {
    .title = "Match Settings",
    .items = main_menu_item_options,
    .num_items = 1
  };

  main_menu_item_options[0] = (SimpleMenuItem) {
    .title = "Sets",
    .subtitle = num_sets_to_string(settings.num_sets),
    .callback = cycle_match_type
  };

  main_menu_layer = simple_menu_layer_create(bounds, window, main_menu_sections, 2, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(main_menu_layer));

}

static void window_unload(Window *window) {
  simple_menu_layer_destroy(main_menu_layer);
  window_destroy(window);
  s_main_window = NULL;
}

void menu_window_push() {
  if (!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
  }
  window_stack_push(s_main_window, true);
}
