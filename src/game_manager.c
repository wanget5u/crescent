#include <game_manager.h>
#include <tools.h>
#include <stdio.h>
#include <time.h>

void game_manager_init(GameManager* game_manager) {
    game_manager -> game_state = RUNNING;
    print_hour();
    printf(" initialised\n");
}

void game_manager_update(GameManager* game_manager, f32 delta_time) {
    (void) game_manager;
    (void) delta_time;
}

void game_manager_render(GameManager* game_manager) {

}

void game_manager_cleanup(GameManager* game_manager) {

}