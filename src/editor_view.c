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
} EditorViewData;

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

static void editor_view_update(Panel* panel, InputManager* input, bool is_focused, f32 delta_time) {
    EditorViewData* view = (EditorViewData*)panel->data;
    if (is_focused) {
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

Panel* editor_view_create(Player* player_ref, Shader grid_shader, i32 cam_pos_loc) {
    Panel* panel = (Panel*)malloc(sizeof(Panel));
    EditorViewData* data = (EditorViewData*)malloc(sizeof(EditorViewData));
    camera_init_pos(&data->camera, (Vec3){5.0f, 10.0f, 5.0f});
    data->camera_speed = BASE_EDITOR_FLY_SPEED;
    data->player_ref = player_ref;
    data->grid_shader = grid_shader;
    data->grid_cam_pos_loc = cam_pos_loc;
    panel->title = "Scene";
    panel->data = data;
    panel->update = editor_view_update;
    panel->render = editor_view_render;
    panel->cleanup = editor_view_cleanup;
    return panel;
}