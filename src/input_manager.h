#pragma once

#include "core.h"

typedef enum {
    ACTION_MOVE_FORWARD,
    ACTION_MOVE_BACKWARD,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,
    ACTION_FLY_UP,
    ACTION_FLY_DOWN,
    ACTION_SPRINT,
    ACTION_TOGGLE_FULLSCREEN,
    ACTION_FOCUS,
    ACTION_MAX
} InputAction;

typedef enum {
    INPUT_KEYBOARD,
    INPUT_MOUSE
} InputType;

typedef struct {
    i32 primary_key;
    i32 secondary_key;
    InputType input_type;
} InputBinding;

typedef struct {
    InputBinding bindings[ACTION_MAX];
    bool state_down[ACTION_MAX];
    bool state_pressed[ACTION_MAX];
    bool state_released[ACTION_MAX];
} InputManager;

void input_manager_init(InputManager* input_manager);
void input_manager_update(InputManager* input_manager);
bool input_is_down(InputManager* input_manager, InputAction input_action);
bool input_is_pressed(InputManager* input_manager, InputAction input_action);
bool input_is_released(InputManager* input_manager, InputAction input_action);