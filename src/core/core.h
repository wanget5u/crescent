#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <raylib.h>
#include <raymath.h>

// standard aliases
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

// raylib alises
typedef Vector2 Vec2;
typedef Vector3 Vec3;

#define MIN_SCREEN_WIDTH (640)
#define MIN_SCREEN_HEIGHT (480)
#define BASE_SCREEN_WIDTH (1600)
#define BASE_SCREEN_HEIGHT (900)
#define BASE_TEXTURE_FILTER TEXTURE_FILTER_BILINEAR
#define DOCK_SPLITTER_THICKNESS (8.0f)
#define DOCK_SPLITTER_COLOR DARKGRAY

#define TEXT_COLOR DARKGRAY
#define BG_COLOR GRAY
#define GRID_COLOR LIGHTGRAY
#define VIEWPORT_FOCUS_COLOR WHITE

#define FONT_SIZE (25.0f)
#define FONT_SPACING (1.0f)

#define PLAYER_SPEED (5.0f)
#define PLAYER_ACC (15.0f)
#define PLAYER_COLOR RED

#define GRID_SCALE (1000.0f)
#define BASE_EDITOR_FLY_SPEED (15.0f)
#define EDITOR_ACCELERATED_FLY_SPEED (40.0f);

#define CAMERA_FOV (70.0f)

#define MOUSE_SENS (0.0035f)
#define SCROLL_SPEED (4)
