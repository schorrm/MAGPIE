#include <assert.h>
#include <stdio.h>

#include "../src/config.h"

#include "test_util.h"
#include "superconfig.h"

void test_alphabet(SuperConfig * superconfig) {
    Config * config = get_nwl_config(superconfig);
    // Test blank
    assert(get_blanked_machine_letter(1) == (1 | BLANK_MASK));
    assert(get_blanked_machine_letter(5) == (5 | BLANK_MASK));

    // Test unblank
    assert(get_unblanked_machine_letter(1) == (1 & UNBLANK_MASK));
    assert(get_unblanked_machine_letter(5) == (5 & UNBLANK_MASK));

    // Test val
    // blank
    assert(human_readable_letter_to_machine_letter(config->letter_distribution, BLANK_TOKEN) == BLANK_MACHINE_LETTER);
    // blank
    assert(human_readable_letter_to_machine_letter(config->letter_distribution, 'a') == get_blanked_machine_letter(1));
    assert(human_readable_letter_to_machine_letter(config->letter_distribution, 'b') == get_blanked_machine_letter(2));
    // not blank
    assert(human_readable_letter_to_machine_letter(config->letter_distribution, 'C') == 3);
    assert(human_readable_letter_to_machine_letter(config->letter_distribution, 'D') == 4);

    // Test user visible
    // separation token
    // The separation letter and machine letter should be the only machine
    // letters that map to the same value, since
    // blank
    assert(machine_letter_to_human_readable_letter(config->letter_distribution, BLANK_MACHINE_LETTER) == BLANK_TOKEN);
    // blank
    assert(machine_letter_to_human_readable_letter(config->letter_distribution, get_blanked_machine_letter(1)) == 'a');
    assert(machine_letter_to_human_readable_letter(config->letter_distribution, get_blanked_machine_letter(2)) == 'b');
    // not blank
    assert(machine_letter_to_human_readable_letter(config->letter_distribution, 3) == 'C');
    assert(machine_letter_to_human_readable_letter(config->letter_distribution, 4) == 'D');

}
