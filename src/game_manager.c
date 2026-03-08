#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "game_manager.h"

static void render_environment(GameManager* game_manager) {
    f32 spacing = 2.0f;
    i32 count = 100;
    f32 snap_x = roundf(game_manager -> player.position.x / spacing) * spacing;
    f32 snap_z = roundf(game_manager -> player.position.z / spacing) * spacing;
    rlPushMatrix();
        rlTranslatef(snap_x, 0.0f, snap_z);
        for (i32 x = -count / 2; x <= count / 2; x++) {
            DrawLine3D((Vec3) {(f32) x * spacing, 0, (f32) -count }, (Vec3){(f32) x * spacing, 0, (f32) count }, WIRE_COLOR);
            DrawLine3D((Vec3) {(f32) -count, 0, (f32) x * spacing }, (Vec3){(f32) count, 0, (f32) x * spacing }, WIRE_COLOR);
        }
    rlPopMatrix();
}

static void render_ui(GameManager* game_manager) {
    (void) game_manager;
    const char* fps_text = TextFormat("%i FPS", GetFPS());
    DrawText(fps_text, 10, 10, 30, TEXT_COLOR);
}

void game_manager_init(GameManager* game_manager) {
    game_manager -> game_state = RUNNING;
    SetTraceLogLevel(LOG_WARNING);
    log_msg("initialising");
    InitWindow(1280, 720, "Crescent");
    SetTargetFPS(10000);
    player_init(&game_manager -> player);
    camera_init(&game_manager -> camera);
}

void game_manager_handle_input(GameManager* game_manager) {
    Vec3 input_direction = {0.0f, 0.0f, 0.0f};
    Vec3 forward = Vector3Subtract(game_manager -> camera.rl_camera.target, game_manager -> camera.rl_camera.position);
    forward.y = 0;
    forward = Vector3Normalize(forward);
    Vec3 right = Vector3CrossProduct(forward, game_manager->camera.rl_camera.up);
    if (IsKeyDown(KEY_W)) {
        input_direction = Vector3Add(input_direction, forward);
    }
    if (IsKeyDown(KEY_S)) {
        input_direction = Vector3Subtract(input_direction, forward);
    }
    if (IsKeyDown(KEY_D)) {
        input_direction = Vector3Add(input_direction, right);
    }
    if (IsKeyDown(KEY_A)) {
        input_direction = Vector3Subtract(input_direction, right);
    }
    game_manager -> player.input_direction = Vector3Normalize(input_direction);
}

void game_manager_update(GameManager* game_manager) {
    game_manager_handle_input(game_manager);
    player_update(&game_manager -> player, &game_manager -> camera.yaw, game_manager -> delta_time);
    camera_update(&game_manager -> camera, &game_manager -> player.position, game_manager -> delta_time);
}

void game_manager_render(GameManager* game_manager) {
    BeginDrawing();
    ClearBackground(BG_COLOR); 
    BeginMode3D(game_manager -> camera.rl_camera);
    render_environment(game_manager);
    player_render(&game_manager -> player);
    EndMode3D();
    render_ui(game_manager);
    EndDrawing();
}

void game_manager_cleanup(GameManager* game_manager) {
    (void) game_manager;
    CloseWindow();
    log_msg("closing");
}