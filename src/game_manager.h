#pragma once

#include "core.h"
#include "player.h"
#include "game_camera.h"

typedef enum {
    RUNNING,
    PAUSED
} GameState;

typedef struct {
    GameState game_state;
    Player player;
    GameCamera camera;
    f32 delta_time;
} GameManager;

void game_manager_init(GameManager* game_manager);
void game_manager_update(GameManager* game_manager);
void game_manager_handle_input(GameManager* game_manager);
void game_manager_render(GameManager* game_manager);
void game_manager_cleanup(GameManager* game_manager);