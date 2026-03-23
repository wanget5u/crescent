#pragma once

#include "core.h"
#include "../user interface/ui_manager.h"
#include "input_manager.h"
#include "../player.h"

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
	UIManager user_interface;
	f32 delta_time;
	InputManager input;
	Player player;
	ViewFocus current_focus;
	Shader grid_shader;
	i32 previous_screen_width;
	i32 previous_screen_height;
	i32 grid_cam_pos;
} GameManager;

void game_manager_init(GameManager* game_manager);
void game_manager_update(GameManager* game_manager);
void game_manager_handle_input(GameManager* game_manager);
void game_manager_render(GameManager* game_manager);
void game_manager_cleanup(GameManager* game_manager);
