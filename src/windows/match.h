#ifndef WINDOWS_MATCH_H
#define WINDOWS_MATCH_H

#include <pebble.h>
#include "in_play_menu.h"
#include "summary.h"
#include "../state.h"
#include "../serial.h"
#include "../persistence.h"

void match_window_push(Settings *settings, list_t *serial);
void draw_server_marker();

#endif
