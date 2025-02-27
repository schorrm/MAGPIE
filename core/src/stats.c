#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "stats.h"

void reset_stat(Stat *stat) {
  stat->cardinality = 0;
  stat->weight = 0;
  stat->mean = 0;
  stat->sum_of_mean_differences_squared = 0;
}

Stat *create_stat() {
  Stat *stat = malloc(sizeof(Stat));
  reset_stat(stat);
  return stat;
}

void destroy_stat(Stat *stat) { free(stat); }

Stat *copy_stat(Stat *original_stat) {
  Stat *copy_stat = create_stat();
  copy_stat->cardinality = original_stat->cardinality;
  copy_stat->weight = original_stat->weight;
  copy_stat->sum_of_mean_differences_squared =
      original_stat->sum_of_mean_differences_squared;
  copy_stat->mean = original_stat->mean;
  return copy_stat;
}

void push_with_cardinality(Stat *stat, double value, uint64_t value_weight,
                           uint64_t cardinality) {
  stat->cardinality += cardinality;
  stat->weight += value_weight;
  double old_mean = stat->mean;
  double value_minus_old_mean = ((double)value) - old_mean;
  stat->mean =
      old_mean + (((double)value_weight) / stat->weight) * value_minus_old_mean;
  stat->sum_of_mean_differences_squared =
      stat->sum_of_mean_differences_squared +
      ((double)value_weight) * value_minus_old_mean *
          (((double)value) - stat->mean);
}

void push(Stat *stat, double value, uint64_t value_weight) {
  push_with_cardinality(stat, value, value_weight, 1);
}

uint64_t get_cardinality(Stat *stat) { return stat->cardinality; }

uint64_t get_weight(Stat *stat) { return stat->weight; }

double get_mean(Stat *stat) { return stat->mean; }

void push_stat(Stat *stat_1, Stat *stat_2) {
  push_with_cardinality(stat_1, get_mean(stat_2), get_weight(stat_2),
                        get_cardinality(stat_2));
}

double get_variance(Stat *stat) {
  if (stat->weight <= 1) {
    return 0.0;
  }
  // BIAS_FIXME: maybe use weight - 1 instead
  return stat->sum_of_mean_differences_squared / (((double)stat->weight));
}

double get_stdev(Stat *stat) { return sqrt(get_variance(stat)); }

void combine_stats(Stat **stats, int number_of_stats, Stat *combined_stat) {
  uint64_t combined_cardinality = 0;
  uint64_t combined_weight = 0;
  double combined_mean = 0;
  for (int i = 0; i < number_of_stats; i++) {
    uint64_t cardinality = get_cardinality(stats[i]);
    combined_cardinality += cardinality;
    uint64_t weight = get_weight(stats[i]);
    combined_weight += weight;
    combined_mean += get_mean(stats[i]) * weight;
  }
  if (combined_weight <= 0) {
    combined_stat->cardinality = 0;
    combined_stat->weight = 0;
    combined_stat->mean = 0;
    combined_stat->sum_of_mean_differences_squared = 0;
    return;
  }
  combined_mean = combined_mean / combined_weight;

  double combined_error_sum_of_squares = 0;
  for (int i = 0; i < number_of_stats; i++) {
    double stdev = get_stdev(stats[i]);
    uint64_t weight = get_weight(stats[i]);
    // BIAS_FIXME: maybe use weight - 1 instead
    combined_error_sum_of_squares += (stdev * stdev) * (weight);
  }

  double combined_sum_of_squares = 0;
  for (int i = 0; i < number_of_stats; i++) {
    uint64_t weight = get_weight(stats[i]);
    double mean = get_mean(stats[i]);
    double mean_diff = (mean - combined_mean);
    combined_sum_of_squares += (mean_diff * mean_diff) * weight;
  }
  double combined_sum_of_mean_differences_squared =
      combined_sum_of_squares + combined_error_sum_of_squares;

  combined_stat->cardinality = combined_cardinality;
  combined_stat->weight = combined_weight;
  combined_stat->mean = combined_mean;
  combined_stat->sum_of_mean_differences_squared =
      combined_sum_of_mean_differences_squared;
}

double get_standard_error(Stat *stat, double m) {
  return m * sqrt(get_variance(stat) / (double)stat->cardinality);
}

int round_to_nearest_int(double a) {
  return (int)(a + 0.5 - (a < 0)); // truncated to 55
}