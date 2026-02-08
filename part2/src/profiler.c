#include <stdio.h>

#include "profiler.h"

#ifdef PROFILER

#include "util.h"
#include "os_performance_metrics.h"
#include <stdio.h>

Profile_Block add_block(char const *name_, u32 anchor_index_, u64 byte_count) {
    Profile_Block result = {
        .anchor_index = anchor_index_,
        .parent_index = global_profiler_parent,
        .name = name_
    };

    Profile_Anchor *anchor = global_profiler_anchors + anchor_index_;
    result.old_tsc_elapsed_inclusive = anchor->tsc_elapsed_inclusive;
    anchor->processed_byte_count += byte_count;

    global_profiler_parent = result.anchor_index;

    result.start_tsc = read_cpu_timer();

    return result;
}

void end_block(Profile_Block *block) {
    u64 start_tsc = block->start_tsc;
    u64 end_tsc = read_cpu_timer();
    u64 tsc_elapsed = end_tsc - start_tsc;

    global_profiler_parent = block->parent_index;

    Profile_Anchor *anchor = global_profiler_anchors + block->anchor_index;
    Profile_Anchor *parent_anchor = global_profiler_anchors + block->parent_index;

    parent_anchor->tsc_elapsed_exclusive -= tsc_elapsed;
    anchor->tsc_elapsed_exclusive += tsc_elapsed;
    anchor->tsc_elapsed_inclusive = block->old_tsc_elapsed_inclusive + tsc_elapsed;

    ++anchor->hit_count;
    anchor->name = block->name;
}

void print_time_elapsed(u64 total_tsc_elapsed, Profile_Anchor *anchor) {
    u64 cpu_freq = get_cpu_freq_fast();
    f64 percent = (f64)anchor->tsc_elapsed_exclusive / (f64)total_tsc_elapsed * 100.0f;
    printf("tsc_elapsed_exclusive: %lu\ntotal_tsc_elapsed: %lu\n", anchor->tsc_elapsed_exclusive, total_tsc_elapsed);

    f64 seconds_elapsed = (f64)anchor->tsc_elapsed_exclusive / (f64)cpu_freq;

    printf("  %s[%lu]: %lu (%lf seconds (%.2f%%)", anchor->name, anchor->hit_count, anchor->tsc_elapsed_exclusive, seconds_elapsed, percent);
    if (anchor->tsc_elapsed_inclusive != anchor->tsc_elapsed_exclusive) {
        f64 percent_with_children = (f64)(anchor->tsc_elapsed_inclusive) / (f64) total_tsc_elapsed * 100.0f;
        f64 seconds_elapsed_with_children = (f64)(anchor->tsc_elapsed_inclusive) / (f64)cpu_freq;
        printf(", %lf seconds (%.2f%%) w/children", seconds_elapsed_with_children, percent_with_children);
    }

    printf(")");

    if (anchor->processed_byte_count) {
        f64 megabyte = 1024.0f*1024.0f;
        f64 gigabyte = megabyte*1024.0f;

        f64 seconds = (f64)anchor->tsc_elapsed_inclusive / (f64)cpu_freq;
        f64 bytes_per_second = (f64)anchor->processed_byte_count / seconds;
        f64 megabytes = (f64)anchor->processed_byte_count / (f64)megabyte;
        f64 gigabytes_per_second = (f64)anchor->processed_byte_count / (f64)gigabyte;

        printf("  %.3fmb at %.2fgb/s", megabytes, gigabytes_per_second);
    }

    printf("\n");
}

void print_anchor_data(u64 total_tsc_elapsed) {
    for (u32 anchor_index = 0; anchor_index < array_count(global_profiler_anchors); ++anchor_index) {
        Profile_Anchor *anchor = global_profiler_anchors + anchor_index;
        if (anchor->tsc_elapsed_inclusive) {
            print_time_elapsed(total_tsc_elapsed, anchor);
        }
    }
}

#endif

void begin_profiler() {
    global_program_profiler.start_tsc = read_cpu_timer();
}

void end_profiler() {
    global_program_profiler.end_tsc = read_cpu_timer();
}

void print_profile() {
    u64 cpu_freq = get_cpu_freq_fast();
    u64 start_tsc = global_program_profiler.start_tsc;
    u64 end_tsc = global_program_profiler.end_tsc;

    u64 total_tsc_elapsed = end_tsc - start_tsc;

    if (cpu_freq) {
        printf("\nTotal time: %0.4fms (CPU freq %lu)\n", 1000.0 * (f64)total_tsc_elapsed / (f64)cpu_freq, cpu_freq);
    }

    print_anchor_data(total_tsc_elapsed);
}


