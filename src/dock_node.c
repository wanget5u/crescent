#include <stdlib.h>
#include "dock_node.h"

DockNode* dock_node_create_split(DockNodeType type, f32 ratio, DockNode* a, DockNode* b) {
    DockNode* node = (DockNode*)malloc(sizeof(DockNode));
    node->type = type;
    node->split_ratio = ratio;
    node->child_a = a;
    node->child_b = b;
    node->tab_count += 1;
    node->active_tab = 0;
    node->is_dragging = false;
    return node;
}

DockNode* dock_node_create_leaf(i32 width, i32 height) {
    DockNode* node = (DockNode*)malloc(sizeof(DockNode));
    node->type = DOCK_LEAF;
    node->tab_count = 0;
    node->active_tab = 0;
    node->is_focused = false;
    width = (width <= 0) ? 1 : width;
    height = (height <= 0) ? 1 : height;
    node->render_target = LoadRenderTexture(width, height);
    SetTextureFilter(node->render_target.texture, BASE_TEXTURE_FILTER);
    return node;
}

DockNode* dock_node_prune_empty(DockNode* node) {
    if (!node) {
        return NULL;
    }
    if (node->type == DOCK_LEAF) {
        if (node->tab_count == 0) {
            if (node->render_target.id > 0) {
                UnloadRenderTexture(node->render_target);
            }
            free(node);
            return NULL;
        }
        return node;
    } else {
        node->child_a = dock_node_prune_empty(node->child_a);
        node->child_b = dock_node_prune_empty(node->child_b);
        if (node->child_a == NULL && node->child_b != NULL) {
            DockNode* survivor = node->child_b;
            free(node);
            return survivor;
        } else if (node->child_b == NULL && node->child_a != NULL) {
            DockNode* survivor = node->child_a;
            free(node);
            return survivor;
        } else if (node->child_a == NULL && node->child_b == NULL) {
            free(node);
            return NULL;
        }
        return node;
    }
}

DockNode* dock_node_get_first_leaf(DockNode* node) {
    if (!node) {
        return NULL;
    }
    if (node->type == DOCK_LEAF) {
        return node;
    }
    DockNode* left = dock_node_get_first_leaf(node->child_a);
    if (left) {
        return left;
    }
    return dock_node_get_first_leaf(node->child_b);
}

void dock_node_add_tab(DockNode* node, Panel* panel) {
    if (node->type == DOCK_LEAF && node->tab_count < MAX_TABS) {
        node->tabs[node->tab_count] = panel;
        node->tab_count++;
    }
}

Panel* dock_node_remove_tab(DockNode* node, i32 tab_index) {
    if (node->type != DOCK_LEAF || tab_index < 0 || tab_index >= node->tab_count) {
        return NULL;
    }
    Panel* extracted_panel = node->tabs[tab_index];
    for (i32 x = tab_index; x < node->tab_count - 1; x++) {
        node->tabs[x] = node->tabs[x + 1];
    }
    node->tab_count--;
    if (node->active_tab >= node->tab_count && node->tab_count > 0) {
        node->active_tab = node->tab_count - 1;
    }
    return extracted_panel;
}

void dock_node_resize_tree(DockNode* node, Rectangle new_bounds) {
    if (!node) {
        return;
    }
    node->bounds = new_bounds;
    if (node->type == DOCK_LEAF) {
        i32 width = (node->bounds.width <= 0) ? 1 : (i32)node->bounds.width;
        i32 height = (node->bounds.height <= 0) ? 1 : (i32)node->bounds.height;
        if (node->render_target.texture.width != width || node->render_target.texture.height != height) {
            if (node->render_target.id > 0) {
                UnloadRenderTexture(node->render_target);
            }
            node->render_target = LoadRenderTexture(width, height);
            SetTextureFilter(node->render_target.texture, BASE_TEXTURE_FILTER);
        }
    } else if (node->type == DOCK_SPLIT_HORIZONTAL) {
        f32 left_width = node->bounds.width * node->split_ratio;
        f32 right_width = node->bounds.width - left_width;
        Rectangle bounds_a = {node->bounds.x, node->bounds.y, left_width, node->bounds.height};
        Rectangle bounds_b = {node->bounds.x + left_width, node->bounds.y, right_width, node->bounds.height};
        dock_node_resize_tree(node->child_a, bounds_a);
        dock_node_resize_tree(node->child_b, bounds_b);
    } else if (node->type == DOCK_SPLIT_VERTICAL) {
        f32 top_height = node->bounds.height * node->split_ratio;
        f32 bottom_height = node->bounds.height - top_height;
        Rectangle bounds_a = {node->bounds.x, node->bounds.y, node->bounds.width, top_height};
        Rectangle bounds_b = {node->bounds.x, node->bounds.y + top_height, node->bounds.width, bottom_height};
        dock_node_resize_tree(node->child_a, bounds_a);
        dock_node_resize_tree(node->child_b, bounds_b);
    }
}

void dock_node_update_tree(DockNode* node, InputManager* input, f32 delta_time, i32* current_cursor, Font font, DockNode** focused_leaf, Panel** out_dragged_tab) {
    if (!node) {
        return;
    }
    if (node->type == DOCK_LEAF) {
        Vec2 mouse_pos = GetMousePosition();
        f32 tab_height = 35.0f;
        f32 current_x = node->bounds.x;
        for (i32 x = 0; x < node->tab_count; x++) {
            f32 current_tab_width = 100.0f;
            if (node->tabs[x]->title) {
                Vec2 text_size = MeasureTextEx(font, node->tabs[x]->title, FONT_SIZE, FONT_SPACING);
                current_tab_width = text_size.x + 20.0f;
            }
            Rectangle tab_hitbox = {current_x, node->bounds.y, current_tab_width, tab_height};
            if (CheckCollisionPointRec(mouse_pos, tab_hitbox)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    node->active_tab = x;
                    *focused_leaf = node;
                }
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && Vector2Length(GetMouseDelta()) > 0.1f) {
                    *out_dragged_tab = dock_node_remove_tab(node, x);
                    break;
                }
            }
            current_x += current_tab_width + 2.0f;
        }
        Rectangle content_bounds = {node->bounds.x, node->bounds.y + tab_height, node->bounds.width, node->bounds.height - tab_height};
        if (input_is_pressed(input, ACTION_FOCUS)) {
            if (!IsCursorHidden() && CheckCollisionPointRec(mouse_pos, content_bounds)) {
                node->is_focused = true;
                node->saved_mouse_pos = mouse_pos;
                DisableCursor();
                *focused_leaf = node;
            }
        }
        if (input_is_released(input, ACTION_FOCUS) && node->is_focused) {
            node->is_focused = false;
            EnableCursor();
            SetMousePosition((i32)node->saved_mouse_pos.x, (i32)node->saved_mouse_pos.y);
        }
        if (node->tab_count > 0 && node->tabs[node->active_tab]->update) {
            node->tabs[node->active_tab]->update(node->tabs[node->active_tab], input, node->is_focused, delta_time);
        }
    } else {
        Rectangle splitter_rectangle;
        if (node->type == DOCK_SPLIT_HORIZONTAL) {
            f32 split_x = node->bounds.x + (node->bounds.width * node->split_ratio);
            splitter_rectangle = (Rectangle){split_x - (DOCK_SPLITTER_THICKNESS / 2.0f), node->bounds.y, DOCK_SPLITTER_THICKNESS, node->bounds.height};
        } else {
            f32 split_y = node->bounds.y + (node->bounds.height * node->split_ratio);
            splitter_rectangle = (Rectangle){node->bounds.x, split_y - (DOCK_SPLITTER_THICKNESS / 2.0f), node->bounds.width, DOCK_SPLITTER_THICKNESS};
        }
        Vec2 mouse_pos = GetMousePosition();
        bool is_hovering = CheckCollisionPointRec(mouse_pos, splitter_rectangle);
        if (is_hovering && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            node->is_dragging = true;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            node->is_dragging = false;
        }
        if (is_hovering || node->is_dragging) {
            if (node->type == DOCK_SPLIT_HORIZONTAL) {
                *current_cursor = MOUSE_CURSOR_RESIZE_EW;
            } else {
                *current_cursor = MOUSE_CURSOR_RESIZE_NS;
            }
        }
        if (node->is_dragging) {
            f32 new_ratio;
            if (node->type == DOCK_SPLIT_HORIZONTAL) {
                new_ratio = (mouse_pos.x - node->bounds.x) / node->bounds.width;
            } else {
                new_ratio = (mouse_pos.y - node->bounds.y) / node->bounds.height;
            }
            node->split_ratio = Clamp(new_ratio, 0.1f, 0.9f);
            dock_node_resize_tree(node, node->bounds);
        }
        dock_node_update_tree(node->child_a, input, delta_time, current_cursor, font, focused_leaf, out_dragged_tab);
        dock_node_update_tree(node->child_b, input, delta_time, current_cursor, font, focused_leaf, out_dragged_tab);
    }
}

void dock_node_render_tree(DockNode* node) {
    if (!node) {
        return;
    }
    if (node->type == DOCK_LEAF) {
        BeginTextureMode(node->render_target);
        ClearBackground(BG_COLOR);
        if (node->tab_count > 0 && node->tabs[node->active_tab]->render) {
            node->tabs[node->active_tab]->render(node->tabs[node->active_tab]);
        }
        EndTextureMode();
    } else {
        dock_node_render_tree(node->child_a);
        dock_node_render_tree(node->child_b);
    }
}