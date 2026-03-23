#pragma once

#include "../user interface/panel.h"
#include "../player.h"

Panel* game_view_create(Player* player, Shader grid_shader, i32 cam_pos_loc, Font font);
