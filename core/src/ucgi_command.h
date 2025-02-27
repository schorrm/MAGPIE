#ifndef UCGI_COMMAND_H
#define UCGI_COMMAND_H

#include "config.h"
#include "game.h"
#include "go_params.h"
#include "infer.h"
#include "sim.h"
#include "thread_control.h"

#define UCGI_COMMAND_STATUS_SUCCESS 0
#define UCGI_COMMAND_STATUS_PARSE_FAILED 1
#define UCGI_COMMAND_STATUS_NOT_STOPPED 2
#define UCGI_COMMAND_STATUS_LEXICON_LD_FAILURE 3
#define UCGI_COMMAND_STATUS_QUIT 4

typedef struct UCGICommandVars {
  Game *loaded_game;
  Config *config;
  Simmer *simmer;
  Inference *inference;
  GoParams *go_params;
  ThreadControl *thread_control;
  char last_lexicon_name[16];
  char last_ld_name[16];
  FILE *outfile;
} UCGICommandVars;

UCGICommandVars *create_ucgi_command_vars(FILE *outfile);
void destroy_ucgi_command_vars(UCGICommandVars *ucgi_command_vars);

int ucgi_go_async(char *go_cmd, UCGICommandVars *ucgi_command_vars);
int process_ucgi_command_async(char *cmd, UCGICommandVars *ucgi_command_vars);
void set_outfile(UCGICommandVars *ucgi_command_vars, FILE *outfile);

char *ucgi_search_status(UCGICommandVars *ucgi_command_vars);
char *ucgi_stop_search(UCGICommandVars *ucgi_command_vars);

#endif