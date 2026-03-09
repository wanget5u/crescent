#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/rlgl.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "game_manager.h"

static void environment_render(GameManager* game_manager, Vec3 camera_pos) {
    SetShaderValue(game_manager -> grid_shader, game_manager -> grid_cam_pos, &camera_pos, SHADER_UNIFORM_VEC3);
    BeginBlendMode(BLEND_ALPHA);
    BeginShaderMode(game_manager -> grid_shader);
    rlDisableBackfaceCulling();
    rlPushMatrix();
    rlTranslatef(camera_pos.x, 0.0f, camera_pos.z);
    rlBegin(RL_QUADS); 
        rlVertex3f(-GRID_SCALE, 0.0f,-GRID_SCALE);
        rlVertex3f(-GRID_SCALE, 0.0f, GRID_SCALE);
        rlVertex3f( GRID_SCALE, 0.0f, GRID_SCALE);
        rlVertex3f( GRID_SCALE, 0.0F,-GRID_SCALE);
    rlEnd();
    rlPopMatrix();
    EndShaderMode();
    EndBlendMode();
}

static void render_ui(GameManager* game_manager) {
    DrawTextEx(game_manager -> font, TextFormat("%i FPS", GetFPS()), (Vec2) {10.0f, 40.0f}, FONT_SIZE, FONT_SPACING, TEXT_COLOR);
    DrawTextEx(game_manager -> font, "Game", (Vec2){10.0f, 10.0f}, FONT_SIZE, FONT_SPACING, TEXT_COLOR);
    const char* coordinates_text = TextFormat("x: %.2f, y: %.2f, z: %.2f", 
        game_manager->player.position.x, 
        game_manager->player.position.y, 
        game_manager->player.position.z
    );
    DrawTextEx(game_manager -> font, coordinates_text, (Vec2) {10.0f, (f32) GetScreenHeight() - 25.0f}, FONT_SIZE, FONT_SPACING, TEXT_COLOR);
    DrawTextEx(game_manager -> font, "Editor", (Vector2){ ((f32) GetScreenWidth() / 2.0f) + 10.0f, 10.0f}, FONT_SIZE, FONT_SPACING, TEXT_COLOR);
}

static void render_game_view(GameManager* game_manager) {
    game_view_begin_render(&game_manager -> game_view);
    environment_render(game_manager, game_manager -> game_view.camera.rl_camera.position);
    game_view_end_render(&game_manager -> game_view);
}

static void render_editor_view(GameManager* game_manager) {
    editor_view_begin_render(&game_manager -> editor_view);
    environment_render(game_manager, game_manager -> editor_view.camera.rl_camera.position);
    player_render(&game_manager -> player);
    editor_view_end_render(&game_manager -> editor_view);
}

static void draw_split_screen(GameManager* game_manager) {
    Rectangle source_rect = {0.0f, 0.0f, (f32) GetScreenWidth() / 2.0f, (f32) -GetScreenHeight()};
    DrawTextureRec(game_manager -> game_view.view.texture, source_rect, (Vec2) {0.0f, 0.0f}, WHITE);
    DrawTextureRec(game_manager -> editor_view.view.texture, source_rect, (Vec2) {(f32) GetScreenWidth() / 2.0f, 0.0f}, WHITE);
}

static void font_init(GameManager* game_manager) {
    game_manager -> font = LoadFontEx("./assets/fonts/JetBrainsMono-Regular.ttf", 64, NULL, 0);
    SetTextureFilter(game_manager -> font.texture, TEXTURE_FILTER_BILINEAR);
}

static void shader_init(GameManager* game_manager) {
    game_manager -> grid_shader = LoadShader("./assets/shaders/world_grid.vs", "assets/shaders/world_grid.fs");
    game_manager -> grid_cam_pos = GetShaderLocation(game_manager -> grid_shader, "camPos");
}

void game_manager_init(GameManager* game_manager) {
    game_manager -> game_state = STATE_RUNNING;
    SetTraceLogLevel(LOG_WARNING);
    log_msg("initialising");
    InitWindow(1280, 720, "Crescent");
    SetTargetFPS(10000);
    shader_init(game_manager);
    font_init(game_manager);
    player_init(&game_manager -> player);
    game_view_init(&game_manager -> game_view);
    editor_view_init(&game_manager -> editor_view);
}

void game_manager_handle_input(GameManager* game_manager) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        i32 click_x = GetMouseX();
        DisableCursor();
        if (click_x < GetScreenWidth() / 2) {
            game_manager -> current_focus = FOCUS_GAME;
        } else {
            game_manager -> current_focus = FOCUS_EDITOR;
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        EnableCursor();
        game_manager -> current_focus = FOCUS_NONE;
    }
    game_manager -> player.input_direction = (Vec3) {0.0f, 0.0f, 0.0f};
    if (game_manager -> current_focus == FOCUS_GAME) {
        game_view_handle_input(&game_manager -> game_view, &game_manager -> player);
    } else if (game_manager -> current_focus == FOCUS_EDITOR) {
        editor_view_handle_input(&game_manager -> editor_view, game_manager -> delta_time);
    }
}

void game_manager_update(GameManager* game_manager) {
    game_manager_handle_input(game_manager);
    player_update(&game_manager -> player, &game_manager -> game_view.camera.yaw, game_manager -> delta_time);
    bool is_game_focused = (game_manager -> current_focus == FOCUS_GAME);
    bool is_editor_focused = (game_manager -> current_focus == FOCUS_EDITOR);
    camera_update(&game_manager->game_view.camera, &game_manager->player.position, is_game_focused, game_manager->delta_time);
    camera_update(&game_manager->editor_view.camera, &game_manager->editor_view.camera.rl_camera.position, is_editor_focused, game_manager->delta_time);
}

void game_manager_render(GameManager* game_manager) {
    render_game_view(game_manager);
    render_editor_view(game_manager);
    BeginDrawing();
    ClearBackground(BG_COLOR); 
    draw_split_screen(game_manager);
    render_ui(game_manager);
    EndDrawing();
}

void game_manager_cleanup(GameManager* game_manager) {
    UnloadShader(game_manager -> grid_shader);
    UnloadFont(game_manager -> font);
    CloseWindow();
    log_msg("closing");
}