#include "menu.h"
#include "match.h"
// #include "server_select.h"
#include "../persistence.h"

static Window *s_main_window;

static SimpleMenuLayer *menu_layer;
static SimpleMenuSection menu_sections[2];
static SimpleMenuItem menu_items[1];
static SimpleMenuItem option_menu_items[4];

const char *final_set_options[] = { "Tie break at 6-6", "No tie break", "Championship tie break" };
const char *switch_options[] = { "Yes", "No" };

static Settings settings =
  { .num_sets = 3
  , .tie_breaks = YES
  , .final_set = FINAL_SET_SIX_ALL_TIE_BREAK
  };

void toggle_switch_setting(int *setting) {
  *setting ^= 1;
}

char* num_sets_to_string(int n) {
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
  option_menu_items[0].subtitle = num_sets_to_string(settings.num_sets);
  layer_mark_dirty(simple_menu_layer_get_layer(menu_layer));
}

void cycle_final_set_setting() {
  if (settings.final_set < 2) {
    ++settings.final_set;
  } else {
    settings.final_set = 0;
  }
  option_menu_items[2].subtitle = final_set_options[settings.final_set];
  layer_mark_dirty(simple_menu_layer_get_layer(menu_layer));
}

void cycle_tie_breaks_setting() {
  toggle_switch_setting(&settings.tie_breaks);
  option_menu_items[1].subtitle = switch_options[settings.tie_breaks];
  layer_mark_dirty(simple_menu_layer_get_layer(menu_layer));
}

void load_settings() {
  if (persist_exists(NUM_SETS)) settings.num_sets = persist_read_int(NUM_SETS);
  if (persist_exists(TIE_BREAKS)) settings.tie_breaks = persist_read_int(TIE_BREAKS);
  if (persist_exists(FINAL_SET)) settings.final_set = persist_read_int(FINAL_SET);
}

void save_settings() {
  persist_write_int(NUM_SETS, settings.num_sets);
  persist_write_int(TIE_BREAKS, settings.tie_breaks);
  persist_write_int(FINAL_SET, settings.final_set);
}

void start_match() {
  match_window_push(&settings);
  save_settings();
}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  load_settings();

  menu_sections[0] = (SimpleMenuSection) {
    .num_items = 1,
    .items = menu_items
  };

  menu_items[0] = (SimpleMenuItem) {
    .title = "Start Match",
    .callback = start_match
  };

  menu_sections[1] = (SimpleMenuSection) {
    .title = "Match Settings",
    .items = option_menu_items,
    .num_items = 3
  };

  option_menu_items[0] = (SimpleMenuItem) {
    .title = "Sets",
    .subtitle = num_sets_to_string(settings.num_sets),
    .callback = cycle_match_type
  };

  option_menu_items[1] = (SimpleMenuItem) {
    .title = "Tie Breaks",
    .subtitle = switch_options[settings.tie_breaks],
    .callback = cycle_tie_breaks_setting
  };

  option_menu_items[2] = (SimpleMenuItem) {
    .title = "Final Set",
    .subtitle = final_set_options[settings.final_set],
    .callback = cycle_final_set_setting
  };

  menu_layer = simple_menu_layer_create(bounds, window, menu_sections, 2, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(menu_layer));

}

static void window_unload(Window *window) {
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
