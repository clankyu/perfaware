#pragma once

#include "util.h"
#include "buffer.h"

typedef struct {
    Buffer name;
    u64 cpu_start;
    u64 cpu_end;
    f64 time_seconds;
    u32 profiler_at;
} Profile_Block;

typedef struct {
    Profile_Block blocks[256];
    u32 at;
} Profiler;

extern Profiler program_profiler;

u64 get_cpu_freq_fast();
u64 get_cpu_freq();
u64 read_cpu_timer();
u64 get_os_timer_freq();
u64 read_os_timer();

void end_profile(Profile_Block *block);
void end_program_profiler();
void profiler_stats();

#define CONCAT_INTERNAL(a, b) a##b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)

#define start_profiler program_profiler = (Profiler){0};\
    program_profiler.blocks[0].name = CONSTANT_STRING("program");\
    program_profiler.blocks[0].cpu_start = read_cpu_timer();\
    program_profiler.at++;\

#define end_profiler end_program_profiler()

#define profile_function \
    Profile_Block __attribute__((cleanup(end_profile))) function_block = {\
        .name = CONSTANT_STRING(__func__),\
        .cpu_start = read_cpu_timer(),\
        .cpu_end = 0,\
        .time_seconds = 0,\
        .profiler_at = program_profiler.at\
    };\
    program_profiler.at++;\

#define profile_scope(name_str) \
    Profile_Block __attribute__((cleanup(end_profile))) CONCAT(block_, __LINE__) = {\
        .name = CONSTANT_STRING(name_str),\
        .cpu_start = read_cpu_timer(),\
        .cpu_end = 0,\
        .time_seconds = 0,\
        .profiler_at = program_profiler.at\
    };\
    program_profiler.at++;\
