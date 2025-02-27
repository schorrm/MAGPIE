#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>

#include "board.h"
#include "constants.h"

typedef struct Move {
  uint8_t tiles[BOARD_DIM];
  int score;
  int row_start;
  int col_start;
  int tiles_played;
  int tiles_length;
  double equity;
  int vertical;
  int move_type;
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
                    int tiles_played, int vertical, int move_type);
void insert_spare_move(MoveList *ml, double equity);
void insert_spare_move_top_equity(MoveList *ml, double equity);
Move *pop_move(MoveList *ml);
void reset_move_list(MoveList *ml);
void set_move(Move *move, uint8_t strip[], int leftstrip, int rightstrip,
              int score, int row_start, int col_start, int tiles_played,
              int vertical, int move_type);
void set_move_as_pass(Move *move);
void set_spare_move_as_pass(MoveList *ml);

#endif