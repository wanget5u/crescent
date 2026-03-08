#pragma once
#include "core.h"

typedef struct {
    Vec3 position;
    f32 speed;
    Vec3 velocity;
    Vec3 input_direction;
    Color color;
} Player;

void player_init(Player* player);
void player_update(Player* player, f32 delta_time);