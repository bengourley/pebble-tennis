#include "overview.h"

static void render(Layer *layer, GContext *ctx) {

  OverviewData *data = (OverviewData *) layer_get_data(layer);
  State *state = data->state;

  GRect bounds = layer_get_frame(layer);

  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  GRect text_box = { .origin = { .x = 10, .y = 10 }, .size = { .h = 34, .w = (bounds.size.w - 30) / 2 } };
  graphics_fill_rect(ctx, text_box, 5, GCornerNone);

  graphics_draw_text(
      ctx
    , state->player_sets > state->opponent_sets ? "WIN" : "LOSE"
    , fonts_get_system_font(FONT_KEY_GOTHIC_24)
    , text_box
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentCenter
    , NULL);

  GRect score_box =
    { .origin = { .x = 10 + (bounds.size.w - 30) / 2, .y = 10 }
    , .size = { .h = 34, .w = (bounds.size.w - 30) / 2 }
    };
  graphics_draw_rect(ctx, score_box);

  char str[10];
  snprintf(str, sizeof(str), "%d-%d", state->player_sets, state->opponent_sets);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(
      ctx
    , str
    , fonts_get_system_font(FONT_KEY_GOTHIC_24)
    , score_box
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentCenter
    , NULL);

  int width = bounds.size.w - 30;
  int num_sets = state->player_sets + state->opponent_sets;
  int cell_size = (width / 5);
  int line_width = cell_size * num_sets;

  graphics_draw_line(ctx
    , GPoint(10 + (width / 2) - (line_width / 2), 68 + cell_size + 3)
    , GPoint(10 + (width / 2) + (line_width / 2), 68 + cell_size + 3));

  for (int i = 0; i < num_sets; i++) {
    bool player_won_set = state->completed_sets[i][0] > state->completed_sets[i][1];
    int left_offset = 10 + (width / 2) - (line_width / 2) + (i * cell_size);
    GRect set_box_a = { .origin = GPoint(left_offset, 68), GSize(cell_size, cell_size) };
    GRect set_box_b = { .origin = GPoint(left_offset, 68 + cell_size + 5), GSize(cell_size, cell_size) };
    char player_set_score[4];
    char opponent_set_score[4];
    snprintf(player_set_score, sizeof(player_set_score), "%d", state->completed_sets[i][0]);
    snprintf(opponent_set_score, sizeof(opponent_set_score), "%d", state->completed_sets[i][1]);
    graphics_draw_text(
        ctx
      , player_set_score
      , fonts_get_system_font(player_won_set ? FONT_KEY_GOTHIC_18_BOLD : FONT_KEY_GOTHIC_18)
      , set_box_a
      , GTextOverflowModeTrailingEllipsis
      , GTextAlignmentCenter
      , NULL);
    graphics_draw_text(
        ctx
      , opponent_set_score
      , fonts_get_system_font(player_won_set ? FONT_KEY_GOTHIC_18 : FONT_KEY_GOTHIC_18_BOLD)
      , set_box_b
      , GTextOverflowModeTrailingEllipsis
      , GTextAlignmentCenter
      , NULL);

    if (state->completed_sets[i][2] != -1) {
      char player_tie_break_score[4];
      char opponent_tie_break_score[4];
      snprintf(player_tie_break_score, sizeof(player_tie_break_score), "%d", state->completed_sets[i][2]);
      snprintf(opponent_tie_break_score, sizeof(opponent_tie_break_score), "%d", state->completed_sets[i][3]);
      GRect tie_break_box_a = { .origin = GPoint(left_offset + 8, 68), GSize(cell_size, cell_size) };
      GRect tie_break_box_b = { .origin = GPoint(left_offset + 8, 68 + cell_size + 5), GSize(cell_size, cell_size) };
      graphics_draw_text(
          ctx
        , player_tie_break_score
        , fonts_get_system_font(FONT_KEY_GOTHIC_14)
        , tie_break_box_a
        , GTextOverflowModeTrailingEllipsis
        , GTextAlignmentCenter
        , NULL);
      graphics_draw_text(
          ctx
        , opponent_tie_break_score
        , fonts_get_system_font(FONT_KEY_GOTHIC_14)
        , tie_break_box_b
        , GTextOverflowModeTrailingEllipsis
        , GTextAlignmentCenter
        , NULL);
    }
  }

  GRect match_time_text_box =
    { .origin = GPoint(10, bounds.size.h - 29)
    , .size = GSize((bounds.size.w - 30) / 2, 30)
    };
  GRect match_time_box =
    { .origin = GPoint(10 + ((bounds.size.w - 30) / 2), bounds.size.h - 33)
    , .size = GSize((bounds.size.w - 30) / 2, 30)
    };

  char time_str[25];
  snprintf(time_str, sizeof(time_str), "%dh %d"
    , (state->match_end_time - state->match_start_time) / SECONDS_PER_HOUR
    , ((state->match_end_time - state->match_start_time) % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE);

  graphics_draw_text(
      ctx
    , "Duration:"
    , fonts_get_system_font(FONT_KEY_GOTHIC_14)
    , match_time_text_box
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentRight
    , NULL);

  graphics_draw_text(
      ctx
    , time_str
    , fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD)
    , match_time_box
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentCenter
    , NULL);

}

Layer *overview_create(GRect bounds, State *state) {

  Layer *layer = layer_create_with_data(bounds, sizeof(OverviewData));
  OverviewData *data = (OverviewData *) layer_get_data(layer);
  data->state = state;
  layer_set_update_proc(layer, render);
  return layer;

}
