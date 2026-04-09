#include <stdio.h>
#include <string.h>

#include "os_performance_metrics.h"

inline u64 read_cpu_timer() {
    return __rdtsc();
}

#ifdef __linux__
#include <unistd.h>
#include <x86intrin.h>
#include <sys/time.h>
#include <sys/resource.h>

typedef struct {
    b32 initialized;
} Os_Metrics;

static Os_Metrics global_os_metrics;

void initialize_os_metrics() {
    global_os_metrics.initialized = true;
    get_cpu_freq_fast();
}

u64 get_os_minor_page_faults() {
    u64 result = 0;
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        result = usage.ru_minflt;
    } else {
        fprintf(stderr, "Error: Failed to get page fault count\n");
    }

    return result;
}


u64 get_cpu_freq() {
    return get_cpu_freq_fast();
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

#elif _WIN32
#include <windows.h>
#include <intrin.h>
#include <psapi.h>

typedef struct {
    b32 initialized;
    HANDLE process_handle;
} Os_Metrics;


static Os_Metrics global_os_metrics;

void initialize_os_metrics() {
    if (!global_os_metrics.initialized) {
        global_os_metrics.initialized = true;
        global_os_metrics.process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
        u64 freq = get_cpu_freq();
    }
}

u64 get_os_minor_page_faults() {
    PROCESS_MEMORY_COUNTERS_EX memory_counters;
    memory_counters.cb = sizeof(memory_counters);
    GetProcessMemoryInfo(global_os_metrics.process_handle, (PROCESS_MEMORY_COUNTERS *)&memory_counters, sizeof(memory_counters));

    u64 result = memory_counters.PageFaultCount;

    return result;
}

u64 get_cpu_freq_fast() {
    return get_cpu_freq();
}

u64 get_cpu_freq() {
    static u64 cpu_freq = 0;

    if (!cpu_freq) {
        u64 ms_to_wait = 100;
        u64 os_freq = get_os_timer_freq();

        u64 cpu_start = read_cpu_timer();
        u64 os_start = read_os_timer();
        u64 os_end = 0;
        u64 os_elapsed = 0;
        u64 os_wait_time = os_freq * ms_to_wait / 1000;
        while (os_elapsed < os_wait_time) {
            os_end = read_os_timer();
            os_elapsed = os_end - os_start;
        }

        u64 cpu_end = read_cpu_timer();
        u64 cpu_elapsed = cpu_end - cpu_start;

        if (os_elapsed) {
            cpu_freq = os_freq * cpu_elapsed / os_elapsed;
        }
    }

    return cpu_freq;
}

u64 read_os_timer() {
    LARGE_INTEGER value;
    QueryPerformanceCounter(&value);
    return value.QuadPart;
}

u64 get_os_timer_freq() {
    LARGE_INTEGER value;
    QueryPerformanceFrequency(&value);
    return value.QuadPart;
}
#endif
