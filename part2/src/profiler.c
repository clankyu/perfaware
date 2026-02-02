#include "profiler.h"
#include "util.h"
#include "os_performance_metrics.h"

void begin_profiler() {
    global_program_profiler.count++;
    global_program_profiler.anchors[0].name = "program";
}

void end_profiler() {
    u64 tsc_elapsed = read_cpu_timer();
    u64 cpu_freq = get_cpu_freq_fast();
    f64 total_seconds_elapsed = (f64)tsc_elapsed / (f64)cpu_freq;
}

Profile_Block add_block(char const *name_, u32 anchor_index_) {
    Profiler p = global_program_profiler;
    Profile_Block result = {
        .start_tsc = get_cpu_freq_fast(),
        .anchor_index = anchor_index_,
        .name = name_
    };

    p.anchors[anchor_index_].hit_count++;
    p.count++;

    return result;
}

void end_block(Profile_Block *block) {

}
/*
void end_profile(Profile_Block *block) {
    block->cpu_end = read_cpu_timer();
    u64 cpu_freq = get_cpu_freq_fast();
    f64 result = ((f64)block->cpu_end - (f64)block->cpu_start) / (f64)cpu_freq;
    block->time_seconds = result;

    program_profiler.blocks[block->profiler_at] = *block;
}


void end_program_profiler() {
    Profile_Block *program_block = &program_profiler.blocks[0];
    program_block->cpu_end = read_cpu_timer();
    u64 cpu_freq = get_cpu_freq_fast();
    f64 result = ((f64)program_block->cpu_end - (f64)program_block->cpu_start) / (f64)cpu_freq;
    program_block->time_seconds = result;
}

void profiler_stats() {
    f64 program_total_elapsed = program_profiler.blocks[0].time_seconds;
    for (u32 i = 0; i < program_profiler.at; i++) {
        Profile_Block block = program_profiler.blocks[i];
        f64 percent_time_taken = block.time_seconds / program_total_elapsed * 100.0f;
        u64 cycles_elapsed = block.cpu_end - block.cpu_start;
        printf("%.*s: %lu cycles elapsed, %lf seconds elapsed (%.4f%%)\n", block.name.count, block.name.data, cycles_elapsed, block.time_seconds, percent_time_taken);
    }
}
*/
