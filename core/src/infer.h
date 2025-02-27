#ifndef INFER_h
#define INFER_h

#include <pthread.h>

#include "game.h"
#include "go_params.h"
#include "klv.h"
#include "leave_rack.h"
#include "move.h"
#include "rack.h"
#include "stats.h"
#include "thread_control.h"

typedef struct InferenceRecord {
  Stat *equity_values;
  uint64_t *draw_and_leave_subtotals;
} InferenceRecord;

typedef struct Inference {
  // InferenceRecords
  InferenceRecord *leave_record;
  InferenceRecord *exchanged_record;
  InferenceRecord *rack_record;
  LeaveRackList *leave_rack_list;

  // Recursive vars
  // Malloc'd by the game:
  Game *game;
  KLV *klv;
  Rack *player_to_infer_rack;
  // Malloc'd by inference:
  Rack *bag_as_rack;
  Rack *leave;
  Rack *exchanged;
  int distribution_size;
  int player_to_infer_index;
  int actual_score;
  int number_of_tiles_exchanged;
  int draw_and_leave_subtotals_size;
  int initial_tiles_to_infer;
  double equity_margin;
  uint64_t current_rack_index;
  int status;
  uint64_t total_racks_evaluated;
  // Multithreading fields
  uint64_t *shared_rack_index;
  pthread_mutex_t *shared_rack_index_lock;
  ThreadControl *thread_control;
} Inference;

void infer(ThreadControl *thread_control, Inference *inference, Game *game,
           Rack *actual_tiles_played, int player_to_infer_index,
           int actual_score, int number_of_tiles_exchanged,
           double equity_margin, int number_of_threads);
Inference *create_inference(int capacity, int distribution_size);
void destroy_inference(Inference *inference);
uint64_t get_subtotal(InferenceRecord *record, uint8_t letter,
                      int number_of_letters, int subtotal_index_offset);
uint64_t get_subtotal_sum_with_minimum(InferenceRecord *record, uint8_t letter,
                                       int minimum_number_of_letters,
                                       int subtotal_index_offset);
void get_stat_for_letter(InferenceRecord *record, Stat *stat, uint8_t letter);
double
get_probability_for_random_minimum_draw(Rack *bag_as_rack, Rack *rack,
                                        uint8_t this_letter, int minimum,
                                        int number_of_actual_tiles_played);
uint64_t choose(uint64_t n, uint64_t k);

#endif