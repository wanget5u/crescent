#include "game_view.h"

void game_view_init(GameView* game_view) {
    camera_init(&game_view->camera);
    game_view->view = LoadRenderTexture(GetScreenWidth() / 2.0f, GetScreenHeight());
    SetTextureFilter(game_view->view.texture, BASE_TEXTURE_FILTER);
    game_view->bounds = (Rectangle){0, 0, (f32)GetScreenWidth() / 2.0f, (f32)GetScreenHeight()};
    game_view->is_focused = false;
}

void game_view_update(GameView* game_view, Player* player, InputManager* input_manager) {
    Vec2 mouse_pos = GetMousePosition();
    if (input_is_pressed(input_manager, ACTION_FOCUS)) {
        if (!IsCursorHidden() && CheckCollisionPointRec(mouse_pos, game_view->bounds)) {
            game_view->is_focused = true;
            game_view->saved_mouse_pos = mouse_pos;
            DisableCursor();
        }
    }
    if (input_is_released(input_manager, ACTION_FOCUS)) {
        if (game_view->is_focused) { 
            game_view->is_focused = false;
            EnableCursor();
            SetMousePosition((i32)game_view->saved_mouse_pos.x, (i32)game_view->saved_mouse_pos.y);
        }
    }
    if (game_view->is_focused) {
        game_view_handle_input(game_view, player, input_manager);
        handle_camera_rotation(&game_view->camera);
    } else {
        player->input_direction = (Vec3){0.0f, 0.0f, 0.0f}; 
    }
}

void game_view_handle_input(GameView* game_view, Player* player, InputManager* input_manager) {
    Vec3 input_direction = {0.0f, 0.0f, 0.0f};
    Vec3 forward = Vector3Subtract(game_view->camera.rl_camera.target, game_view->camera.rl_camera.position);
    forward.y = 0;
    forward = Vector3Normalize(forward);
    Vec3 right = Vector3CrossProduct(forward, game_view->camera.rl_camera.up);
    right = Vector3Normalize(right);
    if (input_is_down(input_manager, ACTION_MOVE_FORWARD)) {
        input_direction = Vector3Add(input_direction, forward);
    }
    if (input_is_down(input_manager, ACTION_MOVE_BACKWARD)) {
        input_direction = Vector3Subtract(input_direction, forward);
    }
    if (input_is_down(input_manager, ACTION_MOVE_RIGHT)) {
        input_direction = Vector3Add(input_direction, right);
    }
    if (input_is_down(input_manager, ACTION_MOVE_LEFT)) {
        input_direction = Vector3Subtract(input_direction, right);
    }
    player->input_direction = Vector3Normalize(input_direction);
}

void game_view_begin_render(GameView* game_view) {
    BeginTextureMode(game_view->view);
    ClearBackground(BG_COLOR);
    BeginMode3D(game_view->camera.rl_camera);
}

void game_view_end_render(GameView* game_view) {
    (void) game_view;
    EndMode3D();
    EndTextureMode();
}

void game_view_resize(GameView* game_view, i32 x, i32 y, i32 width, i32 height) {
    if (game_view->view.id > 0) {
        UnloadRenderTexture(game_view->view);
    }
    game_view->view = LoadRenderTexture(width, height);
    SetTextureFilter(game_view->view.texture, BASE_TEXTURE_FILTER);
    game_view->bounds = (Rectangle) {(f32)x, (f32)y, (f32)width, (f32)height};
}

void game_view_cleanup(GameView* game_view) {
    UnloadRenderTexture(game_view->view);
}