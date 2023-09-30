#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>

#include "board.h"
#include "constants.h"

#define PASS_MOVE_EQUITY -10000
#define INITIAL_TOP_MOVE_EQUITY -100000

typedef enum {
  GAME_EVENT_UNKNOWN,
  GAME_EVENT_TILE_PLACEMENT_MOVE,
  GAME_EVENT_PHONY_TILES_RETURNED,
  GAME_EVENT_PASS,
  GAME_EVENT_CHALLENGE_BONUS,
  GAME_EVENT_EXCHANGE,
  GAME_EVENT_END_RACK_POINTS,
  GAME_EVENT_TIME_PENALTY,
  GAME_EVENT_END_RACK_PENALTY,
} game_event_t;

typedef struct Move {
  game_event_t move_type;
  int score;
  int row_start;
  int col_start;
  int tiles_played;
  int tiles_length;
  double equity;
  int vertical;
  uint8_t tiles[BOARD_DIM];
} Move;

typedef struct MoveList {
  int count;
  int capacity;
  Move *spare_move;
  Move **moves;
} MoveList;

Move *create_move();
void copy_move(Move *src_move, Move *dest_move);
void destroy_move(Move *move);
MoveList *create_move_list(int capacity);
void destroy_move_list(MoveList *ml);
void sort_moves(MoveList *ml);
void store_move_description(Move *move, char *placeholder,
                            LetterDistribution *ld);
void set_spare_move(MoveList *ml, uint8_t strip[], int leftstrip,
                    int rightstrip, int score, int row_start, int col_start,
                    int tiles_played, int vertical, game_event_t move_type);
void insert_spare_move(MoveList *ml, double equity);
void insert_spare_move_top_equity(MoveList *ml, double equity);
Move *pop_move(MoveList *ml);
void reset_move_list(MoveList *ml);
void set_move(Move *move, uint8_t strip[], int leftstrip, int rightstrip,
              int score, int row_start, int col_start, int tiles_played,
              int vertical, game_event_t move_type);
void set_move_as_pass(Move *move);
void set_spare_move_as_pass(MoveList *ml);
void string_builder_add_move(Board *board, Move *m,
                             LetterDistribution *letter_distribution,
                             StringBuilder *string_builder);
#endif