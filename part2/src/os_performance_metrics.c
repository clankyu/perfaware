#include <unistd.h>
#include <x86intrin.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#include "os_performance_metrics.h"

u64 read_cpu_timer() {
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

Profiler init_profiler() {
    Profiler result = {0};
    return result;
}

void profiler_start(Profiler *profiler) {
    profiler->cpu_start = read_cpu_timer();
}

void profiler_end(Profiler *profiler) {
    profiler->cpu_end = read_cpu_timer();

    u64 cpu_freq = get_cpu_freq_fast();
    f64 result = ((f64)profiler->cpu_end - (f64)profiler->cpu_start) / (f64)cpu_freq;
    profiler->time_elapsed_seconds = result;
}

void profiler_calculate(Profiler *profiler) {
    u64 cpu_freq = get_cpu_freq_fast();
    f64 result = ((f64)profiler->cpu_end - (f64)profiler->cpu_start) / (f64)cpu_freq;
    profiler->time_elapsed_seconds = result;
}

f64 profiler_time(Profiler profiler) {
    return profiler.time_elapsed_seconds;
}

f64 percentage_time_taken(Profiler profiler, f64 total_time_seconds) {
    return profiler.time_elapsed_seconds / total_time_seconds * 100.0f;
}

void print_profiler_stats(Profiler profiler, char *profiler_name, f64 total_time_seconds) {
    f64 percent_time_taken = percentage_time_taken(profiler, total_time_seconds);
    u64 cycles_elapsed = profiler.cpu_end - profiler.cpu_start;
    printf("%s: %lu cycles elapsed, %lf seconds elapsed (%.4f%%)\n", profiler_name, cycles_elapsed, profiler.time_elapsed_seconds, percent_time_taken);
}
