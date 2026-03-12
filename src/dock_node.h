#pragma once

#define MAX_TABS 8

#include "panel.h"

typedef enum {
    DOCK_SPLIT_HORIZONTAL,
    DOCK_SPLIT_VERTICAL,
    DOCK_LEAF
} DockNodeType;

typedef struct DockNode DockNode;
struct DockNode {
    DockNodeType type;
    Rectangle bounds;
    // SPLITTER
    f32 split_ratio;       // e.g., 0.5f is a 50/50 split
    DockNode* child_a;     // Left or Top
    DockNode* child_b;     // Right or Bottom
    bool is_dragging;
    // LEAF
    RenderTexture2D render_target; 
    Panel* tabs[MAX_TABS];
    i32 tab_count;
    i32 active_tab;
    bool is_focused;
    Vec2 saved_mouse_pos;
};

DockNode* dock_node_create_split(DockNodeType type, f32 ratio, DockNode* a, DockNode* b);
DockNode* dock_node_create_leaf(i32 width, i32 height);
DockNode* dock_node_prune_empty(DockNode* node);
DockNode* dock_node_get_first_leaf(DockNode* node);
void dock_node_add_tab(DockNode* node, Panel* panel);
Panel* dock_node_remove_tab(DockNode* node, i32 tab_index);
void dock_node_resize_tree(DockNode* node, Rectangle new_bounds);
void dock_node_update_tree(DockNode* node, InputManager* input, f32 delta_time, i32* current_cursor, Font font, DockNode** focused_leaf, Panel** out_dragged_tab);
void dock_node_render_tree(DockNode* node);