
#include <stdint.h>

#include "game_history.h"
#include "move.h"
#include "rack.h"

GameHistory *create_game_history() {
  GameHistory *game_history = malloc(sizeof(GameHistory));
  game_history->title = NULL;
  game_history->description = NULL;
  game_history->id_auth = NULL;
  game_history->uid = NULL;
  game_history->lexicon = NULL;
  game_history->board_layout = BOARD_LAYOUT_CROSSWORD_GAME;
  game_history->players[0] = NULL;
  game_history->players[1] = NULL;
  game_history->number_of_events = 0;
  game_history->events = malloc(sizeof(GameEvent) * (MAX_GAME_EVENTS));
  return game_history;
}

void destroy_game_history(GameHistory *game_history) {
  if (game_history->title != NULL) {
    free(game_history->title);
  }
  if (game_history->description != NULL) {
    free(game_history->description);
  }
  if (game_history->id_auth != NULL) {
    free(game_history->id_auth);
  }
  if (game_history->uid != NULL) {
    free(game_history->uid);
  }
  if (game_history->lexicon != NULL) {
    free(game_history->lexicon);
  }

  for (int i = 0; i < 2; i++) {
    if (game_history->players[i] != NULL) {
      destroy_player(game_history->players[i]);
    }
  }
  for (int i = 0; i < game_history->number_of_events; i++) {
    destroy_game_event(game_history->events[i]);
  }
  free(game_history->events);
  free(game_history);
}

GameEvent *create_game_event(GameHistory *game_history) {
  if (game_history->number_of_events == MAX_GAME_EVENTS) {
    log_fatal("Game events overflow.");
  }
  GameEvent *game_event = malloc(sizeof(GameEvent));
  game_event->cumulative_score = 0;
  game_event->move = NULL;
  game_event->rack = NULL;
  game_event->note = NULL;
  game_history->events[game_history->number_of_events++] = game_event;
  return game_event;
}

void destroy_game_event(GameEvent *game_event) {
  if (game_event->move != NULL) {
    destroy_move(game_event->move);
  }
  if (game_event->rack != NULL) {
    destroy_rack(game_event->rack);
  }
  if (game_event->note != NULL) {
    free(game_event->note);
  }
  free(game_event);
}

GameHistoryPlayer *create_game_history_player(const char *name,
                                              const char *nickname) {
  GameHistoryPlayer *player = malloc(sizeof(GameHistoryPlayer));
  player->name = strdup(name);
  player->nickname = strdup(nickname);
  player->score = 0;
  return player;
}

void destroy_game_history_player(GameHistoryPlayer *player) {
  free(player->name);
  free(player->nickname);
  if (player->last_known_rack != NULL) {
    destroy_rack(player->last_known_rack);
  }
  free(player);
}