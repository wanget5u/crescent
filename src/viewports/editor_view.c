#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <float.h>
#include <raylib.h>
#include "../core/graphics_utils.h"
#include "../viewports/editor_view.h"
#include "../game_camera.h"
#include "../core/input_manager.h"
#include "../user interface/ui_node.h"

typedef struct {
    bool show_grid;
    bool show_3D;
} EditorOptions;

typedef struct {
    GameCamera camera;
    f32 camera_speed;
    Player* player_ref;
    Shader grid_shader;
    i32 grid_cam_pos_loc;
    Font font;
    // animation state
    bool is_animating;

    f32 anim_timer;
    f32 anim_duration;

    Vec3 start_pos;
    Vec3 target_pos;

    f32 start_yaw;
    f32 start_pitch;

    f32 target_yaw;
    f32 target_pitch;

    f32 saved_fov;

    f32 grid_angle;
    f32 start_grid_angle;
    f32 target_grid_angle;
    Vec3 grid_axis;
    i8 active_axis_id;

    UINode* header_buttons[3];
    UINode* hovered_node;
    UINode* pressed_node;

    EditorOptions editor_options;
} EditorViewData;

typedef struct {
    Vec2 pos;
    f32 depth;
    Color fillColor;
    Color outlineColor;
    const char* label;
    u8 id;
    bool is_negative;
} GizmoAxis;

typedef struct {
    f32 margin_right;
    f32 margin_top;
    f32 line_length;
    f32 tip_radius;
    f32 bg_radius;
    f32 dot_radius;
    f32 line_thickness;
} GizmoStyle;

static const GizmoStyle GIZMO_STYLE = {
    .margin_right = 70.0f,
    .margin_top = 102.5f,
    .line_length = 40.0f,
    .tip_radius = 10.0f,
    .bg_radius = 50.0f,
    .dot_radius = 1.0f,
    .line_thickness = 2.0f
};

typedef struct {
    f32 height;
    f32 font_size;
    f32 button_padding;
    f32 button_height;
    f32 button_spacing;
    f32 margin_x;
    f32 margin_y;
} EditorHeaderStyle;

static const EditorHeaderStyle HEADER_STYLE = {
    .height = 35.0f,
    .font_size = 16.0f,
    .button_padding = 20.0f,
    .button_height = 25.0f,
    .button_spacing = 5.0f,
    .margin_x = 5.0f,
    .margin_y = 5.0f
};

static void on_3d_toggle_clicked(void* data) {
    EditorViewData* view = (EditorViewData*)data;
    view->editor_options.show_3D = !view->editor_options.show_3D;
    view->is_animating = true;
    view->anim_timer = 0.0f;
    view->anim_duration = 0.3f;
    view->start_pos = view->camera.rl_camera.position;
    view->start_yaw = view->camera.yaw;
    view->start_pitch = view->camera.pitch;
    view->start_grid_angle = view->grid_angle;

    if (view->editor_options.show_3D) {
        view->active_axis_id = -1;
        view->camera.rl_camera.projection = CAMERA_PERSPECTIVE;
        view->camera.rl_camera.fovy = view->saved_fov;
        view->target_pos = (Vec3){5.0f, 10.0f, 5.0f};
        view->target_yaw = -PI * 0.75f;
        view->target_pitch = -0.5f;
        view->target_grid_angle = 0.0f;
        view->grid_axis = (Vec3){1.0f, 0.0f, 0.0f};
    } else {
        view->saved_fov = view->camera.rl_camera.fovy;
        view->target_pos = (Vec3){view->start_pos.x, 20.0f, view->start_pos.z};
        view->target_yaw = PI;
        view->target_pitch = -(PI / 2.0f) + 0.001f;
        view->target_grid_angle = 0.0f;
        view->grid_axis = (Vec3){1.0f, 0.0f, 0.0f};
    }
    while (view->target_yaw - view->start_yaw > PI) {
        view->target_yaw -= 2.0f * PI;
    }
    while (view->target_yaw - view->start_yaw < -PI) {
        view->target_yaw += 2.0f * PI;
    }
}

static void on_grid_toggle_clicked(void* data) {
    EditorViewData* view = (EditorViewData*)data;
    view->editor_options.show_grid = !view->editor_options.show_grid;
}

static void on_reset_view_clicked(void* data) {
    EditorViewData* view = (EditorViewData*)data;
    if (view->editor_options.show_3D == true) {
        view->active_axis_id = -1;
        view->is_animating = true;
        view->anim_timer = 0.0f;
        view->anim_duration = 0.3f;
        view->start_pos = view->camera.rl_camera.position;
        view->start_yaw = view->camera.yaw;
        view->start_pitch = view->camera.pitch;
        view->target_pos = (Vec3){5.0f, 10.0f, 5.0f};
        view->target_yaw = -PI * 0.75f;
        view->target_pitch = -0.5f;
    }
}

static const char* get_header_option_label(EditorViewData* view, u8 index) {
    switch (index) {
        case 0: return view->editor_options.show_3D ? "3D" : "2D";
        case 1: return view->editor_options.show_grid ? "Hide Grid" : "Show Grid";
        case 2: return "Reset View";
        default: return "";
    }
}

static void calculate_gizmo_axes(Camera3D camera, GizmoAxis* out_axes) {
    Matrix view_mat = GetCameraMatrix(camera);
    f32 length = GIZMO_STYLE.line_length;
    Vec2 axis_x = Vector2Scale((Vec2){view_mat.m0, -view_mat.m1}, length);
    Vec2 axis_y = Vector2Scale((Vec2){view_mat.m4, -view_mat.m5}, length);
    Vec2 axis_z = Vector2Scale((Vec2){view_mat.m8, -view_mat.m9}, length);
    out_axes[0] = (GizmoAxis){axis_x, view_mat.m2, ColorScale(RED, 0.8f), RED, "X", 0, false};
    out_axes[1] = (GizmoAxis){axis_y, view_mat.m6, ColorScale(GREEN, 0.8f), GREEN, "Y", 1, false};
    out_axes[2] = (GizmoAxis){axis_z, view_mat.m10, ColorScale(BLUE, 0.8f), BLUE, "Z", 2, false};
    out_axes[3] = (GizmoAxis){Vector2Negate(axis_x), -view_mat.m2, ColorScale(RED, 0.8f), RED, "-X", 3, true};
    out_axes[4] = (GizmoAxis){Vector2Negate(axis_y), -view_mat.m6, ColorScale(GREEN, 0.8F), GREEN, "-Y", 4, true};
    out_axes[5] = (GizmoAxis){Vector2Negate(axis_z), -view_mat.m10, ColorScale(BLUE, 0.8f), BLUE, "-Z", 5, true};
}

static void draw_world_axes_gizmo(Panel* panel, Camera3D camera, Font font) {
    EditorViewData* view = (EditorViewData*)panel->data;
    Vec2 center = {
        panel->bounds.x + panel->bounds.width - GIZMO_STYLE.margin_right,
        panel->bounds.y + GIZMO_STYLE.margin_top
    };
    Vec2 global_mouse = GetMousePosition();
    bool is_hovered_background = CheckCollisionPointCircle(global_mouse, center, GIZMO_STYLE.bg_radius);
    GizmoAxis axes[6];
    calculate_gizmo_axes(camera, axes);
    i8 closest_id = -1;
    if (is_hovered_background) {
        f32 min_dist = FLT_MAX;
        for (i8 i = 0; i < 6; i++) {
            Vec2 tip_global = Vector2Add(center, axes[i].pos);
            f32 dist = Vector2Distance(global_mouse, tip_global);
            if (dist < min_dist) {
                min_dist = dist;
                closest_id = axes[i].id;
            }
        }
    }
    Color bg_color = is_hovered_background ? ColorAlpha(RAYWHITE, 0.05f) : ColorAlpha(BLACK, 0.3f);
    DrawCircleV(center, GIZMO_STYLE.bg_radius, bg_color);
    for (u8 x = 0; x < 5; x++) {
        for (u8 y = 0; y < 5 - x; y++) {
            if (axes[y].depth > axes[y + 1].depth) {
                GizmoAxis temp = axes[y];
                axes[y] = axes[y + 1];
                axes[y + 1] = temp;
            }
        }
    }
    for (u8 x = 0; x < 6; x++) {
        f32 label_size = 18.0f;
        Vec2 gizmo_pos = Vector2Add(center, axes[x].pos);
        Vec2 text_size = MeasureTextEx(font, axes[x].label, label_size, 1.0f);
        Vec2 text_pos = {
            gizmo_pos.x - (text_size.x / 2.0f),
            gizmo_pos.y - (text_size.y / 2.0f)
        };
        bool is_hovered = (is_hovered_background && axes[x].id == closest_id);
        bool is_active = (!view->editor_options.show_3D && axes[x].id == view->active_axis_id);
        Color transparent_bg = axes[x].fillColor;
        transparent_bg.a = 128;
        Color fill_color = (!axes[x].is_negative || is_hovered || is_active) ? axes[x].fillColor : transparent_bg;
        Color text_color = (is_hovered || is_active) ? WHITE : DARKGRAY;
        DrawCircleV(center, GIZMO_STYLE.dot_radius, WHITE);
        DrawCircleV(gizmo_pos, GIZMO_STYLE.tip_radius, fill_color);
        if (!axes[x].is_negative) {
            DrawLineEx(center, gizmo_pos, GIZMO_STYLE.line_thickness, fill_color);
            DrawTextEx(font, axes[x].label, text_pos, label_size, 1.0f, text_color);
        } else {
            DrawCircleLinesV(gizmo_pos, GIZMO_STYLE.tip_radius, fill_color);
        }
        if (axes[x].is_negative && (is_hovered || is_active)) {
            DrawTextEx(font, axes[x].label, text_pos, label_size, 1.0f, text_color);
        }
    }
}

static void handle_world_axes_gizmo_input(Panel* panel, EditorViewData* view) {
    Vec2 global_mouse = GetMousePosition();
    if (!CheckCollisionPointRec(global_mouse, panel->bounds)) {
        return;
    }
    Vec2 global_gizmo_center = {
        panel->bounds.x + panel->bounds.width - GIZMO_STYLE.margin_right,
        panel->bounds.y + GIZMO_STYLE.margin_top
    };
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointCircle(global_mouse, global_gizmo_center, GIZMO_STYLE.bg_radius)) {
        GizmoAxis axes[6];
        calculate_gizmo_axes(view->camera.rl_camera, axes);
        f32 min_dist = FLT_MAX;
        i8 closest_id = -1;
        for (i8 i = 0; i < 6; i++) {
            Vec2 tip_global = Vector2Add(global_gizmo_center, axes[i].pos);
            f32 dist = Vector2Distance(global_mouse, tip_global);
            if (dist < min_dist) {
                min_dist = dist;
                closest_id = axes[i].id;
            }
        }
        if (closest_id != -1) {
            view->active_axis_id = closest_id;
            view->is_animating = true;
            view->anim_timer = 0.0f;
            view->anim_duration = 0.2f;
            view->start_pos = view->camera.rl_camera.position;
            view->start_yaw = view->camera.yaw;
            view->start_pitch = view->camera.pitch;
            if (view->editor_options.show_3D) {
                view->saved_fov = view->camera.rl_camera.fovy;
                view->editor_options.show_3D = false;
            }
            f32 distance = 10.0f;
            if (closest_id == 0) { // +X (Right View)
                view->target_pos = (Vec3){
                    distance,
                    view->start_pos.y,
                    view->start_pos.z
                };
                view->target_yaw = -PI / 2.0f;
                view->target_pitch = 0.0f;
                view->target_grid_angle = 90.0f;
                view->grid_axis = (Vec3){0.0f, 0.0f, 1.0f};
            } else if (closest_id == 1) { // +Y (Top View)
                view->target_pos = (Vec3){
                    view->start_pos.x,
                    distance,
                    view->start_pos.z};
                view->target_yaw = PI;
                view->target_pitch = -(PI / 2.0f) + 0.001f;
                view->target_grid_angle = 0.0f;
                view->grid_axis = (Vec3){1.0f, 0.0f, 0.0f};
            } else if (closest_id == 2) { // +Z (Front View)
                view->target_pos = (Vec3){
                    view->start_pos.x,
                    view->start_pos.y,
                    distance
                };
                view->target_yaw = PI;
                view->target_pitch = 0.0f;
                view->target_grid_angle = 90.0f;
                view->grid_axis = (Vec3){1.0f, 0.0f, 0.0f};
            } else if (closest_id == 3) { // -X (Left View)
                view->target_pos = (Vec3){
                    -distance,
                    view->start_pos.y,
                    view->start_pos.z
                };
                view->target_yaw = PI / 2.0f;
                view->target_pitch = 0.0f;
                view->target_grid_angle = -90.0f;
                view->grid_axis = (Vec3){0.0f, 0.0f, 1.0f};
            } else if (closest_id == 4) { // -Y (Bottom View)
                view->target_pos = (Vec3){
                    view->start_pos.x,
                    -distance,
                    view->start_pos.z
                };
                view->target_yaw = PI;
                view->target_pitch = (PI / 2.0f) - 0.001f;
                view->target_grid_angle = 0.0f;
                view->grid_axis = (Vec3){1.0f, 0.0f, 0.0f};
            } else if (closest_id == 5) { // -Z (Back View)
                view->target_pos = (Vec3){
                    view->start_pos.x,
                    view->start_pos.y,
                    -distance
                };
                view->target_yaw = 0.0f; view->target_pitch = 0.0f;
                view->target_grid_angle = -90.0f; view->grid_axis = (Vec3){1.0f, 0.0f, 0.0f};
            }
            while (view->target_yaw - view->start_yaw > PI) {
                view->target_yaw -= 2.0f * PI;
            }
            while (view->target_yaw - view->start_yaw < -PI) {
                view->target_yaw += 2.0f * PI;
            }
        }
    }
}

static void draw_editor_view_options(Panel* panel, Font font) {
    EditorViewData* view = (EditorViewData*)panel->data;
    Rectangle header_rectangle = {panel->bounds.x, panel->bounds.y, panel->bounds.width, HEADER_STYLE.height};
    DrawRectangleRec(header_rectangle, ColorAlpha(BLACK, 0.6f));
    DrawLineEx(
        (Vec2){panel->bounds.x, panel->bounds.y + HEADER_STYLE.height},
        (Vec2){panel->bounds.x + panel->bounds.width, panel->bounds.y + HEADER_STYLE.height},
        1.0f, ColorAlpha(WHITE, 0.1f)
    );
    for (u8 x = 0; x < 3; x++) {
        UINode* button = view->header_buttons[x];
        Color bg_color = button->is_hovered ? ColorAlpha(WHITE, 0.4f) : ColorAlpha(WHITE, 0.05f);
        DrawRectangleRounded(button->bounds, 0.1f, 4, bg_color);
        if (button->is_hovered) {
            DrawRectangleRoundedLines(button->bounds, 0.1f, 4, ColorAlpha(WHITE, 0.2f));
        }
        Vec2 text_size = MeasureTextEx(font, button->text, HEADER_STYLE.font_size, 1.0f);
        Vec2 text_pos = {
            button->bounds.x + (HEADER_STYLE.button_padding / 2.0f),
            button->bounds.y + (HEADER_STYLE.height - text_size.y) / 2.0f
        };
        DrawTextEx(font, button->text, text_pos, HEADER_STYLE.font_size, 1.0f, button->is_hovered ? WHITE : LIGHTGRAY);
    }
}

// static bool handle_editor_options_input(Panel* panel, EditorViewData* view, Font font) {
//     Rectangle header_rectangle = {panel->bounds.x, panel->bounds.y, panel->bounds.width, HEADER_STYLE.height};
//     Vec2 mouse_pos = GetMousePosition();
//     if (CheckCollisionPointRec(mouse_pos, header_rectangle) == false) {
//         return false;
//     }
//     if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
//         f32 current_x = panel->bounds.x + HEADER_STYLE.margin_x;
//         u8 option_count = 3;
//         for (u8 x = 0; x < option_count; x++) {
//             const char* label = get_header_option_label(view, x);
//             Vec2 text_size = MeasureTextEx(font, label, HEADER_STYLE.font_size, 1.0f);
//             Rectangle option_button = {current_x, panel->bounds.y + HEADER_STYLE.margin_y, text_size.x + HEADER_STYLE.button_padding, HEADER_STYLE.button_height};
//             if (CheckCollisionPointRec(mouse_pos, option_button)) {
//                 if (x == 0) {
//                     view->editor_options.show_3D = !view->editor_options.show_3D;
//                     view->is_animating = true;
//                     view->anim_timer = 0.0f;
//                     view->anim_duration = 0.3f;
//                     view->start_pos = view->camera.rl_camera.position;
//                     view->start_yaw = view->camera.yaw;
//                     view->start_pitch = view->camera.pitch;
//                     view->start_grid_angle = view->grid_angle;
//                     if (view->editor_options.show_3D) {
//                         view->active_axis_id = -1;
//                         view->camera.rl_camera.projection = CAMERA_PERSPECTIVE;
//                         view->camera.rl_camera.fovy = view->saved_fov;
//                         view->target_pos = (Vec3){5.0f, 10.0f, 5.0f};
//                         view->target_yaw = -PI * 0.75f;
//                         view->target_pitch = -0.5f;
//                         view->target_grid_angle = 0.0f;
//                         view->grid_axis = (Vec3){1.0f, 0.0f, 0.0f};
//                     } else {
//                         view->saved_fov = view->camera.rl_camera.fovy;
//                         view->target_pos = (Vec3){view->start_pos.x, 20.0f, view->start_pos.z};
//                         view->target_yaw = PI;
//                         view->target_pitch = -(PI / 2.0f) + 0.001f;
//                         view->target_grid_angle = 0.0f;
//                         view->grid_axis = (Vec3){1.0f, 0.0f, 0.0f};
//                     }
//                     while (view->target_yaw - view->start_yaw > PI) {
//                         view->target_yaw -= 2.0f * PI;
//                     }
//                     while (view->target_yaw - view->start_yaw < -PI) {
//                         view->target_yaw += 2.0f * PI;
//                     }
//                 } else if (x == 1) {
//                     view->editor_options.show_grid = !view->editor_options.show_grid;
//                 } else if (x == 2) {
//                     if (view->editor_options.show_3D == true) {
//                         view->active_axis_id = -1;
//                         view->is_animating = true;
//                         view->anim_timer = 0.0f;
//                         view->anim_duration = 0.3f;
//                         view->start_pos = view->camera.rl_camera.position;
//                         view->start_yaw = view->camera.yaw;
//                         view->start_pitch = view->camera.pitch;
//                         view->target_pos = (Vec3){5.0f, 10.0f, 5.0f};
//                         view->target_yaw = -PI * 0.75f;
//                         view->target_pitch = -0.5f;
//                     } else {

//                     }
//                 }
//                 break;
//             }
//             current_x += option_button.width + HEADER_STYLE.button_spacing;
//         }
//     }
//     return true;
// }

static bool handle_editor_ui_input(Panel* panel, EditorViewData* view, Font font) {
    Rectangle header_rectangle = {panel->bounds.x, panel->bounds.y, panel->bounds.width, HEADER_STYLE.height};
    Vec2 mouse_pos = GetMousePosition();
    f32 current_x = panel->bounds.x + HEADER_STYLE.margin_x;
    for (u8 x = 0; x < 3; x++) {
        view->header_buttons[x]->text = get_header_option_label(view, x);
        Vec2 text_size = MeasureTextEx(font, view->header_buttons[x]->text, HEADER_STYLE.font_size, 1.0f);
        view->header_buttons[x]->bounds = (Rectangle) {
            current_x,
            panel->bounds.y + HEADER_STYLE.margin_y,
            text_size.x + HEADER_STYLE.button_padding,
            HEADER_STYLE.button_height
        };
        current_x += view->header_buttons[x]->bounds.width + HEADER_STYLE.button_spacing;
    }
    if (Vector2Length(GetMouseDelta()) > 0.0f) {
        if (view->hovered_node) {
            view->hovered_node->is_hovered = false;
        }
        view->hovered_node = NULL;
        for (u8 x = 0; x < 3; x++) {
            if (CheckCollisionPointRec(mouse_pos, view->header_buttons[x]->bounds)) {
                view->hovered_node = view->header_buttons[x];
                view->hovered_node->is_hovered = true;
                break;
            }
        }
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && view->hovered_node) {
        view->pressed_node = view->hovered_node;
        view->pressed_node->is_pressed = true;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && view->pressed_node) {
        if (view->hovered_node == view->pressed_node && view->pressed_node->on_click) {
            view->pressed_node->on_click(view->pressed_node->user_data);
        }
        view->hovered_node->is_pressed = false;
        view->pressed_node = NULL;
    }
    return CheckCollisionPointRec(mouse_pos, header_rectangle);
}

static void handle_editor_input(EditorViewData* view, InputManager* input, f32 delta_time) {
    Vec3 input_direction = {0.0f, 0.0f, 0.0f};
    Vec3 forward = Vector3Subtract(view->camera.rl_camera.target, view->camera.rl_camera.position);
    forward = Vector3Normalize(forward);
    Vec3 right = Vector3CrossProduct(forward, view->camera.rl_camera.up);
    right = Vector3Normalize(right);
    if (view->editor_options.show_3D) {
        if (input_is_down(input, ACTION_MOVE_FORWARD)) input_direction = Vector3Add(input_direction, forward);
        if (input_is_down(input, ACTION_MOVE_BACKWARD)) input_direction = Vector3Subtract(input_direction, forward);
        if (input_is_down(input, ACTION_MOVE_RIGHT)) input_direction = Vector3Add(input_direction, right);
        if (input_is_down(input, ACTION_MOVE_LEFT)) input_direction = Vector3Subtract(input_direction, right);
    } else {
        Vec3 up = Vector3CrossProduct(right, forward);
        up = Vector3Normalize(up);
        if (input_is_down(input, ACTION_MOVE_FORWARD)) input_direction = Vector3Add(input_direction, up);
        if (input_is_down(input, ACTION_MOVE_BACKWARD)) input_direction = Vector3Subtract(input_direction, up);
        if (input_is_down(input, ACTION_MOVE_RIGHT)) input_direction = Vector3Add(input_direction, right);
        if (input_is_down(input, ACTION_MOVE_LEFT)) input_direction = Vector3Subtract(input_direction, right);
    }
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        view->camera_speed = EDITOR_ACCELERATED_FLY_SPEED;
    } else {
        view->camera_speed = BASE_EDITOR_FLY_SPEED;
    }
    input_direction = Vector3Normalize(input_direction);
    f32 fly_speed = view->camera_speed * delta_time;
    view->camera.rl_camera.position.x += input_direction.x * fly_speed;
    view->camera.rl_camera.position.y += input_direction.y * fly_speed;
    view->camera.rl_camera.position.z += input_direction.z * fly_speed;
}

static void editor_view_update(Panel* panel, InputManager* input, bool is_focused, f32 delta_time, Font font) {
    EditorViewData* view = (EditorViewData*)panel->data;
    bool input_consumed = handle_editor_ui_input(panel, view, font);
    if (input_consumed == false) {
        handle_world_axes_gizmo_input(panel, view);
    }
    if (view->is_animating) {
        view->anim_timer += delta_time;
        f32 t = view->anim_timer / view->anim_duration;
        if (t >= 1.0f) {
            t = 1.0f;
            view->is_animating = false;
            if (view->editor_options.show_3D == false) {
                view->camera.rl_camera.projection = CAMERA_ORTHOGRAPHIC;
                view->camera.rl_camera.fovy = 20.0f;
            }
        }
        f32 ease_t = t * t * (3.0f - 2.0f * t);
        view->camera.rl_camera.position = Vector3Lerp(view->start_pos, view->target_pos, ease_t);
        view->camera.yaw = Lerp(view->start_yaw, view->target_yaw, ease_t);
        view->camera.pitch = Lerp(view->start_pitch, view->target_pitch, ease_t);
        view->grid_angle = Lerp(view->start_grid_angle, view->target_grid_angle, ease_t);
    } else if (is_focused && input_consumed == false) {
        handle_editor_input(view, input, delta_time);
    }
    bool allow_camera_rotation = is_focused && view->editor_options.show_3D;
    camera_update(&view->camera, &view->camera.rl_camera.position, allow_camera_rotation, delta_time);
}

static void editor_view_render(Panel* panel) {
    EditorViewData* view = (EditorViewData*)panel->data;
    BeginMode3D(view->camera.rl_camera);
    if (view->editor_options.show_grid) {
        render_world_grid(view->grid_shader, view->grid_cam_pos_loc, view->camera.rl_camera.position, view->grid_angle, view->grid_axis);
    }
    if (view->player_ref) {
        player_render(view->player_ref);
    }
    EndMode3D();
}

static void editor_view_cleanup(Panel* panel) {
    EditorViewData* view = (EditorViewData*)panel->data;
    for (u8 x = 0; x < 3; x++) {
        free(view->header_buttons[x]);
    }
    free(panel->data);
    free(panel);
}

static void editor_view_render_overlay(Panel* panel, Font font) {
    EditorViewData* view = (EditorViewData*)panel->data;
    BeginScissorMode((i32)panel->bounds.x, (i32)panel->bounds.y, (i32)panel->bounds.width, (i32)panel->bounds.height);
    draw_world_axes_gizmo(panel, view->camera.rl_camera, font);
    draw_editor_view_options(panel, font);
    EndScissorMode();
}

Panel* editor_view_create(Player* player_ref, Shader grid_shader, i32 cam_pos_loc, Font font) {
    Panel* panel = (Panel*)malloc(sizeof(Panel));
    EditorViewData* data = (EditorViewData*)malloc(sizeof(EditorViewData));
    camera_init_pos(&data->camera, (Vec3){5.0f, 10.0f, 5.0f});
    data->camera_speed = BASE_EDITOR_FLY_SPEED;
    data->player_ref = player_ref;
    data->grid_shader = grid_shader;
    data->grid_cam_pos_loc = cam_pos_loc;
    data->is_animating = false;
    data->grid_angle = 0.0f;
    data->grid_axis = (Vec3){1.0f, 0.0f, 0.0f};
    data->active_axis_id = -1;
    data->saved_fov = data->camera.rl_camera.fovy;
    data->font = font;

    data->hovered_node = NULL;
    data->pressed_node = NULL;
    for (u8 x = 0; x < 3; x++) {
        data->header_buttons[x] = (UINode*)calloc(1, sizeof(UINode));
        data->header_buttons[x]->type = UI_BUTTON;
        data->header_buttons[x]->user_data = data;
    }
    data->header_buttons[0]->on_click = on_3d_toggle_clicked;
    data->header_buttons[1]->on_click = on_grid_toggle_clicked;
    data->header_buttons[2]->on_click = on_reset_view_clicked;

    data->editor_options.show_grid = true;
    data->editor_options.show_3D = true;
    panel->title = "Scene";
    panel->tab_width = MeasureTextEx(font, panel->title, FONT_SIZE, FONT_SPACING).x + 20.0f;
    panel->data = data;
    panel->update = editor_view_update;
    panel->render = editor_view_render;
    panel->render_overlay = editor_view_render_overlay;
    panel->cleanup = editor_view_cleanup;
    return panel;
}
