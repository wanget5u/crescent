#pragma once

#include "../core/core.h"
#include "../user interface/panel.h"
#include "../player.h"

Panel* editor_view_create(Player* player_ref, Shader grid_shader, i32 cam_pos_loc, Font font);
