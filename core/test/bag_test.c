#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/config.h"
#include "../src/gameplay.h"
#include "../src/letter_distribution.h"

#include "bag_print.h"
#include "superconfig.h"
#include "test_util.h"

void write_bag_to_string(char *bag_string, Bag *bag,
                         LetterDistribution *letter_distribution) {
  reset_string(bag_string);
  write_bag_to_end_of_buffer(bag_string, bag, letter_distribution);
}

void test_add_letter(Config *config, Bag *bag, char *r,
                     char *expected_bag_string) {
  char bag_string[100];
  reset_string(bag_string);

  add_letter(bag, human_readable_letter_to_machine_letter(
                      config->letter_distribution, r));
  write_bag_to_string(bag_string, bag, config->letter_distribution);
  assert(!strcmp(bag_string, expected_bag_string));
}

void test_bag(SuperConfig *superconfig) {
  Config *config = get_nwl_config(superconfig);
  Bag *bag = create_bag(config->letter_distribution);
  Rack *rack = create_rack(config->letter_distribution->size);

  // Check initial bag configuration
  assert(bag->last_tile_index == BAG_SIZE - 1);

  for (uint32_t i = 0; i < (config->letter_distribution->size); i++) {
    uint32_t number_of_letters = 0;
    for (int k = 0; k <= bag->last_tile_index; k++) {
      if (bag->tiles[k] == i) {
        number_of_letters++;
      }
    }
    assert(config->letter_distribution->distribution[i] == number_of_letters);
  }

  int number_of_remaining_tiles = bag->last_tile_index + 1;

  // Check drawing from the bag
  while (bag->last_tile_index + 1 > RACK_SIZE) {
    draw_at_most_to_rack(bag, rack, RACK_SIZE);
    number_of_remaining_tiles -= RACK_SIZE;
    assert(bag->last_tile_index + 1 == number_of_remaining_tiles);
    assert(!rack->empty);
    assert(rack->number_of_letters == RACK_SIZE);
    reset_rack(rack);
  }

  draw_at_most_to_rack(bag, rack, RACK_SIZE);
  assert(bag->last_tile_index == -1);
  assert(!rack->empty);
  assert(rack->number_of_letters == number_of_remaining_tiles);
  reset_rack(rack);

  // Check adding letters to the bag

  test_add_letter(config, bag, "A", "A");
  test_add_letter(config, bag, "F", "AF");
  test_add_letter(config, bag, "Z", "AFZ");
  test_add_letter(config, bag, "B", "ABFZ");
  test_add_letter(config, bag, "a", "ABFZ?");
  test_add_letter(config, bag, "b", "ABFZ??");
  test_add_letter(config, bag, "z", "ABFZ???");

  destroy_bag(bag);
  destroy_rack(rack);
}
