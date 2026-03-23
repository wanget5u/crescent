#pragma once

#include "../core/core.h"

typedef enum {
    UI_BUTTON,
    UI_PANEL,
    UI_TEXT,
    UI_GIZMO
} UIType;

typedef struct UINode UINode;
struct UINode{
    UIType type;
    Rectangle bounds;
    const char* text;
    bool is_hovered;
    bool is_pressed;
    UINode* parent;
    UINode** children;
    i32 children_count;
    void (*on_click)(void* user_data);
    void* user_data;
};
