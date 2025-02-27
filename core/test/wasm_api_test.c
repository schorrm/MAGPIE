#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/game.h"
#include "../src/wasm_api.h"
#include "superconfig.h"
#include "test_constants.h"

const char *cgp1 = "4AUREOLED3/11O3/11Z3/10FY3/10A4/10C4/10I4/7THANX3/10GUV2/"
                   "15/15/15/15/15/15 AHMPRTU/ 177/44 0 lex CSW21; ld english;";

void test_wasm_api() {
  // play a phony 6G DI(PET)AZ keeping ERS.
  char *retstr = score_play(VS_ED, MOVE_TYPE_PLAY, 5, 6, 0,
                            (uint8_t[]){4, 9, 0, 0, 0, 1, 26},
                            (uint8_t[]){5, 18, 19}, 7, 3);

  // score is 57
  // equity of ERS is 15.947
  // -> total equity is 72.947
  printf("%s\n", retstr);
  assert(strcmp(retstr,
                "currmove 6g.DIPETAZ result scored valid false invalid_words "
                "WIFAY,ZGENUINE,DIPETAZ sc 57 eq 72.947") == 0);
  free(retstr);

  // Score an exchange keeping AEINR (equity is 12.610)
  retstr = score_play(VS_ED, MOVE_TYPE_EXCHANGE, 0, 0, 0, (uint8_t[]){17, 23},
                      (uint8_t[]){1, 5, 9, 14, 18}, 2, 5);
  assert(strcmp(retstr,
                "currmove ex.QW result scored valid true sc 0 eq 12.610") == 0);
  free(retstr);

  // Score another play
  retstr = score_play(cgp1, MOVE_TYPE_PLAY, 1, 1, 0,
                      (uint8_t[]){20, 8, 21, 13, 16}, (uint8_t[]){1, 18}, 5, 2);
  assert(strcmp(retstr,
                "currmove 2b.THUMP result scored valid true sc 50 eq 54.356") ==
         0);
  free(retstr);
}
