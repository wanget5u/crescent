#pragma once

#include "core.h"
#include "panel.h"
#include "player.h"

Panel* editor_view_create(Player* player_ref, Shader grid_shader, i32 cam_pos_loc, Font font);