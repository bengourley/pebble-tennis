#include <pebble.h>
#include "menu_view.h"

static Window *menu_window;

Window *menu_view_new() {

  menu_window = window_create();

  // Make the text
  Layer *window_layer = window_get_root_layer(menu_window);
  GRect bounds = layer_get_frame(window_layer);

  // Main title
  TextLayer *title_text = text_layer_create(GRect(0, 35, bounds.size.w, bounds.size.h / 2));
  text_layer_set_text(title_text, "TENNIS|AD");
  text_layer_set_font(title_text, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(title_text, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) title_text);
  
  TextLayer *play_text = text_layer_create(GRect(40, 78, bounds.size.w - 80, 19));
  text_layer_set_text(play_text, "PLAY");
  text_layer_set_font(play_text, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(play_text, GTextAlignmentCenter);
  text_layer_set_text_color(play_text, GColorWhite);
  text_layer_set_background_color(play_text, GColorBlack);
  layer_add_child(window_layer, (Layer *) play_text);

  TextLayer *configure_text = text_layer_create(GRect(40, 106, bounds.size.w - 80, 19));
  text_layer_set_text(configure_text, "SETTINGS");
  text_layer_set_font(configure_text, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(configure_text, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) configure_text);

  return menu_window;

}