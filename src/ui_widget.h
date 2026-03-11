#pragma once

#include "core.h"

typedef enum {
    WIDGET_PANEL,
    WIDGET_BUTTON,
    WIDGET_LABEL,
    WIDGET_TREE_VIEW
} WidgetType;

typedef struct UIWidget {
    WidgetType type;
    Rectangle local_bounds;
    Rectangle world_bounds;
    bool is_hovered;
    bool is_focused;
    struct UIWidget* parent;
    struct UIWidget** children;
    i32 child_count;
    union {
        struct { char text[64]; bool clicked; } button;
        struct { char text[256]; } label;
    } data;
} UIWidget;