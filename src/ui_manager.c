#include "ui_manager.h"
#include "game_view.h"
#include "editor_view.h"

static void draw_dock_tree(DockNode* node, Font font) {
    if (!node) {
        return;
    }
    if (node->type == DOCK_LEAF) {
        Rectangle source_rect = {0.0f, 0.0f, (f32)node->render_target.texture.width, (f32)-node->render_target.texture.height};
        Vec2 position = {node->bounds.x, node->bounds.y};
        DrawTextureRec(node->render_target.texture, source_rect, position, WHITE);
        f32 tab_height = 35.0f;
        f32 current_x = node->bounds.x;
        DrawRectangle(node->bounds.x, node->bounds.y, node->bounds.width, tab_height, ColorAlpha(BLACK, 0.8f));
        for (i32 x = 0; x < node->tab_count; x++) {
            f32 current_tab_width = 100.0f;
            if (node->tabs[x]->title) {
                Vec2 text_size = MeasureTextEx(font, node->tabs[x]->title, FONT_SIZE, FONT_SPACING);
                current_tab_width = text_size.x + 20.0f;
            }
            Rectangle tab_bg = {current_x, node->bounds.y, current_tab_width, tab_height};
            Color tab_color = (x == node->active_tab) ? GRAY : DARKGRAY;
            DrawRectangleRec(tab_bg, tab_color);
            if (node->tabs[x]->title) {
                Vec2 text_pos = {current_x + 8.0f, node->bounds.y + 8.0f};
                DrawTextEx(font, node->tabs[x]->title, text_pos, FONT_SIZE, FONT_SPACING, RAYWHITE);
            }
            current_x += current_tab_width + 2.0f;
        }
        if (node->is_focused) {
            f32 inset = 1.0f; 
            Rectangle highlight_rect = {
                node->bounds.x + inset,
                node->bounds.y + tab_height + inset,
                node->bounds.width - (inset * 2.0f),
                node->bounds.height - tab_height - (inset * 2.0f)
            };
            DrawRectangleLinesEx(highlight_rect, 3.0f, WHITE);
        }
    } else {
        draw_dock_tree(node->child_a, font);
        draw_dock_tree(node->child_b, font);
        Color splitter_color = DOCK_SPLITTER_COLOR;
        f32 visual_thickness = 2.0f;
        if (node->type == DOCK_SPLIT_HORIZONTAL) {
            f32 split_x = node->bounds.x + (node->bounds.width * node->split_ratio);
            DrawRectangle((i32)(split_x - (visual_thickness / 2.0f)), (i32)node->bounds.y, (i32)visual_thickness, (i32)node->bounds.height, splitter_color);
        } else if (node->type == DOCK_SPLIT_VERTICAL) {
            f32 split_y = node->bounds.y + (node->bounds.height * node->split_ratio);
            DrawRectangle((i32)node->bounds.x, (i32)(split_y - (visual_thickness / 2.0f)), (i32)node->bounds.width, (i32)visual_thickness, splitter_color);
        }
    }
}

static void render_global_ui(UIManager* user_interface) {
    DrawTextEx(user_interface->font, TextFormat("%i FPS", GetFPS()), (Vec2) {10.0f, 40.0f}, FONT_SIZE, FONT_SPACING, TEXT_COLOR);
    const char* coordinates_text = TextFormat("x: %.2f, y: %.2f, z: %.2f", 
        user_interface->player_ref->position.x, 
        user_interface->player_ref->position.y, 
        user_interface->player_ref->position.z
    );
    DrawTextEx(user_interface->font, coordinates_text, (Vec2) {10.0f, (f32)GetScreenHeight() - 30.0f}, FONT_SIZE, FONT_SPACING, TEXT_COLOR);
}

void ui_manager_init(UIManager* user_interface, Player* player, Shader grid_shader, int grid_cam_pos) {
    user_interface->player_ref = player;
    user_interface->font = LoadFontEx("./assets/fonts/JetBrainsMono-Regular.ttf", 64, NULL, 0);
    SetTextureFilter(user_interface->font.texture, TEXTURE_FILTER_BILINEAR);
    Panel* game_panel = game_view_create(player, grid_shader, grid_cam_pos);
    Panel* editor_panel = editor_view_create(player, grid_shader, grid_cam_pos);
    DockNode* root_leaf = dock_node_create_leaf(BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT);
    dock_node_add_tab(root_leaf, game_panel);
    dock_node_add_tab(root_leaf, editor_panel);
    user_interface->root = root_leaf;
    ui_manager_resize(user_interface, BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT);
}

void ui_manager_update(UIManager* user_interface, InputManager* input, f32 delta_time) {
    i32 desired_mouse_cursor = MOUSE_CURSOR_DEFAULT;
    dock_node_update_tree(user_interface->root, input, delta_time, &desired_mouse_cursor, user_interface->font);
    if (!IsCursorHidden()) {
        SetMouseCursor(desired_mouse_cursor);
    }
}

void ui_manager_render(UIManager* user_interface) {
    dock_node_render_tree(user_interface->root);
    BeginDrawing();
    ClearBackground(BG_COLOR); 
    draw_dock_tree(user_interface->root, user_interface->font);
    render_global_ui(user_interface);
    EndDrawing();
}

void ui_manager_resize(UIManager* user_interface, f32 width, f32 height) {
    Rectangle bounds = {0, 0, width, height};
    dock_node_resize_tree(user_interface->root, bounds);
}

void ui_manager_cleanup(UIManager* user_interface) {
    UnloadFont(user_interface->font);
}