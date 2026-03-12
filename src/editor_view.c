#include <stdlib.h>
#include "graphics_utils.h"
#include "editor_view.h"
#include "game_camera.h"

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
} EditorViewData;

typedef struct {
    Vec2 pos;
    f32 depth;
    Color color;
    const char* label;
    i32 id;
} GizmoAxis;

typedef struct {
    f32 margin_right;
    f32 margin_top;
    f32 line_length;
    f32 tip_radius;
    f32 bg_radius;
    f32 line_thickness;
} GizmoStyle;

static const GizmoStyle GIZMO_STYLE = {
    .margin_right = 60.0f,
    .margin_top = 60.0f,
    .line_length = 35.0f,
    .tip_radius = 8.0f,
    .bg_radius = 43.0f,
    .line_thickness = 2.0f
};

static void calculate_gizmo_axes(Camera3D camera, GizmoAxis* out_axes) {
    Matrix view_mat = GetCameraMatrix(camera);
    f32 length = GIZMO_STYLE.line_length; 
    Vec2 axis_x = Vector2Scale((Vec2){view_mat.m0, -view_mat.m1}, length);
    Vec2 axis_y = Vector2Scale((Vec2){view_mat.m4, -view_mat.m5}, length);
    Vec2 axis_z = Vector2Scale((Vec2){view_mat.m8, -view_mat.m9}, length);
    out_axes[0] = (GizmoAxis){axis_x, view_mat.m2, RED, "X", 0};
    out_axes[1] = (GizmoAxis){axis_y, view_mat.m6, GREEN, "Y", 1};
    out_axes[2] = (GizmoAxis){axis_z, view_mat.m10, BLUE, "Z", 2};
}

static void draw_world_axes_gizmo(Panel* panel, Camera3D camera, Font font) {
    Vec2 center = { 
        panel->bounds.x + panel->bounds.width - GIZMO_STYLE.margin_right, 
        panel->bounds.y + GIZMO_STYLE.margin_top 
    };
    Vec2 global_mouse = GetMousePosition();
    bool is_hovered = CheckCollisionPointCircle(global_mouse, center, GIZMO_STYLE.bg_radius);
    GizmoAxis axes[3];
    calculate_gizmo_axes(camera, axes);
    i32 closest_id = -1;
    if (is_hovered) {
        f32 min_dist = 99999.0f;
        for (i32 i = 0; i < 3; i++) {
            Vec2 tip_global = Vector2Add(center, axes[i].pos);
            f32 dist = Vector2Distance(global_mouse, tip_global);
            if (dist < min_dist) {
                min_dist = dist;
                closest_id = axes[i].id;
            }
        }
    }
    Color bg_color = is_hovered ? ColorAlpha(RAYWHITE, 0.15f) : ColorAlpha(BLACK, 0.3f);
    DrawCircleV(center, GIZMO_STYLE.bg_radius, bg_color);
    for (i32 x = 0; x < 2; x++) {
        for (i32 y = 0; y < 2 - x; y++) {
            if (axes[y].depth > axes[y + 1].depth) {
                GizmoAxis temp = axes[y];
                axes[y] = axes[y + 1];
                axes[y + 1] = temp;
            }
        }
    }
    for (i32 x = 0; x < 3; x++) {
        Vec2 end_pos = Vector2Add(center, axes[x].pos);
        DrawLineEx(center, end_pos, GIZMO_STYLE.line_thickness, axes[x].color);
        DrawCircleV(end_pos, GIZMO_STYLE.tip_radius, axes[x].color);
        Color text_color = (is_hovered && axes[x].id == closest_id) ? WHITE : (Color){48, 48, 48, 255};
        f32 label_size = 16.0f;
        f32 label_spacing = 1.0f;
        Vec2 text_size = MeasureTextEx(font, axes[x].label, label_size, label_spacing);
        Vec2 text_pos = { 
            end_pos.x - (text_size.x / 2.0f), 
            end_pos.y - (text_size.y / 2.0f) 
        };
        DrawTextEx(font, axes[x].label, text_pos, label_size, label_spacing, text_color);
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
        GizmoAxis axes[3];
        calculate_gizmo_axes(view->camera.rl_camera, axes);
        f32 min_dist = 99999.0f;
        i32 closest_id = -1;
        for (i32 i = 0; i < 3; i++) {
            Vec2 tip_global = Vector2Add(global_gizmo_center, axes[i].pos);
            f32 dist = Vector2Distance(global_mouse, tip_global);
            if (dist < min_dist) {
                min_dist = dist;
                closest_id = axes[i].id;
            }
        }
        if (closest_id != -1) {
            view->is_animating = true;
            view->anim_timer = 0.0f;
            view->anim_duration = 0.2f; 
            view->start_pos = view->camera.rl_camera.position;
            view->start_yaw = view->camera.yaw;
            view->start_pitch = view->camera.pitch;
            f32 distance = 10.0f;
            if (closest_id == 0) {
                view->target_pos = (Vec3){distance, 0.0f, 0.0f};
                view->target_yaw = -PI / 2.0f; 
                view->target_pitch = 0.0f;
            } else if (closest_id == 1) {
                view->target_pos = (Vec3){0.0f, distance, 0.0f};
                view->target_yaw = PI; 
                view->target_pitch = -1.5f;
            } else if (closest_id == 2) {
                view->target_pos = (Vec3){0.0f, 0.0f, distance};
                view->target_yaw = PI; 
                view->target_pitch = 0.0f;
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

static void handle_editor_input(EditorViewData* view, InputManager* input, f32 delta_time) {
    Vec3 input_direction = {0.0f, 0.0f, 0.0f};
    Vec3 forward = Vector3Subtract(view->camera.rl_camera.target, view->camera.rl_camera.position);
    forward = Vector3Normalize(forward);
    Vec3 right = Vector3CrossProduct(forward, view->camera.rl_camera.up);
    right = Vector3Normalize(right);
    if (input_is_down(input, ACTION_MOVE_FORWARD))  input_direction = Vector3Add(input_direction, forward);
    if (input_is_down(input, ACTION_MOVE_BACKWARD)) input_direction = Vector3Subtract(input_direction, forward);
    if (input_is_down(input, ACTION_MOVE_RIGHT))    input_direction = Vector3Add(input_direction, right);
    if (input_is_down(input, ACTION_MOVE_LEFT))     input_direction = Vector3Subtract(input_direction, right);
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
    (void) font;
    EditorViewData* view = (EditorViewData*)panel->data;
    handle_world_axes_gizmo_input(panel, view);
    if (view->is_animating) {
        view->anim_timer += delta_time;
        f32 t = view->anim_timer / view->anim_duration;
        if (t >= 1.0f) {
            t = 1.0f;
            view->is_animating = false;
        }
        f32 ease_t = t * t * (3.0f - 2.0f * t);
        view->camera.rl_camera.position = Vector3Lerp(view->start_pos, view->target_pos, ease_t);
        view->camera.yaw = Lerp(view->start_yaw, view->target_yaw, ease_t);
        view->camera.pitch = Lerp(view->start_pitch, view->target_pitch, ease_t);
        
    } else if (is_focused) {
        handle_editor_input(view, input, delta_time);
    }
    camera_update(&view->camera, &view->camera.rl_camera.position, is_focused, delta_time);
}

static void editor_view_render(Panel* panel) {
    EditorViewData* view = (EditorViewData*)panel->data;
    BeginMode3D(view->camera.rl_camera);
    render_environment_grid(view->grid_shader, view->grid_cam_pos_loc, view->camera.rl_camera.position);
    if (view->player_ref) {
        player_render(view->player_ref);
    }
    EndMode3D();
}

static void editor_view_cleanup(Panel* panel) {
    free(panel->data);
    free(panel);
}

static void editor_view_render_overlay(Panel* panel, Font font) {
    EditorViewData* view = (EditorViewData*)panel->data;
    BeginScissorMode((i32)panel->bounds.x, (i32)panel->bounds.y, (i32)panel->bounds.width, (i32)panel->bounds.height);
    draw_world_axes_gizmo(panel, view->camera.rl_camera, font);
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
    panel->title = "Scene";
    data->font = font;
    panel->data = data;
    panel->update = editor_view_update;
    panel->render = editor_view_render;
    panel->render_overlay = editor_view_render_overlay;
    panel->cleanup = editor_view_cleanup;
    return panel;
}