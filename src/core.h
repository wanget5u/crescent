#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <time.h>

#include "tools.h"

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

typedef int32_t b32;

// raylib alises
typedef Vector2 Vec2;
typedef Vector3 Vec3;

#define PLAYER_SPEED 800.0f
#define PLAYER_ACC 10.0f
#define PLAYER_COLOR WHITE