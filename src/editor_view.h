#pragma once

#include "core.h"
#include "input_manager.h"
#include "game_camera.h"

typedef struct {
    GameCamera camera;
    i32 camera_speed;
    RenderTexture2D view;
    Rectangle bounds;
    bool is_focused;
    Vec2 saved_mouse_pos;
} EditorView;

void editor_view_init(EditorView* editor_view);
void editor_view_update(EditorView* game_view, InputManager* input_manager, f32 delta_time);
void editor_view_handle_input(EditorView* editor_view, f32 delta_time, InputManager* input_manager);
void editor_view_begin_render(EditorView* editor_view);
void editor_view_end_render(EditorView* editor_view);
void editor_view_resize(EditorView* editor_view, i32 x, i32 y, i32 width, i32 height);
void editor_view_cleanup(EditorView* editor_view);