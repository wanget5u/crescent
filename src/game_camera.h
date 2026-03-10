#pragma once

#include "core.h"
#include "input_manager.h"

typedef struct {
    Camera3D rl_camera;
    f32 pitch;
    f32 yaw;
} GameCamera;

void camera_init(GameCamera* camera);
void camera_init_pos(GameCamera* camera, Vec3 initial_pos);
void camera_update(GameCamera* camera, Vec3* target_pos, bool is_focused, f32 delta_time);
void handle_cursor_visibility(InputManager* input_manager);
void handle_camera_rotation(GameCamera* camera);
void handle_camera_clamp(GameCamera* camera);
void apply_camera_pos(GameCamera* camera, Vec3* target_pos);