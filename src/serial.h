#ifndef SERIAL_H
#define SERIAL_H

#include "../deps/list/list.h"

#define PLAYER_SCORE "P"
#define OPPONENT_SCORE "O"

list_t *serial_new();
void add_player_score(list_t *serial);
void add_opponent_score(list_t *serial);
void undo(list_t *serial);

#endif
