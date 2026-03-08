#include <raylib.h>
#include <stdio.h>
#include <time.h>

#include "game_manager.h"

void game_manager_init(GameManager* game_manager) {
    game_manager -> game_state = RUNNING;
    log_msg("initialised");
    InitWindow(1280, 720, "Crescent");
    SetTargetFPS(1000);
    player_init(&game_manager -> player);
}

void game_manager_handle_input(GameManager* game_manager) {
    Vec3 input_direction = { 0.0f, 0.0f, 0.0f };
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        input_direction.x += 1.0f;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        input_direction.x -= 1.0f;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        input_direction.y += 1.0f;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        input_direction.y -= 1.0f;
    }
    input_direction = Vector3Normalize(input_direction);
    game_manager -> player.input_direction = input_direction;
}

void game_manager_update(GameManager* game_manager) {
    game_manager_handle_input(game_manager);
    player_update(&game_manager -> player, game_manager -> delta_time);
}

void game_manager_render(GameManager* game_manager) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawRectangle(
        (i32)game_manager->player.position.x - 20,
        (i32)game_manager->player.position.y - 20, 
        40, 40, game_manager->player.color
    );
    DrawFPS(10, 10);
    EndDrawing();
}

void game_manager_cleanup(GameManager* game_manager) {
    (void) game_manager;
    CloseWindow();
    log_msg("closing");
}