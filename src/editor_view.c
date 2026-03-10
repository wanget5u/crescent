#include "editor_view.h"

void editor_view_init(EditorView* editor_view) {
    camera_init_pos(&editor_view->camera, (Vec3){5.0f, 10.0f, 5.0f});
    editor_view->view = LoadRenderTexture(GetScreenWidth() / 2.0f, GetScreenHeight());
    SetTextureFilter(editor_view->view.texture, BASE_TEXTURE_FILTER);
    editor_view->bounds = (Rectangle){(f32)GetScreenWidth() / 2.0f, 0, (f32)GetScreenWidth() / 2.0f, (f32)GetScreenHeight()};
    editor_view->is_focused = false;
}

void editor_view_update(EditorView* editor_view, InputManager* input_manager, f32 delta_time) {
    Vec2 mouse_pos = GetMousePosition();
    if (input_is_pressed(input_manager, ACTION_FOCUS)) {
        if (!IsCursorHidden() && CheckCollisionPointRec(mouse_pos, editor_view->bounds)) {
            editor_view->is_focused = true;
            editor_view->saved_mouse_pos = mouse_pos;
            DisableCursor();
        }
    }
    if (input_is_released(input_manager, ACTION_FOCUS)) {
        if (editor_view->is_focused) {
            editor_view->is_focused = false;
            EnableCursor();
            SetMousePosition((i32)editor_view->saved_mouse_pos.x, (i32)editor_view->saved_mouse_pos.y);
        }
    }
    if (editor_view->is_focused) {
        editor_view_handle_input(editor_view, delta_time, input_manager);
        handle_camera_rotation(&editor_view->camera);
    }
}

void editor_view_handle_input(EditorView* editor_view, f32 delta_time, InputManager* input_manager) {
    Vec3 input_direction = (Vec3) {0.0f, 0.0f, 0.0f};
    Vec3 forward = Vector3Subtract(editor_view->camera.rl_camera.target, editor_view->camera.rl_camera.position);
    forward = Vector3Normalize(forward);
    Vec3 right = Vector3CrossProduct(forward, editor_view->camera.rl_camera.up);
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
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        editor_view->camera_speed = EDITOR_ACCELERATED_FLY_SPEED;
    } else {
        editor_view->camera_speed = BASE_EDITOR_FLY_SPEED;
    }
    input_direction = Vector3Normalize(input_direction);
    f32 fly_speed = editor_view->camera_speed * delta_time;
    editor_view->camera.rl_camera.position.x += input_direction.x * fly_speed;
    editor_view->camera.rl_camera.position.y += input_direction.y * fly_speed;
    editor_view->camera.rl_camera.position.z += input_direction.z * fly_speed;
}

void editor_view_begin_render(EditorView* editor_view) {
    BeginTextureMode(editor_view->view);
    ClearBackground(BG_COLOR);
    BeginMode3D(editor_view->camera.rl_camera);
}

void editor_view_end_render(EditorView* editor_view) {
    (void) editor_view;
    EndMode3D();
    EndTextureMode();
}

void editor_view_resize(EditorView* editor_view, i32 x, i32 y, i32 width, i32 height) {
    if (editor_view->view.id > 0) {
        UnloadRenderTexture(editor_view->view);
    }
    editor_view->view = LoadRenderTexture(width, height);
    SetTextureFilter(editor_view->view.texture, BASE_TEXTURE_FILTER);
    editor_view->bounds = (Rectangle) {(f32)x, (f32)y, (f32)width, (f32)height};
}

void editor_view_cleanup(EditorView* editor_view) {
    UnloadRenderTexture(editor_view->view);
}