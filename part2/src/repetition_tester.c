#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "repetition_tester.h"
#include "os_performance_metrics.h"
#include "util.h"

#ifdef __linux__
#include <sys/mman.h>
#include <unistd.h>
#endif

void new_test_wave(Repetition_Tester *tester, Test_Parameters *parameters, f64 seconds_to_try) {
    if (tester->mode == TestMode_uninitialized) {
        *tester = (Repetition_Tester){0};
        tester->cpu_freq = get_cpu_freq();

        if (parameters->name) {
            tester->target_processed_byte_count = parameters->dest.count;
        } else {
            test_error(tester, "Missing file name");
        }

        tester->mode = TestMode_testing;
        tester->tests_started_at = read_cpu_timer();
    } else if (tester->mode == TestMode_completed) {
        tester->mode = TestMode_testing;

        /*
        if (tester->target_processed_byte_count != tester->bytes_accumulated_on_test) {
            printf("target processed byte count: %lu\nbytes accumulated on test: %lu\n", tester->target_processed_byte_count, tester->bytes_accumulated_on_test);
            test_error(tester, "Target processed byte count does not match bytes accumulated on test.");
        }
        */
    }

    tester->try_for_time = seconds_to_try * tester->cpu_freq;
    //tester->try_for_time = seconds_to_try * tester->cpu_freq;
    tester->tests_started_at = read_cpu_timer();
}

b32 is_testing(Repetition_Tester *tester) {
    if (tester->mode == TestMode_testing) {
        Repetition_Test_Values accum = tester->values_on_this_test;
        u64 current_time = read_cpu_timer();

        if (tester->open_block_count) {
            if (tester->open_block_count != tester->close_block_count) {
                test_error(tester, "Open and close block count don't match.");
            }
            if (accum.byte_count != tester->target_processed_byte_count) {
                printf("bytes accumulated on test: %lu\ntarget processed byte count: %lu\n", tester->results.total.byte_count, tester->target_processed_byte_count);
                test_error(tester, "Bytes accumulated on test don't match target processed byte count.");
            }

            if (tester->mode == TestMode_testing) {
                Repetition_Test_Results *results = &tester->results;
                ++accum.test_count;

                u64 elapsed_time = accum.total_time;
                results->total.total_time += elapsed_time;
                results->total.byte_count += accum.byte_count;
                results->total.mem_pagefault_count += accum.mem_pagefault_count;
                ++results->total.test_count;

                if (results->max.total_time < elapsed_time) {
                    results->max = accum;
                }

                if (results->total.test_count == 1) {
                    results->min = accum;
                    tester->tests_started_at = current_time;

                    printf("\r\033[K");
                    print_values("Min", tester->cpu_freq, results->min);
                }

                if (results->min.total_time > elapsed_time) {
                    results->min = accum;
                    tester->tests_started_at = current_time;

                    printf("\r\033[K");
                    print_values("Min", tester->cpu_freq, results->min);
                }

                tester->open_block_count = 0;
                tester->close_block_count = 0;
                tester->values_on_this_test = (Repetition_Test_Values){0};
            }

        }

        if ((current_time - tester->tests_started_at) > tester->try_for_time) {
            tester->mode = TestMode_completed;
            print_results(tester->results, tester->cpu_freq, tester->target_processed_byte_count);
            printf("\r\033[K");
        }
    }

    b32 result = (tester->mode == TestMode_testing);
    return result;
}

void test_error(Repetition_Tester *tester, char const *message) {
    fprintf(stderr, "Error while performing read test. %s\n", message);
    tester->mode = TestMode_error;
}

inline void count_bytes(Repetition_Tester *tester, u64 bytes) {
    tester->values_on_this_test.byte_count += bytes;
}

inline void start_time(Repetition_Tester *tester) {
    ++tester->open_block_count;
    tester->values_on_this_test.total_time -= read_cpu_timer();
    tester->values_on_this_test.mem_pagefault_count -= get_os_minor_page_faults();
}

inline void end_time(Repetition_Tester *tester) {
    ++tester->close_block_count;
    tester->values_on_this_test.total_time += read_cpu_timer();
    tester->values_on_this_test.mem_pagefault_count += get_os_minor_page_faults();
}

void print_values(char const *label, u64 cpu_freq, Repetition_Test_Values values) {
    f64 seconds = (f64)values.total_time / (f64)cpu_freq;
    f64 gigabyte = 1024.0f*1024.0f*1024.0f;
    f64 throughput = (f64)(values.byte_count/seconds) / gigabyte;

    printf("%s: (%.4f ms), %.4f gb/s", label, seconds*1000.0f, throughput);
    if(values.mem_pagefault_count > 0) {
        printf(" PF: %0.4f (%0.4fk/fault)", (f64)values.mem_pagefault_count, values.byte_count / (values.mem_pagefault_count * 1024.0));
    } else {
        printf(" NO PF");
    }
    fflush(stdout);
}

void print_results(Repetition_Test_Results results, u64 cpu_freq, u64 processed_byte_count) {
    printf("\r\033[K");
    print_values("Min", cpu_freq, results.min);
    printf("\n");

    print_values("Max", cpu_freq, results.max);
    printf("\n");

    if(results.total.test_count)
    {
        Repetition_Test_Values average = {0};
        average.total_time = (f64) results.total.total_time / (f64) results.total.test_count;
        average.byte_count = results.total.byte_count / results.total.test_count;
        average.mem_pagefault_count = results.total.mem_pagefault_count / results.total.test_count;

        print_values("Avg", cpu_freq, average);
        printf("\n");
    }
}

void handle_allocation(Test_Parameters *parameters, Buffer *buffer) {
    switch (parameters->allocation_type) {
        case AllocType_none: {} break;
        case AllocType_malloc: {
            *buffer = allocate_buffer(parameters->dest.count);
        } break;
        #ifdef __linux__
        case AllocType_mmap: {
            buffer->data = mmap(NULL, parameters->dest.count, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            buffer->count = parameters->dest.count;

            if (buffer->data == MAP_FAILED) {
                fprintf(stderr, "Failed to allocate data via mmap.\n");
            }
        } break;
        /*
        case AllocType_mmap_large_pages: {
            u32 missing_2mb_alignment = parameters->dest.count % (1024*1024);
            u32 pages_in_allocation = parameters->dest.count / (1024*1024);
            u32 aligned_size = (missing_2mb_alignment != 0) ? (1024*1024)*(pages_in_allocation + 1) : (1024*1024)*pages_in_allocation;
            buffer->data = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
            buffer->count = parameters->dest.count;

            if (buffer->data == MAP_FAILED) {
                fprintf(stderr, "Failed to allocate data via mmap.\n");
            }
        } break;
        */
        #endif
        default: {
            fprintf(stderr, "Error. Unrecognized alloc type: %u.\n", parameters->allocation_type);
        }
    }
}

void handle_deallocation(Test_Parameters *parameters, Buffer *buffer) {
    switch (parameters->allocation_type) {
        case AllocType_none: {} break;
        case AllocType_malloc: {
            free_buffer(buffer);
        } break;
        #ifdef __linux__
        case AllocType_mmap: {
            munmap(buffer->data, buffer->count);
        } break;
        /*
        case AllocType_mmap_large_pages: {
            munmap(buffer->data, buffer->count);
        } break;
        */
        #endif
        default: {
            fprintf(stderr, "Error. Unrecognized alloc type: %u.\n", parameters->allocation_type);
        }
    }
}

char const *get_allocation_str(Allocation_Type alloc_type) {
    char const *result;
    switch (alloc_type) {
        case AllocType_none: { result = ""; } break;
        case AllocType_malloc: { result = "malloc"; } break;
        #ifdef __linux__
        case AllocType_mmap: {result = "mmap"; } break;
        //case AllocType_mmap_large_pages: {result = "mmap (large pages)"; } break;
        #endif
        default: { result = "unknown"; }
    }

    return result;
}

char const *get_branch_pattern_str(Branch_Pattern pattern) {
    char const *result;

    switch (pattern) {
        case Branch_Pattern_never_taken: { result = "never taken"; } break;
        case Branch_Pattern_always_taken: { result = "always taken"; } break;
        case Branch_Pattern_every2: { result = "every2"; } break;
        case Branch_Pattern_every3: { result = "every3"; } break;
        case Branch_Pattern_every4: { result = "every4"; } break;
        case Branch_Pattern_CRT_random: { result = "CRT random"; } break;
        case Branch_Pattern_OS_random: { result = "OS random"; } break;
        default: { result = "Unimplemented branch pattern"; } break;
    }

    return result;
}

void fill_with_branch_pattern(Buffer *dest, Branch_Pattern pattern) {
    if (pattern == Branch_Pattern_OS_random) {
        b32 success = fill_buffer_random_os(dest);
        if (success == false) {
            fprintf(stderr, "Failed to generate buffer with random os generator. Default value = 0\n");
            memset(dest->data, 0, dest->count);
        }
    } else {
        for (u64 buffer_index = 0; buffer_index < dest->count; ++buffer_index) {
            u8 value = 0;

            switch (pattern) {
                case Branch_Pattern_never_taken: {
                    value = 0;
                } break;
                case Branch_Pattern_always_taken: {
                    value = 1;
                } break;
                case Branch_Pattern_every2: {
                    value = (buffer_index % 2 == 0) ? 1 : 0;
                } break;
                case Branch_Pattern_every3: {
                    value = (buffer_index % 3 == 0) ? 1 : 0;
                } break;
                case Branch_Pattern_every4: {
                    value = (buffer_index % 4 == 0) ? 1 : 0;
                } break;
                case Branch_Pattern_CRT_random: {
                    value = (u8)rand();
                } break;
                case Branch_Pattern_none: {
                    break;
                } break;
                default: {
                    fprintf(stderr, "Branch pattern not implemented: %u\n", pattern);
                    value = 0;
                } break;
            }

            dest->data[buffer_index] = value;
        }
    }
}

#ifdef _WIN32

#include <windows.h>
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

b32 fill_buffer_random_os(Buffer *dest) {
    b32 result;

    if (dest->count) {
        result = BCryptGenRandom(0, (BYTE *)dest->data, (u32)dest->count, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0;
    }

    return result;
}
#endif
