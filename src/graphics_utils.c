#include <stdlib.h>
#include "graphics_utils.h"

void render_environment_grid(Shader grid_shader, i32 cam_pos_loc, Vec3 camera_pos) {
    SetShaderValue(grid_shader, cam_pos_loc, &camera_pos, SHADER_UNIFORM_VEC3);
    BeginBlendMode(BLEND_ALPHA);
    BeginShaderMode(grid_shader);
    rlDisableBackfaceCulling();
    rlPushMatrix();
    rlTranslatef(camera_pos.x, 0.0f, camera_pos.z);
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