#pragma once

#include "util.h"
#include "buffer.h"

typedef enum {
    TestMode_uninitialized,
    TestMode_testing,
    TestMode_completed,
    TestMode_error
} Test_Mode;

typedef struct {
    u64 test_count;
    u64 total_time;
    u64 max_time;
    u64 min_time;
} Repetition_Test_Results;

typedef struct {
    Buffer dest;
    char *name;
} Read_Parameters;

typedef struct {
    Test_Mode mode;

    u64 target_processed_byte_count;
    u64 cpu_freq;
    u64 try_for_time;
    u32 open_block_count;
    u32 close_block_count;

    u64 time_accumulated_on_test;
    u64 bytes_accumulated_on_test;

    u64 tests_started_at;
    Repetition_Test_Results results;
} Repetition_Tester;

typedef void (*Read_Test_Function)(Repetition_Tester*, Read_Parameters*);

typedef struct {
    char *name;
    Read_Test_Function function;
} Read_Test;

void new_test_wave(Repetition_Tester *tester, Read_Parameters *parameters, f64 seconds_to_try);
b32 is_testing(Repetition_Tester *tester);
void count_bytes(Repetition_Tester *tester, u64 bytes);
void start_time(Repetition_Tester *tester);
void end_time(Repetition_Tester *tester);
void test_error(Repetition_Tester *tester, char const *message);
