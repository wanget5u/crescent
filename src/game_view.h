#pragma once

#include "core.h"
#include "game_camera.h"
#include "player.h"

typedef struct {
    GameCamera camera;
    RenderTexture2D view;
} GameView;

void game_view_init(GameView* game_view);
void game_view_handle_input(GameView* game_view, Player* player);
void game_view_begin_render(GameView* game_view);
void game_view_end_render(GameView* game_view);
void game_view_resize(GameView* editor_view, i32 new_width, i32 new_height);
void game_view_cleanup(GameView* game_view);