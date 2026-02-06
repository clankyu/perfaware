#pragma once

#include "util.h"
#include "buffer.h"
#include "os_performance_metrics.h"

#ifdef PROFILER

typedef struct {
    u64 tsc_elapsed_exclusive;
    u64 tsc_elapsed_inclusive;
    u64 hit_count;
    char const *name;
} Profile_Anchor;

typedef struct {
    u64 start_tsc;
    u64 old_tsc_elapsed_inclusive;
    u32 anchor_index;
    u32 parent_index;
    char const *name;
} Profile_Block;

static Profile_Anchor global_profiler_anchors[256];
static u32 global_profiler_parent;

Profile_Block add_block(char const *name_, u32 anchor_index_);
void end_block(Profile_Block *block);

void print_time_elapsed(u64 total_tsc_elapsed, Profile_Anchor *anchor);
void print_anchor_data(u64 total_tsc_elapsed);

#define CONCAT_INTERNAL(a, b) a##b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)

#define time_block(name_str) Profile_Block __attribute__((cleanup(end_block))) \
    CONCAT(block_, __LINE__) = add_block(name_str, __COUNTER__ + 1)\

#define time_function time_block(__func__)

#endif

#ifndef PROFILER
#define time_block(...)
#define time_function
#define print_anchor_data(...)
#endif

typedef struct {
    u64 start_tsc;
    u64 end_tsc;
} Profiler;

static Profiler global_program_profiler;

void begin_profiler();
void end_profiler();
void print_profile();
