#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include "game_manager.h"

static void window_init() {
    InitWindow(BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT, "Crescent");
    SetWindowMinSize(MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);
    SetWindowState(FLAG_VSYNC_HINT);
}

static void shader_init(GameManager* game_manager) {
    game_manager->grid_shader = LoadShader("./assets/shaders/world_grid.vs", "assets/shaders/world_grid.fs");
    game_manager->grid_cam_pos = GetShaderLocation(game_manager->grid_shader, "camPos");
}

static void handle_window_resize(GameManager* game_manager) {
    if (IsWindowResized()) {
        ui_manager_resize(&game_manager->user_interface, (f32)GetScreenWidth(), (f32)GetScreenHeight());
    }
}

static void handle_toggle_fullscreen(GameManager* game_manager) {
    if (input_is_pressed(&game_manager->input, ACTION_TOGGLE_FULLSCREEN)) {
        i32 display = GetCurrentMonitor();
        if (IsWindowFullscreen()) {
            ToggleFullscreen();
            SetWindowSize(game_manager->previous_screen_width, game_manager->previous_screen_height);
            ui_manager_resize(&game_manager->user_interface, (f32)game_manager->previous_screen_width, (f32)game_manager->previous_screen_height);
        } else {
            game_manager->previous_screen_width = GetScreenWidth();
            game_manager->previous_screen_height = GetScreenHeight();
            i32 monitorWidth = GetMonitorWidth(display);
            i32 monitorHeight = GetMonitorHeight(display);
            SetWindowSize(monitorWidth, monitorHeight);
            ToggleFullscreen();
            ui_manager_resize(&game_manager->user_interface, (f32)monitorWidth, (f32)monitorHeight);
        }
    }
}

void game_manager_init(GameManager* game_manager) {
    game_manager->game_state = STATE_RUNNING;
    log_msg("initialising");
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    window_init();
    shader_init(game_manager);
    input_init(&game_manager->input);
    player_init(&game_manager->player);
    ui_manager_init(&game_manager->user_interface, &game_manager->player, game_manager->grid_shader, game_manager->grid_cam_pos);
}

void game_manager_update(GameManager* game_manager) {
    handle_window_resize(game_manager);
    input_update(&game_manager->input);
    handle_toggle_fullscreen(game_manager);
    ui_manager_update(&game_manager->user_interface, &game_manager->input, game_manager->delta_time);
}

void game_manager_render(GameManager* game_manager) {
    ui_manager_render(&game_manager->user_interface);
}

void game_manager_cleanup(GameManager* game_manager) {
    ui_manager_cleanup(&game_manager->user_interface);
    UnloadShader(game_manager->grid_shader);
    CloseWindow();
    log_msg("closing");
}
