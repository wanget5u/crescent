#include "game_view.h"

void game_view_init(GameView* game_view) {
    camera_init(&game_view->camera);
    game_view->view = LoadRenderTexture(GetScreenWidth() / 2.0f, GetScreenHeight());
    SetTextureFilter(game_view->view.texture, BASE_TEXTURE_FILTER);
}

void game_view_handle_input(GameView* game_view, Player* player) {
    Vec3 input_direction = {0.0f, 0.0f, 0.0f};
    Vec3 forward = Vector3Subtract(game_view->camera.rl_camera.target, game_view->camera.rl_camera.position);
    forward.y = 0;
    forward = Vector3Normalize(forward);
    Vec3 right = Vector3CrossProduct(forward, game_view->camera.rl_camera.up);
    right = Vector3Normalize(right);
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

void game_view_resize(GameView* game_view, i32 new_width, i32 new_height) {
    if (game_view->view.id > 0) {
        UnloadRenderTexture(game_view->view);
    }
    game_view->view = LoadRenderTexture(new_width, new_height);
    SetTextureFilter(game_view->view.texture, BASE_TEXTURE_FILTER);
}

void game_view_cleanup(GameView* game_view) {
    UnloadRenderTexture(game_view->view);
}