#pragma once

#include "util.h"

typedef struct {
    f64 time_elapsed_seconds;
    u64 cpu_start;
    u64 cpu_end;
} Profiler;

u64 get_cpu_freq_fast();
u64 get_cpu_freq();
u64 read_cpu_timer();
u64 get_os_timer_freq();
u64 read_os_timer();

Profiler init_profiler();
void profiler_start(Profiler *profiler);
void profiler_end(Profiler *profiler);
f64 profiler_time(Profiler profiler);
f64 percentage_time_taken(Profiler profiler, f64 total_time_seconds);
void print_profiler_stats(Profiler profiler, char *profiler_name, f64 total_time_seconds);
