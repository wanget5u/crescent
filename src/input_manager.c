#include "input_manager.h"

static void bind_keyboard(InputManager* input, InputAction input_action, i32 primary_key, i32 secondary_key) {
    input->bindings[input_action].primary_key = primary_key;
    input->bindings[input_action].secondary_key = secondary_key;
    input->bindings[input_action].input_type = INPUT_KEYBOARD;
}

static void bind_mouse(InputManager* input, InputAction input_action, i32 button) {
    input->bindings[input_action].primary_key = button;
    input->bindings[input_action].secondary_key = KEY_NULL; 
    input->bindings[input_action].input_type = INPUT_MOUSE;
}

void input_init(InputManager* input) {
    for (i32 x = 0; x < ACTION_MAX; x++) {
        input->state_down[x] = false;
        input->state_pressed[x] = false;
        input->state_released[x] = false;
    }
    bind_keyboard(input, ACTION_MOVE_FORWARD, KEY_W, KEY_UP);
    bind_keyboard(input, ACTION_MOVE_BACKWARD, KEY_S, KEY_DOWN);
    bind_keyboard(input, ACTION_MOVE_LEFT, KEY_A, KEY_LEFT);
    bind_keyboard(input, ACTION_MOVE_RIGHT, KEY_D, KEY_RIGHT);
    bind_keyboard(input, ACTION_SPRINT, KEY_LEFT_SHIFT, KEY_NULL);
    bind_keyboard(input, ACTION_TOGGLE_FULLSCREEN, KEY_F11, KEY_NULL);
    bind_mouse(input, ACTION_FOCUS, MOUSE_BUTTON_RIGHT);
}

void input_update(InputManager* input) {
    for (i32 x = 0; x < ACTION_MAX; x++) {
        i32 key1 = input->bindings[x].primary_key;
        i32 key2 = input->bindings[x].secondary_key;
        InputType type = input->bindings[x].input_type;
        if (type == INPUT_KEYBOARD) {
            input->state_down[x] = IsKeyDown(key1) || (key2 != KEY_NULL && IsKeyDown(key2));
            input->state_pressed[x] = IsKeyPressed(key1) || (key2 != KEY_NULL && IsKeyPressed(key2));
            input->state_released[x] = IsKeyReleased(key1) || (key2 != KEY_NULL && IsKeyReleased(key2));
        } else if (type == INPUT_MOUSE) {
            input->state_down[x] = IsMouseButtonDown(key1);
            input->state_pressed[x] = IsMouseButtonPressed(key1);
            input->state_released[x] = IsMouseButtonReleased(key1);
        }
    }
}

bool input_is_down(InputManager* input, InputAction input_action) {
    return input->state_down[input_action];
}

bool input_is_pressed(InputManager* input, InputAction input_action) {
    return input->state_pressed[input_action];
}

bool input_is_released(InputManager* input, InputAction input_action) {
    return input->state_released[input_action];
}