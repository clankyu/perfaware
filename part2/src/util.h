#pragma once

#include <stdint.h>

#define CLUSTER_FLAG 1

#define array_count(array) (sizeof(array) / sizeof((array)[0]))

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef u8 b8;
typedef u16 b16;
typedef u32 b32;
typedef u64 b64;

#define bool u32
#define true 1
#define false 0
