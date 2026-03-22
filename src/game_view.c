#include <stdlib.h>
#include "core.h"
#include "graphics_utils.h"
#include "game_view.h"
#include "game_camera.h"
#include "raylib.h"

typedef struct {
    GameCamera camera;
    Player* player_ref;
    Shader grid_shader;
    i32 grid_cam_pos_loc;
} GameViewData;

static void handle_game_input(GameViewData* view, InputManager* input) {
    Vec3 input_direction = {0.0f, 0.0f, 0.0f};
    Vec3 forward = Vector3Subtract(view->camera.rl_camera.target, view->camera.rl_camera.position);
    forward.y = 0.0f;
    forward = Vector3Normalize(forward);
    Vec3 right = Vector3CrossProduct(forward, view->camera.rl_camera.up);
    right = Vector3Normalize(right);
    if (input_is_down(input, ACTION_MOVE_FORWARD))  input_direction = Vector3Add(input_direction, forward);
    if (input_is_down(input, ACTION_MOVE_BACKWARD)) input_direction = Vector3Subtract(input_direction, forward);
    if (input_is_down(input, ACTION_MOVE_RIGHT))    input_direction = Vector3Add(input_direction, right);
    if (input_is_down(input, ACTION_MOVE_LEFT))     input_direction = Vector3Subtract(input_direction, right);
    view->player_ref->input_direction = Vector3Normalize(input_direction);
}

static void game_view_update(Panel* panel, InputManager* input, bool is_focused, f32 delta_time, Font font) {
    (void) font;
    (void) delta_time;
    GameViewData* view = (GameViewData*)panel->data;
    if (is_focused) {
        handle_game_input(view, input);
    } else {
        view->player_ref->input_direction = (Vec3){0.0f, 0.0f, 0.0f};
    }
    player_update(view->player_ref, &view->camera.yaw, delta_time);
    camera_update(&view->camera, &view->player_ref->position, is_focused, delta_time);
}

static void game_view_render(Panel* panel) {
    GameViewData* view = (GameViewData*)panel->data;
    BeginMode3D(view->camera.rl_camera);
    render_environment_grid(view->grid_shader, view->grid_cam_pos_loc, view->camera.rl_camera.position);
    player_render(view->player_ref);
    EndMode3D();
}

static void game_view_cleanup(Panel* panel) {
    free(panel->data);
    free(panel);
}

static void game_view_render_overlay(Panel* panel, Font font) {
    GameViewData* view = (GameViewData*)panel->data;
    BeginScissorMode((i32)panel->bounds.x, (i32)panel->bounds.y, (i32)panel->bounds.width, (i32)panel->bounds.height);
    f32 pos_x = panel->bounds.x + 10.0f;
    f32 pos_y = panel->bounds.y + 10.0f;
    f32 font_size = 22.0f;
    f32 line_spacing = font_size + 4.0f;
    DrawTextEx(font, TextFormat("X: %.2f", view->player_ref->position.x), (Vec2){pos_x, pos_y}, font_size, FONT_SPACING, ColorAlpha(ColorScale(RED, 0.9f), 0.7f));
    pos_y += line_spacing;
    DrawTextEx(font, TextFormat("Y: %.2f", view->player_ref->position.y), (Vec2){pos_x, pos_y}, font_size, FONT_SPACING, ColorAlpha(ColorScale(GREEN, 0.9f), 0.7f));
    pos_y += line_spacing;
    DrawTextEx(font, TextFormat("Z: %.2f", view->player_ref->position.z), (Vec2){pos_x, pos_y}, font_size, FONT_SPACING, ColorAlpha(ColorScale(BLUE, 0.9f), 0.7f));
    EndScissorMode();
}

Panel* game_view_create(Player* player, Shader grid_shader, i32 cam_pos_loc, Font font) {
    (void) font;
    Panel* panel = (Panel*)malloc(sizeof(Panel));
    GameViewData* data = (GameViewData*)malloc(sizeof(GameViewData));
    camera_init(&data->camera);
    data->player_ref = player;
    data->grid_shader = grid_shader;
    data->grid_cam_pos_loc = cam_pos_loc;
    panel->title = "Game";
    panel->tab_width = MeasureTextEx(font, panel->title, FONT_SIZE, FONT_SPACING).x + 20.0f;
    panel->data = data;
    panel->update = game_view_update;
    panel->render = game_view_render;
    panel->render_overlay = game_view_render_overlay;
    panel->cleanup = game_view_cleanup;
    return panel;
}
