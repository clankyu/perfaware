#include <stdio.h>
#include "util.h"
#include "os_performance_metrics.h"

int main(void) {
    u64 os_freq = get_os_timer_freq();
    printf("cpu frequency: %lu\n", os_freq);

    u64 cpu_start = read_cpu_timer();
    u64 os_start = read_os_timer();
    u64 os_end = 0;
    u64 os_elapsed = 0;
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

    printf("os timer: %lu -> %lu = %lu elapsed\n", os_start, os_end, os_elapsed);
    printf("os seconds: %.4f\n", (f64)os_elapsed / (f64)os_freq);
    printf("cpu timer: %lu -> %lu = %lu elapsed\n", cpu_start, cpu_end, cpu_elapsed);
    printf("cpu frequency: %lu\n", cpu_freq);

    u64 real_cpu_freq = get_cpu_freq_fast();
    printf("real cpu frequency hz: %lu\n", real_cpu_freq);

    return 0;
}

