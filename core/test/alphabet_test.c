#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/config.h"

#include "superconfig.h"
#include "test_util.h"

void test_alphabet(SuperConfig *superconfig) {
  Config *config = get_nwl_config(superconfig);
  // Test blank
  assert(get_blanked_machine_letter(1) == (1 | BLANK_MASK));
  assert(get_blanked_machine_letter(5) == (5 | BLANK_MASK));

  // Test unblank
  assert(get_unblanked_machine_letter(1) == (1 & UNBLANK_MASK));
  assert(get_unblanked_machine_letter(5) == (5 & UNBLANK_MASK));

  // Test val
  // blank
  assert(human_readable_letter_to_machine_letter(config->letter_distribution,
                                                 "?") == BLANK_MACHINE_LETTER);
  // blank
  assert(human_readable_letter_to_machine_letter(config->letter_distribution,
                                                 "a") ==
         get_blanked_machine_letter(1));
  assert(human_readable_letter_to_machine_letter(config->letter_distribution,
                                                 "b") ==
         get_blanked_machine_letter(2));
  // not blank
  assert(human_readable_letter_to_machine_letter(config->letter_distribution,
                                                 "C") == 3);
  assert(human_readable_letter_to_machine_letter(config->letter_distribution,
                                                 "D") == 4);

  // Test user visible
  // separation token
  // The separation letter and machine letter should be the only machine
  // letters that map to the same value, since
  StringBuilder *letter = create_string_builder();

  // blank
  string_builder_add_user_visible_letter(config->letter_distribution,
                                         BLANK_MACHINE_LETTER, 0, letter);
  assert_strings_equal(string_builder_peek(letter), "?");
  string_builder_clear(letter);

  // blank A
  string_builder_add_user_visible_letter(
      config->letter_distribution, get_blanked_machine_letter(1), 0, letter);
  assert_strings_equal(string_builder_peek(letter), "a");
  string_builder_clear(letter);

  string_builder_add_user_visible_letter(
      config->letter_distribution, get_blanked_machine_letter(2), 0, letter);
  assert_strings_equal(string_builder_peek(letter), "b");
  string_builder_clear(letter);

  // not blank
  string_builder_add_user_visible_letter(config->letter_distribution, 3, 0,
                                         letter);
  assert_strings_equal(string_builder_peek(letter), "C");
  string_builder_clear(letter);
  string_builder_add_user_visible_letter(config->letter_distribution, 4, 0,
                                         letter);
  assert_strings_equal(string_builder_peek(letter), "D");
  string_builder_clear(letter);

  Config *catalan_config = get_disc_config(superconfig);
  string_builder_add_user_visible_letter(catalan_config->letter_distribution,
                                         get_blanked_machine_letter(13), 0,
                                         letter);
  assert_strings_equal(string_builder_peek(letter), "l·l");
  string_builder_clear(letter);

  destroy_string_builder(letter);
}
