#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "game_manager.h"

static void window_init() {
    InitWindow(BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT, "Crescent");
    SetWindowMinSize(MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);
    SetTargetFPS(10000);
}

static void draw_dock_tree(DockNode* node, Font font) {
    if (!node) {
        return;
    }
    if (node->type == DOCK_LEAF) {
        Rectangle source_rect = {0.0f, 0.0f, (f32)node->render_target.texture.width, (f32)-node->render_target.texture.height};
        Vec2 position = {node->bounds.x, node->bounds.y};
        DrawTextureRec(node->render_target.texture, source_rect, position, WHITE);
        if (node->content && node->content->title) {
            Vec2 text_size = MeasureTextEx(font, node->content->title, FONT_SIZE, FONT_SPACING);
            f32 padding = 5.0f;
            Rectangle title_bg = {
                node->bounds.x,
                node->bounds.y,
                text_size.x + (padding * 2.0f),
                text_size.y + (padding * 2.0f)
            };
            DrawRectangleRec(title_bg, TEXT_COLOR);
            Vec2 text_pos = {node->bounds.x + padding, node->bounds.y + padding};
            DrawTextEx(font, node->content->title, text_pos, FONT_SIZE, FONT_SPACING, RAYWHITE);
        }
    } else {
        draw_dock_tree(node->child_a, font);
        draw_dock_tree(node->child_b, font);
        Color splitter_color = DOCK_SPLITTER_COLOR;
        f32 visual_thickness = 4.0f;
        if (node->type == DOCK_SPLIT_HORIZONTAL) {
            f32 split_x = node->bounds.x + (node->bounds.width * node->split_ratio);
            DrawRectangle(
                (i32)(split_x - (visual_thickness / 2.0f)), 
                (i32)node->bounds.y, 
                (i32)visual_thickness, 
                (i32)node->bounds.height, 
                splitter_color
            );
        } else if (node->type == DOCK_SPLIT_VERTICAL) {
            f32 split_y = node->bounds.y + (node->bounds.height * node->split_ratio);
            DrawRectangle(
                (i32)node->bounds.x, 
                (i32)(split_y - (visual_thickness / 2.0f)), 
                (i32)node->bounds.width, 
                (i32)visual_thickness, 
                splitter_color
            );
        }
    }
}

static void render_ui(GameManager* game_manager) {
    DrawTextEx(game_manager->font, TextFormat("%i FPS", GetFPS()), (Vec2) {10.0f, 40.0f}, FONT_SIZE, FONT_SPACING, TEXT_COLOR);
    const char* coordinates_text = TextFormat("x: %.2f, y: %.2f, z: %.2f", 
        game_manager->player.position.x, 
        game_manager->player.position.y, 
        game_manager->player.position.z
    );
    DrawTextEx(game_manager->font, coordinates_text, (Vec2) {10.0f, (f32)GetScreenHeight() - 25.0f}, FONT_SIZE, FONT_SPACING, TEXT_COLOR);
}

static void font_init(GameManager* game_manager) {
    game_manager->font = LoadFontEx("./assets/fonts/JetBrainsMono-Regular.ttf", 64, NULL, 0);
    SetTextureFilter(game_manager->font.texture, TEXTURE_FILTER_BILINEAR);
}

static void shader_init(GameManager* game_manager) {
    game_manager->grid_shader = LoadShader("./assets/shaders/world_grid.vs", "assets/shaders/world_grid.fs");
    game_manager->grid_cam_pos = GetShaderLocation(game_manager->grid_shader, "camPos");
}

static void handle_window_resize(GameManager* game_manager) {
    if (IsWindowResized()) {
        Rectangle new_bounds = {0, 0, (f32)GetScreenWidth(), (f32)GetScreenHeight()};
        dock_node_resize_tree(game_manager->ui_root, new_bounds);
    }
}

static void handle_toggle_fullscreen(GameManager* game_manager) {
    if (input_is_pressed(&game_manager->input, ACTION_TOGGLE_FULLSCREEN)) {
        i32 display = GetCurrentMonitor();
        if (IsWindowFullscreen()) {
            ToggleFullscreen();
            SetWindowSize(game_manager->previous_screen_width, game_manager->previous_screen_height);
        } else {
            game_manager->previous_screen_width = GetScreenWidth();
            game_manager->previous_screen_height = GetScreenHeight();
            i32 monitorWidth = GetMonitorWidth(display);
            i32 monitorHeight = GetMonitorHeight(display);
            SetWindowSize(monitorWidth, monitorHeight);
            ToggleFullscreen();
        }
    }
}

static void window_panels_init(GameManager* game_manager) {
    Panel* game_panel = game_view_create(
        &game_manager->player,
        game_manager->grid_shader,
        game_manager->grid_cam_pos
    );
    Panel* editor_panel = editor_view_create(
        &game_manager->player,
        game_manager->grid_shader,
        game_manager->grid_cam_pos
    );
    DockNode* left_leaf = dock_node_create_leaf(game_panel, BASE_SCREEN_WIDTH / 2, BASE_SCREEN_HEIGHT);
    DockNode* right_leaf = dock_node_create_leaf(editor_panel, BASE_SCREEN_WIDTH / 2, BASE_SCREEN_HEIGHT);
    game_manager->ui_root = dock_node_create_split(DOCK_SPLIT_HORIZONTAL, 0.5f, left_leaf, right_leaf);
    Rectangle initial_bounds = {0, 0, BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT};
    dock_node_resize_tree(game_manager->ui_root, initial_bounds);
}

void game_manager_init(GameManager* game_manager) {
    game_manager->game_state = STATE_RUNNING;
    log_msg("initialising");
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    window_init();
    shader_init(game_manager);
    font_init(game_manager);
    input_init(&game_manager->input);
    player_init(&game_manager->player);
    window_panels_init(game_manager);
}

void game_manager_update(GameManager* game_manager) {
    i32 desired_mouse_cursor = MOUSE_CURSOR_DEFAULT;
    handle_window_resize(game_manager);
    input_update(&game_manager->input);
    handle_toggle_fullscreen(game_manager);
    dock_node_update_tree(game_manager->ui_root, &game_manager->input, game_manager->delta_time, &desired_mouse_cursor);
    if (!IsCursorHidden()) {
        SetMouseCursor(desired_mouse_cursor);
    }
}

void game_manager_render(GameManager* game_manager) {
    dock_node_render_tree(game_manager->ui_root);
    BeginDrawing();
    ClearBackground(BG_COLOR); 
    draw_dock_tree(game_manager->ui_root, game_manager->font);
    render_ui(game_manager);
    EndDrawing();
}

void game_manager_cleanup(GameManager* game_manager) {
    UnloadShader(game_manager->grid_shader);
    UnloadFont(game_manager->font);
    CloseWindow();
    log_msg("closing");
}