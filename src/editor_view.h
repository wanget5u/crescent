#pragma once

#include "core.h"
#include "game_camera.h"

typedef struct {
    GameCamera camera;
    i32 camera_speed;
    RenderTexture2D view;
} EditorView;

void editor_view_init(EditorView* editor_view);
void editor_view_handle_input(EditorView* editor_view, f32 delta_time);
void editor_view_begin_render(EditorView* editor_view);
void editor_view_end_render(EditorView* editor_view);
void editor_view_resize(EditorView* editor_view, i32 new_width, i32 new_height);
void editor_view_cleanup(EditorView* editor_view);