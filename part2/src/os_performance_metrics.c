#include <unistd.h>
#include <x86intrin.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#include "os_performance_metrics.h"

Profiler program_profiler;

inline u64 read_cpu_timer() {
    return __rdtsc();
}

u64 get_cpu_freq() {
    u64 os_freq = get_os_timer_freq();

    u64 os_start = read_os_timer();
    u64 os_end = 0;
    u64 os_elapsed = 0;

    u64 cpu_start = read_cpu_timer();

    while (os_elapsed < os_freq) {
        os_end = read_os_timer();
        os_elapsed = os_end - os_start;
    }

    u64 cpu_end = read_cpu_timer();
    u64 cpu_elapsed = cpu_end - cpu_start;
    u64 cpu_freq = 0;

    if (os_elapsed) {
        cpu_freq = os_freq * cpu_elapsed / os_elapsed;
    }

    return cpu_freq;
}

u64 get_cpu_freq_fast() {
    static u64 cpu_frequency = 0;

    if (!cpu_frequency) {
        FILE *fp = fopen("/proc/cpuinfo", "r");
        if (!fp) return 0;

        char line[256];
        double mhz = 0;
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "cpu MHz", 7) == 0) {
                sscanf(line, "cpu MHz : %lf", &mhz);
                break;
            }
        }

        fclose(fp);
        cpu_frequency = (uint64_t)(mhz * 1000000);
    }

    return cpu_frequency;
}

u64 read_os_timer() {
    struct timeval value;
    gettimeofday(&value, 0);
    u64 result = get_os_timer_freq()*(u64)value.tv_sec + (u64)value.tv_usec;
    return result;
}

u64 get_os_timer_freq() {
    return 1000000;
}

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
