#pragma once

#include "util.h"
#include "buffer.h"

typedef enum {
    AllocType_none,
    AllocType_malloc,
    #ifdef __linux__
    AllocType_mmap,
    // AllocType_mmap_large_pages, note: couldn't get large pages working in wsl, not sure if it has anything to do with windows permissions, maybe at a later point
    #endif

    AllocType_count
} Allocation_Type;

typedef enum {
    TestMode_uninitialized,
    TestMode_testing,
    TestMode_completed,
    TestMode_error
} Test_Mode;


typedef struct {
    u64 test_count;
    u64 total_time;
    u64 mem_pagefault_count;
    u64 byte_count;
} Repetition_Test_Values;

typedef struct {
    Repetition_Test_Values min;
    Repetition_Test_Values max;
    Repetition_Test_Values total;
} Repetition_Test_Results;

typedef struct {
    Buffer dest;
    Allocation_Type allocation_type;
    char *name;
} Read_Parameters;

typedef struct {
    Test_Mode mode;

    u64 target_processed_byte_count;
    u64 cpu_freq;
    u64 try_for_time;
    u32 open_block_count;
    u32 close_block_count;

    u64 tests_started_at;
    Repetition_Test_Values values_on_this_test;
    Repetition_Test_Results results;
} Repetition_Tester;

typedef void (*Read_Test_Function)(Repetition_Tester*, Read_Parameters*);

typedef struct {
    char *name;
    Read_Test_Function function;
} Read_Test;

void handle_allocation(Read_Parameters *parameters, Buffer *buffer);
void handle_deallocation(Read_Parameters *parameters, Buffer *buffer);
char const *get_allocation_str(Allocation_Type alloc_type);

void new_test_wave(Repetition_Tester *tester, Read_Parameters *parameters, f64 seconds_to_try);
b32 is_testing(Repetition_Tester *tester);
void count_bytes(Repetition_Tester *tester, u64 bytes);
void start_time(Repetition_Tester *tester);
void end_time(Repetition_Tester *tester);
void test_error(Repetition_Tester *tester, char const *message);
void print_values(char const *label, u64 cpu_freq, Repetition_Test_Values values);
void print_results(Repetition_Test_Results results, u64 cpu_freq, u64 processed_byte_count);
