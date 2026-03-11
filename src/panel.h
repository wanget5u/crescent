#pragma once

#include "core.h"
#include "input_manager.h"

typedef struct Panel Panel;
struct Panel {
    const char* title;
    void* data;
    void (*update)(Panel* panel, InputManager* input, bool is_focused, f32 delta_time);
    void (*render)(Panel* panel);
    void (*cleanup)(Panel* panel);
};