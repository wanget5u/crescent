#include "game_camera.h"

void camera_init(GameCamera* camera) {
    camera->rl_camera.position = (Vec3) {0.0f, 0.0f, 0.0f};
    camera->rl_camera.target = (Vec3){ 0.0f, 0.0f, 0.0f };
    camera->rl_camera.up = (Vec3) {0.0f, 1.0f, 0.0f};
    camera->rl_camera.fovy = CAMERA_FOV;
    camera->rl_camera.projection = CAMERA_PERSPECTIVE;
    camera->pitch = 0.0f;
    camera->yaw = 0.0f;
}

void camera_init_pos(GameCamera* camera, Vec3 initial_pos) {
    camera_init(camera); 
    camera->rl_camera.position = initial_pos;
    camera->rl_camera.target = (Vec3) {initial_pos.x, initial_pos.y, initial_pos.z - 1.0f};
}

void camera_update(GameCamera* camera, Vec3* target_pos, bool is_focused, f32 delta_time) {
    (void) delta_time;
    if (is_focused) {
        handle_camera_rotation(camera);   
    }
    handle_camera_clamp(camera);
    apply_camera_pos(camera, target_pos);
}

void handle_camera_rotation(GameCamera* camera) {
    Vec2 mouse_delta = GetMouseDelta();
    camera->yaw -= mouse_delta.x * MOUSE_SENS;
    camera->pitch -= mouse_delta.y * MOUSE_SENS;
}

void handle_camera_zoom(GameCamera* camera) {
    camera->rl_camera.fovy = GetMouseWheelMove();
}

void handle_camera_clamp(GameCamera* camera) {
    f32 max_pitch = (PI / 2.0f) - 0.001f;
    if (camera->pitch > max_pitch) {
        camera->pitch = max_pitch;
    }
    if (camera->pitch < -max_pitch) {
        camera->pitch = -max_pitch;
    }
}

void apply_camera_pos(GameCamera* camera, Vec3* target_pos) {
    camera->rl_camera.position = *target_pos;
    f32 x = cosf(camera->pitch) * sinf(camera->yaw);
    f32 y = sinf(camera->pitch);
    f32 z = cosf(camera->pitch) * cosf(camera->yaw);
    Vec3 view_dir = {x, y, z};
    camera->rl_camera.target = Vector3Add(camera->rl_camera.position, view_dir);
}