#include "pagination_dots.h"

static const int dot_spacing = 4;
static const int dot_size = 6;

void draw_dots(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_frame(layer);
  PaginationLayerData* layer_data = (PaginationLayerData *) layer_get_data(layer);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  for (int i = 0; i < layer_data->num_pages; i++) {
    int dot_vert_offset = (bounds.size.h / 2) - ((layer_data->num_pages * (dot_size + dot_spacing)) / 2);
    int current_dot_vert_offset = dot_vert_offset + (i * (dot_spacing + dot_size));
    if (i == layer_data->index) {
      graphics_context_set_fill_color(ctx, GColorBlack);
    } else {
      graphics_context_set_fill_color(ctx, GColorWhite);
    }
    graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, current_dot_vert_offset + (dot_size / 2)), dot_size / 2);
    graphics_draw_circle(ctx, GPoint(bounds.size.w / 2, current_dot_vert_offset + (dot_size / 2)), dot_size / 2);
  }
}

Layer *pagination_dots_create(GRect bounds, int num_pages) {

  Layer *layer = layer_create_with_data((GRect)
    { .origin = { .x = bounds.size.w - 20, .y = 0 }
    , .size = { .w = 20, .h = bounds.size.h }
    }, sizeof(PaginationLayerData));

  PaginationLayerData* layer_data = (PaginationLayerData *) layer_get_data(layer);
  layer_data->index = 0;
  layer_data->num_pages = num_pages;

  layer_set_update_proc(layer, draw_dots);

  return layer;

}

void pagination_dots_set_index(Layer *layer, int i) {
  PaginationLayerData* layer_data = (PaginationLayerData *) layer_get_data(layer);
  if (i < 0 || i >= layer_data->num_pages) return;
  layer_data->index = i;
  layer_mark_dirty(layer);
}
