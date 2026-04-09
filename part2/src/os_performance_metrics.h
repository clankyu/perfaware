#pragma once

#include <stdint.h>

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

#include "buffer.h"
#include "util.h"

void initialize_os_metrics();
u64 get_os_minor_page_faults();
u64 get_cpu_freq_fast();
u64 get_cpu_freq();
u64 read_cpu_timer();
u64 get_os_timer_freq();
u64 read_os_timer();
