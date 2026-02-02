#include "util.h"
#include "buffer.h"

#define CONCAT_INTERNAL(a, b) a##b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)

typedef struct{
    u64 tsc_elapsed;
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
    char const *name;
} Profile_Block;

typedef struct {
    Profile_Anchor anchors[256];
    u32 count;
} Profiler;

static Profiler global_program_profiler;
static u32 global_profiler_parent;

void begin_profiler();
void end_profiler();
void end_profile(Profile_Block *block);
void profiler_stats();

Profile_Block add_block(char const *name_, u32 anchor_index_);
void end_block(Profile_Block *block);

#define time_block(name_str) Profile_Block __attribute__((cleanup(end_block))) \
    CONCAT(block_, __LINE__) = add_block(name_str, __COUNTER__ + 1)\

/*
#define start_profiler program_profiler = (Profiler){0};\
    program_profiler.blocks[0].name = CONSTANT_STRING("program");\
    program_profiler.blocks[0].cpu_start = read_cpu_timer();\
    program_profiler.at++;\

#define end_profiler end_program_profiler()

#define profile_block(name_str) \
    Profile_Block __attribute__((cleanup(end_profile))) CONCAT(block_, __LINE__) = {\
        .name = CONSTANT_STRING(name_str),\
        .cpu_start = read_cpu_timer(),\
        .cpu_end = 0,\
        .time_seconds = 0,\
        .profiler_at = program_profiler.at\
    };\
    program_profiler.at++;\

#define profile_function profile_block(__func__)
*/
