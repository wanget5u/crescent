#pragma once

#include "core.h"
#include "player.h"
#include "game_view.h"
#include "editor_view.h"

typedef enum {
    FOCUS_NONE,
    FOCUS_GAME,
    FOCUS_EDITOR
} ViewFocus;

typedef enum {
    STATE_EDITING,
    STATE_RUNNING
} GameState;

typedef struct {
    GameState game_state;
    f32 delta_time;
    Player player;
    GameView game_view;
    EditorView editor_view;
    ViewFocus current_focus;
    Font font;
} GameManager;

void game_manager_init(GameManager* game_manager);
void game_manager_update(GameManager* game_manager);
void game_manager_handle_input(GameManager* game_manager);
void game_manager_render(GameManager* game_manager);
void game_manager_cleanup(GameManager* game_manager);