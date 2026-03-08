#pragma once

#include "core.h"

typedef enum {
    RUNNING,
    PAUSED
} GameState;

typedef struct {
    GameState game_state;
    f32 delta_time;
} GameManager;

void game_manager_init(GameManager* game_manager);
void game_manager_update(GameManager* game_manager, f32 delta_time);
void game_manager_render(GameManager* game_manager);
void game_manager_cleanup(GameManager* game_manager);