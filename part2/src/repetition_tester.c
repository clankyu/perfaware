#include <stdio.h>
#include "repetition_tester.h"
#include "os_performance_metrics.h"
#include "util.h"

void new_test_wave(Repetition_Tester *tester, Read_Parameters *parameters, f64 seconds_to_try) {
    if (tester->mode == TestMode_uninitialized) {
        *tester = (Repetition_Tester){0};
        tester->cpu_freq = get_cpu_freq_fast();

        if (parameters->name) {
            parameters->dest = buffer_from_file(parameters->name);
            tester->target_processed_byte_count = parameters->dest.count;
        } else {
            test_error(tester, "Missing file name");
        }

        tester->mode = TestMode_testing;
        tester->tests_started_at = read_cpu_timer();
    } else if (tester->mode == TestMode_completed) {
        tester->mode = TestMode_testing;
        printf("test completed\n");

        if (tester->target_processed_byte_count != tester->bytes_accumulated_on_test) {
            test_error(tester, "Target processed byte count does not match bytes accumulated on test.");
        }
    }

    tester->try_for_time = seconds_to_try * tester->cpu_freq;
    tester->tests_started_at = read_cpu_timer();
}

b32 is_testing(Repetition_Tester *tester) {
    if (tester->mode == TestMode_testing) {
        u64 current_time = read_cpu_timer();

        if (tester->open_block_count) {
            if (tester->open_block_count != tester->close_block_count) {
                test_error(tester, "Open and close block count don't match.");
            }
            if (tester->bytes_accumulated_on_test != tester->target_processed_byte_count) {
                printf("bytes accumulated on test: %lu\ntarget processed byte count: %lu\n", tester->bytes_accumulated_on_test, tester->target_processed_byte_count);
                test_error(tester, "Bytes accumulated on test don't match target processed byte count.");
            }

            if (tester->mode == TestMode_testing) {
                Repetition_Test_Results *results = &tester->results;
                ++results->test_count;

                u64 elapsed_time = tester->time_accumulated_on_test;
                results->total_time += elapsed_time;

                if (results->max_time < elapsed_time) {
                    results->max_time = elapsed_time;
                }

                if (results->min_time > elapsed_time) {
                    results->min_time = elapsed_time;

                    tester->tests_started_at = current_time;

                    print_time("Min", results->min_time, tester->cpu_freq, tester->target_processed_byte_count);

                }

                tester->open_block_count = 0;
                tester->close_block_count = 0;
                tester->time_accumulated_on_test = 0;
                tester->bytes_accumulated_on_test = 0;
            }

            if (current_time - tester->tests_started_at > tester->try_for_time) {
                tester->mode = TestMode_completed;
                print_results(tester->results, tester->cpu_freq, tester->target_processed_byte_count);
            }
        } else {
            Repetition_Test_Results *results = &tester->results;

            results->min_time = tester->time_accumulated_on_test;
            tester->tests_started_at = current_time;
            print_time("Min", results->min_time, tester->cpu_freq, tester->target_processed_byte_count);
        }
    }

    b32 result = (tester->mode == TestMode_testing) ? true : false;
    return result;
}

void test_error(Repetition_Tester *tester, char const *message) {
    fprintf(stderr, "Error while performing read test. %s\n", message);
    tester->mode = TestMode_error;
}

inline void count_bytes(Repetition_Tester *tester, u64 bytes) {
    tester->bytes_accumulated_on_test += bytes;
}

inline void start_time(Repetition_Tester *tester) {
    ++tester->open_block_count;
    tester->time_accumulated_on_test -= read_cpu_timer();
}

inline void end_time(Repetition_Tester *tester) {
    ++tester->close_block_count;
    tester->time_accumulated_on_test += read_cpu_timer();
}

void print_time(char const *label, u64 total_cpu, u64 cpu_freq, u64 bytes_processed) {
    f64 seconds = (f64)total_cpu / (f64)cpu_freq;
    f64 gigabyte = 1024.0f*1024.0f*1024.0f;
    f64 throughput = (f64)(bytes_processed/seconds) / gigabyte;

    printf("%s: (%.4f ms), %.4f gb/s\n", label, seconds*1000.0f, throughput);
}

void print_results(Repetition_Test_Results results, u64 cpu_freq, u64 processed_byte_count) {
    print_time("Min", (f64)results.min_time, cpu_freq, processed_byte_count);
    printf("\n");

    print_time("Max", (f64)results.max_time, cpu_freq, processed_byte_count);
    printf("\n");

    if(results.test_count)
    {
        print_time("Avg", (f64)results.total_time / (f64)results.test_count, cpu_freq, processed_byte_count);
        printf("\n");
    }
}

