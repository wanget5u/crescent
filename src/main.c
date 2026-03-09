#include <raylib/raylib.h>
#include <stdio.h>

#include "game_manager.h"
#include "tools.h"

void on_update(GameManager* game_manager) {
    log_msg("running");
    while (game_manager -> game_state == STATE_RUNNING && !WindowShouldClose()) {
        game_manager -> delta_time = GetFrameTime();
        game_manager_update(game_manager);
        game_manager_render(game_manager);
    }
    game_manager_cleanup(game_manager);
}

int main() {
    log_msg("starting");
    GameManager game_manager;
    game_manager_init(&game_manager);
    on_update(&game_manager);
    return 0;
}