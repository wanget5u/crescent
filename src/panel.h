#pragma once

#include "core.h"
#include "input_manager.h"

typedef struct Panel Panel;
struct Panel {
    const char* title;
    f32 tab_width;
    Rectangle bounds;
    void* data;
    void (*update)(Panel* panel, InputManager* input, bool is_focused, f32 delta_time, Font font);
    void (*render)(Panel* panel);
    void (*render_overlay)(struct Panel* panel, Font font);
    void (*cleanup)(Panel* panel);
};
