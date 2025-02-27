#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "constants.h"
#include "klv.h"
#include "kwg.h"
#include "letter_distribution.h"
#include "log.h"
#include "util.h"

Config *create_config(const char *letter_distribution_filename, const char *cgp,
                      const char *kwg_filename_1, const char *klv_filename_1,
                      int move_sorting_1, int play_recorder_type_1,
                      const char *kwg_filename_2, const char *klv_filename_2,
                      int move_sorting_2, int play_recorder_type_2,
                      int game_pair_flag, int number_of_games_or_pairs,
                      int print_info, int checkstop,
                      const char *actual_tiles_played,
                      int player_to_infer_index, int actual_score,
                      int number_of_tiles_exchanged, double equity_margin,
                      int number_of_threads, const char *winpct_filename,
                      int move_list_capacity) {

  Config *config = malloc(sizeof(Config));
  config->letter_distribution =
      create_letter_distribution(letter_distribution_filename);
  strcpy(config->ld_filename, letter_distribution_filename);
  config->cgp = strdup(cgp);
  config->actual_tiles_played = create_rack(config->letter_distribution->size);
  if (actual_tiles_played != NULL) {
    set_rack_to_string(config->actual_tiles_played, actual_tiles_played,
                       config->letter_distribution);
  }
  config->use_game_pairs = game_pair_flag;
  config->number_of_games_or_pairs = number_of_games_or_pairs;
  config->print_info = print_info;
  config->checkstop = checkstop;
  config->player_to_infer_index = player_to_infer_index;
  config->actual_score = actual_score;
  config->number_of_tiles_exchanged = number_of_tiles_exchanged;
  config->equity_margin = equity_margin;
  config->number_of_threads = number_of_threads;

  StrategyParams *player_1_strategy_params = malloc(sizeof(StrategyParams));
  if (strcmp(kwg_filename_1, "") != 0) {
    player_1_strategy_params->kwg = create_kwg(kwg_filename_1);
    strcpy(player_1_strategy_params->kwg_filename, kwg_filename_1);
  } else {
    player_1_strategy_params->kwg = NULL;
  }

  if (strcmp(klv_filename_1, "") != 0) {
    player_1_strategy_params->klv = create_klv(klv_filename_1);
    strcpy(player_1_strategy_params->klv_filename, klv_filename_1);
  } else {
    player_1_strategy_params->klv = NULL;
  }
  player_1_strategy_params->move_sorting = move_sorting_1;
  player_1_strategy_params->play_recorder_type = play_recorder_type_1;

  config->player_1_strategy_params = player_1_strategy_params;

  StrategyParams *player_2_strategy_params = malloc(sizeof(StrategyParams));
  if (!strcmp(kwg_filename_2, "") || !strcmp(kwg_filename_2, kwg_filename_1)) {
    player_2_strategy_params->kwg = player_1_strategy_params->kwg;
    strcpy(player_2_strategy_params->kwg_filename, kwg_filename_1);
    config->kwg_is_shared = 1;
  } else {
    strcpy(player_2_strategy_params->kwg_filename, kwg_filename_2);
    player_2_strategy_params->kwg = create_kwg(kwg_filename_2);
    config->kwg_is_shared = 0;
  }

  if (!strcmp(klv_filename_2, "") || !strcmp(klv_filename_2, klv_filename_1)) {
    player_2_strategy_params->klv = player_1_strategy_params->klv;
    strcpy(player_2_strategy_params->klv_filename, klv_filename_1);
    config->klv_is_shared = 1;
  } else {
    strcpy(player_2_strategy_params->klv_filename, klv_filename_2);
    player_2_strategy_params->klv = create_klv(klv_filename_2);
    config->klv_is_shared = 0;
  }

  if (move_sorting_2 < 0) {
    player_2_strategy_params->move_sorting =
        player_1_strategy_params->move_sorting;
  } else {
    player_2_strategy_params->move_sorting = move_sorting_2;
  }

  if (play_recorder_type_2 < 0) {
    player_2_strategy_params->play_recorder_type =
        player_1_strategy_params->play_recorder_type;
  } else {
    player_2_strategy_params->play_recorder_type = play_recorder_type_2;
  }

  config->player_2_strategy_params = player_2_strategy_params;

  // XXX: do we want to do it this way? not consistent with rest of config.
  if (strcmp(winpct_filename, "")) {
    config->win_pcts = create_winpct(winpct_filename);
    strcpy(config->win_pct_filename, winpct_filename);
  } else {
    config->win_pcts = NULL;
  }
  config->move_list_capacity = move_list_capacity;
  return config;
}

void check_arg_length(const char *arg) {
  if (strlen(arg) > (MAX_ARG_LENGTH)-1) {
    printf("argument exceeded maximum size: %s\n", arg);
    exit(EXIT_FAILURE);
  }
}

Config *create_config_from_args(int argc, char *argv[]) {
  char letter_distribution_filename[(MAX_ARG_LENGTH)] = "";
  char cgp[(MAX_ARG_LENGTH)] = "";

  char kwg_filename_1[(MAX_ARG_LENGTH)] = "";
  char klv_filename_1[(MAX_ARG_LENGTH)] = "";
  int play_recorder_type_1 = PLAY_RECORDER_TYPE_ALL;
  int move_sorting_1 = SORT_BY_EQUITY;

  char kwg_filename_2[(MAX_ARG_LENGTH)] = "";
  char klv_filename_2[(MAX_ARG_LENGTH)] = "";
  int play_recorder_type_2 = -1;
  int move_sorting_2 = -1;

  int number_of_games_or_pairs = 0;
  int print_info = 0;
  int checkstop = 0;

  char actual_tiles_played[(MAX_ARG_LENGTH)] = "";
  int player_to_infer_index = -1;
  int actual_score = -1;
  int number_of_tiles_exchanged = 0;
  double equity_margin = -1.0;
  int number_of_threads = 1;

  char winpct_filename[(MAX_ARG_LENGTH)] = "";
  int use_game_pairs = 1;

  int c;
  long n;

  while (1) {
    static struct option long_options[] = {
        {"c", required_argument, 0, 1001},  {"d", required_argument, 0, 1002},
        {"g1", required_argument, 0, 1003}, {"l1", required_argument, 0, 1004},
        {"r1", required_argument, 0, 1005}, {"s1", required_argument, 0, 1006},
        {"g2", required_argument, 0, 1007}, {"l2", required_argument, 0, 1008},
        {"r2", required_argument, 0, 1009}, {"s2", required_argument, 0, 1010},
        {"n", required_argument, 0, 1011},  {"t", required_argument, 0, 1012},
        {"i", required_argument, 0, 1013},  {"a", required_argument, 0, 1014},
        {"e", required_argument, 0, 1015},  {"q", required_argument, 0, 1016},
        {"h", required_argument, 0, 1017},  {"w", required_argument, 0, 1018},
        {"f", required_argument, 0, 1019},  {"k", required_argument, 0, 1020},
        {"p", required_argument, 0, 1021},  {0, 0, 0, 0}};
    int option_index = 0;
    c = getopt_long_only(argc, argv, "", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1) {
      break;
    }

    switch (c) {
    case 1001:
      check_arg_length(optarg);
      strcpy(cgp, optarg);
      break;

    case 1002:
      check_arg_length(optarg);
      strcpy(letter_distribution_filename, optarg);
      break;

    case 1003:
      check_arg_length(optarg);
      strcpy(kwg_filename_1, optarg);
      break;

    case 1004:
      check_arg_length(optarg);
      strcpy(klv_filename_1, optarg);
      break;

    case 1005:
      check_arg_length(optarg);
      if (!strcmp("all", optarg)) {
        play_recorder_type_1 = PLAY_RECORDER_TYPE_ALL;
      } else if (!strcmp("top", optarg)) {
        // Not strictly necessary since this
        // is the default.
        play_recorder_type_1 = PLAY_RECORDER_TYPE_TOP_EQUITY;
      } else {
        printf("invalid play recorder option: %s\n", optarg);
        exit(EXIT_FAILURE);
      }
      break;

    case 1006:
      check_arg_length(optarg);
      if (!strcmp("score", optarg)) {
        move_sorting_1 = SORT_BY_SCORE;
      } else if (!strcmp("equity", optarg)) {
        // Not strictly necessary since this
        // is the default.
        move_sorting_1 = SORT_BY_EQUITY;
      } else {
        printf("invalid sort option: %s\n", optarg);
        exit(EXIT_FAILURE);
      }
      break;

    case 1007:
      check_arg_length(optarg);
      strcpy(kwg_filename_2, optarg);
      break;

    case 1008:
      check_arg_length(optarg);
      strcpy(klv_filename_2, optarg);
      break;

    case 1009:
      check_arg_length(optarg);
      if (!strcmp("all", optarg)) {
        play_recorder_type_2 = PLAY_RECORDER_TYPE_ALL;
      } else if (!strcmp("top", optarg)) {
        // Not strictly necessary since this
        // is the default.
        play_recorder_type_2 = PLAY_RECORDER_TYPE_TOP_EQUITY;
      } else {
        printf("invalid play recorder option: %s\n", optarg);
        exit(EXIT_FAILURE);
      }
      break;

    case 1010:
      check_arg_length(optarg);
      if (!strcmp("score", optarg)) {
        move_sorting_2 = SORT_BY_SCORE;
      } else if (!strcmp("equity", optarg)) {
        // Not strictly necessary since this
        // is the default.
        move_sorting_2 = SORT_BY_EQUITY;
      } else {
        printf("invalid sort option: %s\n", optarg);
        exit(EXIT_FAILURE);
      }
      break;

    case 1011:
      check_arg_length(optarg);
      n = strtol(optarg, NULL, 10);
      number_of_games_or_pairs = (int)n;
      break;

    case 1012:
      check_arg_length(optarg);
      strcpy(actual_tiles_played, optarg);
      break;

    case 1013:
      check_arg_length(optarg);
      n = strtol(optarg, NULL, 10);
      player_to_infer_index = (int)n;
      break;

    case 1014:
      check_arg_length(optarg);
      n = strtol(optarg, NULL, 10);
      actual_score = (int)n;
      break;

    case 1015:
      check_arg_length(optarg);
      n = strtol(optarg, NULL, 10);
      number_of_tiles_exchanged = (int)n;
      break;

    case 1016:
      check_arg_length(optarg);
      equity_margin = atof(optarg);
      break;

    case 1017:
      check_arg_length(optarg);
      n = strtol(optarg, NULL, 10);
      number_of_threads = (int)n;
      break;

    case 1018:
      check_arg_length(optarg);
      strcpy(winpct_filename, optarg);
      break;

    case 1019:
      check_arg_length(optarg);
      n = strtol(optarg, NULL, 10);
      print_info = (int)n;
      break;

    case 1020:
      check_arg_length(optarg);
      n = strtol(optarg, NULL, 10);
      checkstop = (int)n;
      break;

    case 1021:
      check_arg_length(optarg);
      n = strtol(optarg, NULL, 10);
      use_game_pairs = (int)n;
      break;

    case '?':
      /* getopt_long already printed an error message. */
      break;

    default:
      abort();
    }
  }

  return create_config(
      letter_distribution_filename, cgp, kwg_filename_1, klv_filename_1,
      move_sorting_1, play_recorder_type_1, kwg_filename_2, klv_filename_2,
      move_sorting_2, play_recorder_type_2, use_game_pairs,
      number_of_games_or_pairs, print_info, checkstop, actual_tiles_played,
      player_to_infer_index, actual_score, number_of_tiles_exchanged,
      equity_margin, number_of_threads, winpct_filename, MOVE_LIST_CAPACITY);
}

void destroy_config(Config *config) {
  if (config->player_1_strategy_params->klv != NULL) {
    destroy_klv(config->player_1_strategy_params->klv);
  }
  if (config->player_1_strategy_params->kwg != NULL) {
    destroy_kwg(config->player_1_strategy_params->kwg);
  }
  free(config->player_1_strategy_params);

  if (!config->klv_is_shared) {
    destroy_klv(config->player_2_strategy_params->klv);
  }
  if (!config->kwg_is_shared) {
    destroy_kwg(config->player_2_strategy_params->kwg);
  }
  free(config->player_2_strategy_params);

  destroy_letter_distribution(config->letter_distribution);
  destroy_rack(config->actual_tiles_played);
  free(config->cgp);

  destroy_winpct(config->win_pcts);

  free(config);
}

// potentially edit an existing config, or create a brand new one
void load_config_from_lexargs(Config **config, const char *cgp,
                              char *lexicon_name, char *ldname) {

  char dist[50];
  sprintf(dist, "data/letterdistributions/%s.csv", ldname);
  char leaves[50] = "data/lexica/english.klv2";
  char winpct[50] = "data/strategy/default_english/winpct.csv";
  char lexicon_file[50];
  sprintf(lexicon_file, "data/lexica/%s.kwg", lexicon_name);
  if (strcmp(lexicon_name, "CSW21") == 0) {
    strcpy(leaves, "data/lexica/CSW21.klv2");
  } else if (prefix("NSF", lexicon_name)) {
    strcpy(leaves, "data/lexica/norwegian.klv2");
  } else if (prefix("RD", lexicon_name)) {
    strcpy(leaves, "data/lexica/german.klv2");
  } else if (prefix("DISC", lexicon_name)) {
    strcpy(leaves, "data/lexica/catalan.klv2");
  } else if (prefix("FRA", lexicon_name)) {
    strcpy(leaves, "data/lexica/french.klv2");
  }

  if (*config == NULL) {
    *config = create_config(dist, cgp, lexicon_file, leaves, SORT_BY_EQUITY,
                            PLAY_RECORDER_TYPE_ALL, "", "", SORT_BY_EQUITY,
                            PLAY_RECORDER_TYPE_ALL, 0, 0, 9, 0, "", 0, 0, 0, 0,
                            0, winpct, 100);
  } else {
    Config *c = (*config);
    // check each filename
    if (strcmp(c->ld_filename, dist)) {
      // They're different; reload.
      log_debug("reloading letter distribution; was %s, new %s", c->ld_filename,
                dist);
      destroy_letter_distribution(c->letter_distribution);
      c->letter_distribution = create_letter_distribution(dist);
    }
    if (strcmp(c->player_1_strategy_params->kwg_filename, lexicon_file)) {
      log_debug("reloading kwg #1");
      destroy_kwg(c->player_1_strategy_params->kwg);
      c->player_1_strategy_params->kwg = create_kwg(lexicon_file);
      // assume the kwg applies to both players if we're using this function
      assert(c->kwg_is_shared);
      c->player_2_strategy_params->kwg = c->player_1_strategy_params->kwg;
      strcpy(c->player_2_strategy_params->kwg_filename, lexicon_file);
    }
    if (strcmp(c->player_1_strategy_params->klv_filename, leaves)) {
      log_debug("reloading klv #1");
      destroy_klv(c->player_1_strategy_params->klv);
      c->player_1_strategy_params->klv = create_klv(leaves);
      // assume the klv applies to both players if we're using this function
      assert(c->klv_is_shared);
      c->player_2_strategy_params->klv = c->player_1_strategy_params->klv;
      strcpy(c->player_2_strategy_params->klv_filename, leaves);
    }

    free(c->cgp);
    c->cgp = strdup(cgp);
  }
}

StrategyParams *copy_strategy_params(StrategyParams *orig) {
  StrategyParams *sp = malloc(sizeof(StrategyParams));
  // No need to copy the klv itself.
  sp->klv = orig->klv;
  sp->kwg = orig->kwg;
  strcpy(sp->klv_filename, orig->klv_filename);
  sp->move_sorting = orig->move_sorting;
  sp->play_recorder_type = orig->play_recorder_type;

  return sp;
}

void destroy_strategy_params(StrategyParams *sp) { free(sp); }