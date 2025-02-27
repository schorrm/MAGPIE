#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../src/autoplay.h"
#include "../src/config.h"
#include "../src/log.h"
#include "../src/thread_control.h"

#include "alphabet_test.h"
#include "autoplay_test.h"
#include "bag_test.h"
#include "board_test.h"
#include "config_test.h"
#include "cross_set_test.h"
#include "equity_adjustment_test.h"
#include "game_test.h"
#include "gameplay_test.h"
#include "gcg_test.h"
#include "gen_all_test.h"
#include "infer_test.h"
#include "leave_map_test.h"
#include "leaves_test.h"
#include "letter_distribution_test.h"
#include "movegen_test.h"
#include "play_recorder_test.h"
#include "prof_tests.h"
#include "rack_test.h"
#include "shadow_test.h"
#include "sim_test.h"
#include "stats_test.h"
#include "superconfig.h"
#include "test_constants.h"
#include "ucgi_command_test.h"
#include "wasm_api_test.h"
#include "word_test.h"

void unit_tests(SuperConfig *superconfig) {
  // Test the loading of the config
  test_config();

  // Test the readonly data first
  test_alphabet(superconfig);
  test_letter_distribution(superconfig);
  test_str_to_machine_letters(superconfig);
  test_leaves(superconfig, "./data/lexica/CSW21.csv");
  test_leave_map(superconfig);

  // Now test the rest
  test_bag(superconfig);
  test_rack(superconfig);
  test_board(superconfig);
  test_cross_set(superconfig);
  test_game(superconfig);
  test_shadow(superconfig);
  test_movegen(superconfig);
  test_equity_adjustments(superconfig);
  test_gameplay(superconfig);
  test_stats();
  test_infer(superconfig);
  test_sim(superconfig);
  test_ucgi_command();
  test_gcg();
  test_autoplay(superconfig);
  test_wasm_api();
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("must specify exactly one command\n");
    exit(EXIT_FAILURE);
  }
  log_set_level(3);

  if (!strcmp(argv[1], CMD_GEN)) {
    Config *config = create_config_from_args(argc, argv);
    test_gen_all(config);
    destroy_config(config);
  } else if (!strcmp(argv[1], CMD_INFER)) {
    Config *config = create_config_from_args(argc, argv);
    infer_from_config(config);
    destroy_config(config);
  } else if (!strcmp(argv[1], CMD_AUTOPLAY)) {
    Config *config = create_config_from_args(argc, argv);
    ThreadControl *thread_control = create_thread_control_from_config(config);
    AutoplayResults *autoplay_results = create_autoplay_results();
    autoplay(thread_control, autoplay_results, config, time(NULL));
    destroy_config(config);
    destroy_autoplay_results(autoplay_results);
    destroy_thread_control(thread_control);
  } else if (!strcmp(argv[1], CMD_PROF)) {
    Config *config = create_config_from_args(argc, argv);
    prof_tests(config);
    destroy_config(config);
  } else if (!strcmp(argv[1], CMD_TOPVALL)) {
    Config *config = create_config_from_args(argc, argv);
    test_play_recorder(config);
    destroy_config(config);
  } else if (!strcmp(argv[1], CMD_SIM)) {
    Config *config = create_config_from_args(argc, argv);
    ThreadControl *thread_control = create_thread_control_from_config(config);
    perf_test_multithread_sim(config, thread_control);
    destroy_thread_control(thread_control);
    destroy_config(config);
  } else if (!strcmp(argv[1], CMD_SIM_STOPPING)) {
    Config *config = create_config_from_args(argc, argv);
    ThreadControl *thread_control = create_thread_control_from_config(config);
    thread_control->print_info_interval = 500;
    thread_control->check_stopping_condition_interval = 500;
    perf_test_multithread_blocking_sim(config, thread_control);
    destroy_thread_control(thread_control);
    destroy_config(config);
  } else if (!strcmp(argv[1], CMD_UNIT_TESTS)) {
    Config *csw_config = create_config(
        "./data/letterdistributions/english.csv", "", "./data/lexica/CSW21.kwg",
        "./data/lexica/CSW21.klv2", SORT_BY_EQUITY, PLAY_RECORDER_TYPE_ALL, "",
        "", -1, -1, 0, 10000, 0, 0, NULL, 0, 0, 0, 0, 1,
        "./data/strategy/default_english/winpct.csv", MOVE_LIST_CAPACITY);

    Config *nwl_config = create_config(
        "./data/letterdistributions/english.csv", "", "./data/lexica/NWL20.kwg",
        "./data/lexica/CSW21.klv2", SORT_BY_SCORE, PLAY_RECORDER_TYPE_ALL, "",
        "", -1, -1, 0, 10000, 0, 0, NULL, 0, 0, 0, 0, 1,
        "./data/strategy/default_english/winpct.csv", MOVE_LIST_CAPACITY);

    Config *osps_config = create_config(
        // no OSPS kwg yet, use later when we have tests.
        "./data/letterdistributions/polish.csv", "", "./data/lexica/OSPS44.kwg",
        "", SORT_BY_EQUITY, PLAY_RECORDER_TYPE_ALL, "", "", -1, -1, 0, 10000, 0,
        0, NULL, 0, 0, 0, 0, 1, "./data/strategy/default_english/winpct.csv",
        MOVE_LIST_CAPACITY);

    Config *disc_config = create_config(
        "./data/letterdistributions/catalan.csv", "", "./data/lexica/DISC2.kwg",
        "./data/lexica/catalan.klv2", SORT_BY_EQUITY, PLAY_RECORDER_TYPE_ALL,
        "", "", -1, -1, 0, 10000, 0, 0, NULL, 0, 0, 0, 0, 1,
        "./data/strategy/default_english/winpct.csv", MOVE_LIST_CAPACITY);

    Config *distinct_lexica_config = create_config(
        "./data/letterdistributions/english.csv", "", "./data/lexica/CSW21.kwg",
        "./data/lexica/CSW21.klv2", SORT_BY_EQUITY, PLAY_RECORDER_TYPE_ALL,
        "./data/lexica/NWL20.kwg", "", -1, -1, 0, 10000, 0, 0, NULL, 0, 0, 0, 0,
        1, "./data/strategy/default_english/winpct.csv", MOVE_LIST_CAPACITY);

    SuperConfig *superconfig =
        create_superconfig(csw_config, nwl_config, osps_config, disc_config,
                           distinct_lexica_config);
    unit_tests(superconfig);
    // This also frees the nested configs
    destroy_superconfig(superconfig);
  } else {
    printf("unrecognized command: %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }
}