#include "game_manager.h"
#include <stdio.h>

int main() {
    GameManager game_manager;
    f32 delta_time = 0.0f;
    game_manager_init(&game_manager);
    game_manager_update(&game_manager, delta_time);
    game_manager_render(&game_manager);
    game_manager_cleanup(&game_manager);
    return 0;
}