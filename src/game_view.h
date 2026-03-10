#pragma once

#include "core.h"
#include "input_manager.h"
#include "game_camera.h"
#include "player.h"

typedef struct {
    GameCamera camera;
    RenderTexture2D view;
    Rectangle bounds;
    bool is_focused;
    Vec2 saved_mouse_pos;
} GameView;

void game_view_init(GameView* game_view);
void game_view_update(GameView* game_view, Player* player, InputManager* input);
void game_view_handle_input(GameView* game_view, Player* player, InputManager* input_manager);
void game_view_begin_render(GameView* game_view);
void game_view_end_render(GameView* game_view);
void game_view_resize(GameView* editor_view, i32 x, i32 y, i32 width, i32 height);
void game_view_cleanup(GameView* game_view);