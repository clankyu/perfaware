#pragma once

#include "util.h"
#include "buffer.h"

u64 get_cpu_freq_fast();
u64 get_cpu_freq();
u64 read_cpu_timer();
u64 get_os_timer_freq();
u64 read_os_timer();
