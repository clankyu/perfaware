#include "util.h"
#include "buffer.h"
#include "os_performance_metrics.h"

#define CONCAT_INTERNAL(a, b) a##b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)

typedef struct{
    u64 tsc_elapsed;
    u64 children_tsc_elapsed;
    u64 hit_count;
    char const *name;
} Profile_Anchor;

/*
typedef struct {
    Buffer name;
    u64 cpu_start;
    u64 cpu_end;
    f64 time_seconds;
    u32 profiler_at;
} Profile_Block;
*/

typedef struct {
    u64 start_tsc;
    u32 anchor_index;
    u32 parent_index;
    char const *name;
} Profile_Block;

typedef struct {
    Profile_Anchor anchors[256];
    u64 start_tsc;
    u64 end_tsc;
} Profiler;

static Profiler global_program_profiler;
static u32 global_profiler_parent;

void begin_profiler();
void end_profiler();
void end_block(Profile_Block *block);
void print_time_elapsed(u64 total_tsc_elapsed, Profile_Anchor *anchor);
void print_profile();

Profile_Block add_block(char const *name_, u32 anchor_index_);
void end_block(Profile_Block *block);

#define time_block(name_str) Profile_Block __attribute__((cleanup(end_block))) \
    CONCAT(block_, __LINE__) = add_block(name_str, __COUNTER__ + 1)\

#define time_function time_block(__func__)
