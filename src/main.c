#include <raylib.h>
#include <stdio.h>

#include "game_manager.h"
#include "tools.h"

void on_update(GameManager* game_manager);

int main() {
    GameManager game_manager;
    game_manager_init(&game_manager);
    on_update(&game_manager);
    return 0;
}

void on_update(GameManager* game_manager) {
    log_msg("looping");
    while (game_manager -> game_state == RUNNING && !WindowShouldClose()) {
        game_manager -> delta_time = GetFrameTime();
        game_manager_update(game_manager);
        game_manager_render(game_manager);
    }
    game_manager_cleanup(game_manager);
}