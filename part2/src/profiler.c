#include "profiler.h"
#include "util.h"
#include <stdio.h>
#include "os_performance_metrics.h"

void begin_profiler() {
    global_program_profiler = (Profiler){0};
    global_program_profiler.anchors[0].name = "program";
    global_program_profiler.start_tsc = read_cpu_timer();
}

void end_profiler() {
    global_program_profiler.end_tsc = read_cpu_timer();
}

Profile_Block add_block(char const *name_, u32 anchor_index_) {
    Profile_Block result = {
        .anchor_index = anchor_index_,
        .parent_index = global_profiler_parent,
        .name = name_
    };

    global_profiler_parent = result.anchor_index;
    global_program_profiler.anchors[anchor_index_].hit_count++;

    result.start_tsc = read_cpu_timer();

    return result;
}

void end_block(Profile_Block *block) {
    u64 start_tsc = block->start_tsc;
    u64 end_tsc = read_cpu_timer();
    u64 tsc_elapsed = end_tsc - start_tsc;

    global_profiler_parent = block->parent_index;

    Profile_Anchor *anchor = global_program_profiler.anchors + block->anchor_index;
    Profile_Anchor *parent_anchor = global_program_profiler.anchors + block->parent_index;

    anchor->tsc_elapsed += tsc_elapsed;
    parent_anchor->children_tsc_elapsed += tsc_elapsed;
    ++anchor->hit_count;

    anchor->name = block->name;
}

void print_time_elapsed(u64 total_tsc_elapsed, Profile_Anchor *anchor) {
    u64 cpu_freq = get_cpu_freq_fast();
    u64 elapsed = anchor->tsc_elapsed - anchor->children_tsc_elapsed;
    f64 percent = (f64)elapsed / (f64)total_tsc_elapsed * 100.0f;

    f64 seconds_elapsed = (f64)elapsed / (f64)cpu_freq;

    printf("  %s[%lu]: %lu (%lf seconds (%.2f%%)", anchor->name, anchor->hit_count, elapsed, seconds_elapsed, percent);
    if (anchor->children_tsc_elapsed) {
        f64 percent_with_children = (f64)(elapsed + anchor->children_tsc_elapsed) / (f64) total_tsc_elapsed * 100.0f;
        f64 seconds_elapsed_with_children = (f64)(elapsed + anchor->children_tsc_elapsed) / (f64)cpu_freq;
        printf(", %lf seconds (%.2f%%) w/children", seconds_elapsed_with_children, percent_with_children);
    }

    printf(")\n");
}

void print_profile() {
    u64 start_tsc = global_program_profiler.start_tsc;
    u64 end_tsc = global_program_profiler.end_tsc;

    u64 total_tsc_elapsed = end_tsc - start_tsc;

    for (u32 anchor_index = 0; anchor_index < array_count(global_program_profiler.anchors); anchor_index++) {
        Profile_Anchor *anchor = &global_program_profiler.anchors[anchor_index];

        if (anchor->tsc_elapsed) {
            print_time_elapsed(total_tsc_elapsed, anchor);
        }
    }
}
