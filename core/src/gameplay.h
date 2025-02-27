#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stdint.h>

#include "game.h"
#include "move.h"

void draw_at_most_to_rack(Bag *bag, Rack *rack, int n);
void play_move(Game *game, Move *move);
void set_random_rack(Game *game, int pidx, Rack *existing_rack);
Move *get_top_equity_move(Game *game);

#endif