#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/board.h"
#include "../src/config.h"
#include "../src/constants.h"
#include "../src/game.h"
#include "../src/gameplay.h"
#include "../src/klv.h"
#include "../src/move.h"
#include "../src/rack.h"
#include "../src/util.h"

#include "game_print.h"
#include "move_print.h"
#include "test_constants.h"
#include "test_util.h"

void write_string_to_end_of_buffer(char *buffer, char *s) {
  sprintf(buffer + strlen(buffer), "%s", s);
}

void write_spaces_to_end_of_buffer(char *buffer, int n) {
  sprintf(buffer + strlen(buffer), "%*s", n, "");
}

void write_int_to_end_of_buffer(char *buffer, int n) {
  sprintf(buffer + strlen(buffer), "%d", n);
}

void write_char_to_end_of_buffer(char *buffer, char c) {
  sprintf(buffer + strlen(buffer), "%c", c);
}

void write_float_to_end_of_buffer(char *buffer, double d) {
  sprintf(buffer + strlen(buffer), "%0.2f", d);
}

void reset_string(char *string) { memset(string, 0, sizeof(*string)); }

int within_epsilon(double a, double b) { return fabs(a - b) < 1e-6; }

double get_leave_value_for_rack(KLV *klv, Rack *rack) {
  return get_leave_value(klv, rack);
}

void generate_moves_for_game(Game *game) {
  generate_moves(game->gen, game->players[game->player_on_turn_index],
                 game->players[1 - game->player_on_turn_index]->rack,
                 game->gen->bag->last_tile_index + 1 >= RACK_SIZE);
}

SortedMoveList *create_sorted_move_list(MoveList *ml) {
  int number_of_moves = ml->count;
  SortedMoveList *sorted_move_list = malloc((sizeof(SortedMoveList)));
  sorted_move_list->moves = malloc((sizeof(Move *)) * (number_of_moves));
  sorted_move_list->count = number_of_moves;
  for (int i = number_of_moves - 1; i >= 0; i--) {
    Move *move = pop_move(ml);
    sorted_move_list->moves[i] = move;
  }
  return sorted_move_list;
}

void destroy_sorted_move_list(SortedMoveList *sorted_move_list) {
  free(sorted_move_list->moves);
  free(sorted_move_list);
}

void print_anchor_list(Generator *gen) {
  for (int i = 0; i < gen->anchor_list->count; i++) {
    Anchor *anchor = gen->anchor_list->anchors[i];
    int row = anchor->row;
    int col = anchor->col;
    char *dir = "Horizontal";
    if (anchor->vertical) {
      row = anchor->col;
      col = anchor->row;
      dir = "Vertical";
    }
    printf("Anchor %d: Row %d, Col %d, %s, %0.4f, %d\n", i, row, col, dir,
           anchor->highest_possible_equity, anchor->last_anchor_col);
  }
}

void print_move_list(Board *board, LetterDistribution *letter_distribution,
                     SortedMoveList *sml, int move_list_length) {
  for (int i = 0; i < move_list_length; i++) {
    char move_string[40];
    reset_string(move_string);
    write_user_visible_move_to_end_of_buffer(move_string, board, sml->moves[i],
                                             letter_distribution);
    printf("%s\n", move_string);
    printf("equity: %f\n", sml->moves[i]->equity);
  }
}

void sort_and_print_move_list(Board *board,
                              LetterDistribution *letter_distribution,
                              MoveList *ml) {
  SortedMoveList *sml = create_sorted_move_list(ml);
  print_move_list(board, letter_distribution, sml, ml->count);
  destroy_sorted_move_list(sml);
}

void play_top_n_equity_move(Game *game, int n) {
  generate_moves(game->gen, game->players[game->player_on_turn_index],
                 game->players[1 - game->player_on_turn_index]->rack,
                 game->gen->bag->last_tile_index + 1 >= RACK_SIZE);
  SortedMoveList *sorted_move_list =
      create_sorted_move_list(game->gen->move_list);
  play_move(game, sorted_move_list->moves[n]);
  destroy_sorted_move_list(sorted_move_list);
  reset_move_list(game->gen->move_list);
}

void draw_rack_to_string(Bag *bag, Rack *rack, char *letters,
                         LetterDistribution *letter_distribution) {

  uint8_t mls[50];
  int num_mls =
      str_to_machine_letters(letter_distribution, letters, false, mls);
  for (int i = 0; i < num_mls; i++) {
    draw_letter_to_rack(bag, rack, mls[i]);
  }
}

int count_newlines(const char *str) {
  int count = 0;

  while (*str != '\0') {
    if (*str == '\n') {
      count++;
    }
    str++;
  }
  return count;
}

void assert_strings_equal(char *str1, char *str2) {
  if (strcmp(str1, str2) != 0) {
    fprintf(stderr, "strings are not equal: %s != %s", str1, str2);
    assert(0);
  }
}