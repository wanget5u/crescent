#pragma once

#include "core.h"
#include "dock_node.h"
#include "input_manager.h"
#include "player.h"

typedef struct {
    DockNode* root;
    Font font;
    Player* player_ref;
} UIManager;

void ui_manager_init(UIManager* user_interface, Player* player, Shader grid_shader, i32 grid_cam_pos);
void ui_manager_update(UIManager* user_interface, InputManager* input, f32 delta_time);
void ui_manager_render(UIManager* user_interface);
void ui_manager_resize(UIManager* user_interface, f32 width, f32 height);
void ui_manager_cleanup(UIManager* user_interface);