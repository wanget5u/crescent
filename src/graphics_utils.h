#pragma once

#include "core.h"
#include <rlgl.h>

static inline Color ColorScale(Color color, f32 scale) {
    return (Color) {
      (unsigned char)(color.r * scale),
      (unsigned char)(color.g * scale),
      (unsigned char)(color.b * scale),
      color.a
    };
}

void render_world_grid(Shader grid_shader, i32 cam_pos_loc, Vec3 camera_pos, f32 angle, Vec3 axis);
