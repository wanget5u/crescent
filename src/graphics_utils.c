#include "graphics_utils.h"

void render_world_grid(Shader grid_shader, i32 cam_pos_loc, Vec3 camera_pos, f32 angle, Vec3 axis) {
    SetShaderValue(grid_shader, cam_pos_loc, &camera_pos, SHADER_UNIFORM_VEC3);
    BeginBlendMode(BLEND_ALPHA);
    BeginShaderMode(grid_shader);
    rlDisableBackfaceCulling();
    rlPushMatrix();
    Vec3 grid_pos = camera_pos;
    if (axis.z > 0.5f) {
        grid_pos.x = 0.0f;
    } else if (axis.x > 0.5f && (angle > 45.0f || angle < -45.0f)) {
        grid_pos.z = 0.0f;
    } else {
        grid_pos.y = 0.0f;
    }
    rlTranslatef(grid_pos.x, grid_pos.y, grid_pos.z);
    rlRotatef(angle, axis.x, axis.y, axis.z);
    rlBegin(RL_QUADS);
        rlVertex3f(-GRID_SCALE, 0.0f,-GRID_SCALE);
        rlVertex3f(-GRID_SCALE, 0.0f, GRID_SCALE);
        rlVertex3f( GRID_SCALE, 0.0f, GRID_SCALE);
        rlVertex3f( GRID_SCALE, 0.0F,-GRID_SCALE);
    rlEnd();
    rlPopMatrix();
    EndShaderMode();
    EndBlendMode();
}
