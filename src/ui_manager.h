#pragma once

#include "core.h"
#include "dock_node.h"
#include "input_manager.h"
#include "player.h"

typedef enum {
    DROP_NONE,      
    DROP_CENTER,    // Adds to the target leaf's tab array
    DROP_LEFT,      // Splits the leaf horizontally, places tab on the left
    DROP_RIGHT,     // Splits the leaf horizontally, places tab on the right
    DROP_TOP,       // Splits the leaf vertically  , places tab on top
    DROP_BOTTOM     // Splits the leaf vertically  , places tab on bottom
} DropZone;

typedef struct {
    DockNode* root;
    Font font;
    Player* player_ref;
    DockNode* focused_leaf;

    Panel* dragging_tab;
    DockNode* hover_target;
    DropZone current_drop_zone;
    Rectangle drop_preview_rectangle;
} UIManager;

void ui_manager_init(UIManager* user_interface, Player* player, Shader grid_shader, i32 grid_cam_pos);
void ui_manager_update(UIManager* user_interface, InputManager* input, f32 delta_time);
void ui_manager_render(UIManager* user_interface);
void ui_manager_resize(UIManager* user_interface, f32 width, f32 height);
void ui_manager_cleanup(UIManager* user_interface);