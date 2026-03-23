#pragma once

#include "core/core.h"

typedef struct {
    Vec3 position;
    f32 speed;
    Vec3 velocity;
    Vec3 input_direction;
    f32 rotation;
    Color color;
} Player;

void player_init(Player* player);
void player_update(Player* player, f32* camera_yaw, f32 delta_time);
void player_render(Player* player);
