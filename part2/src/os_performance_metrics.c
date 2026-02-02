#include <unistd.h>
#include <x86intrin.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#include "os_performance_metrics.h"

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
