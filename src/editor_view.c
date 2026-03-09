#include "editor_view.h"

void editor_view_init(EditorView* editor_view) {
    camera_init_pos(&editor_view -> camera, (Vec3){5.0f, 10.0f, 5.0f});
    editor_view -> view = LoadRenderTexture(640, 720);
}

void editor_view_handle_input(EditorView* editor_view, f32 delta_time) {
    Vec3 input_direction = (Vec3) {0.0f, 0.0f, 0.0f};
    Vec3 forward = Vector3Subtract(editor_view -> camera.rl_camera.target, editor_view -> camera.rl_camera.position);
    forward = Vector3Normalize(forward);
    Vec3 right = Vector3CrossProduct(forward, editor_view -> camera.rl_camera.up);
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
    input_direction = Vector3Normalize(input_direction);
    f32 fly_speed = EDITOR_FLY_SPEED * delta_time;
    editor_view -> camera.rl_camera.position.x += input_direction.x * fly_speed;
    editor_view -> camera.rl_camera.position.y += input_direction.y * fly_speed;
    editor_view -> camera.rl_camera.position.z += input_direction.z * fly_speed;
}

void editor_view_begin_render(EditorView* editor_view) {
    BeginTextureMode(editor_view -> view);
    ClearBackground(BG_COLOR);
    BeginMode3D(editor_view -> camera.rl_camera);
}

void editor_view_end_render(EditorView* editor_view) {
    (void) editor_view;
    EndMode3D();
    EndTextureMode();
}

void editor_view_cleanup(EditorView* editor_view) {
    UnloadRenderTexture(editor_view -> view);
}