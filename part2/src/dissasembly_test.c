#include "util.h"
#include <x86intrin.h>
#include <time.h>

void QueryPerformanceCounter(u64 *result){
    //ignore bullshit
    struct timespec ts;
    *result = (u64)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

u64 get_os_timer_freq() {
    return 1000000000;
}

int main(void) {
    u64 performance_counter;
    QueryPerformanceCounter(&performance_counter);

    return 0;
}
