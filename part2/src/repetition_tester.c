#include <stdio.h>
#include "repetition_tester.h"
#include "os_performance_metrics.h"
#include "util.h"

void new_test_wave(Repetition_Tester *tester, Read_Parameters *parameters, f64 seconds_to_try) {
    if (tester->mode == TestMode_uninitialized) {
        *tester = (Repetition_Tester){0}; // this might bite me in the ass
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

        if (tester->target_processed_byte_count != tester->bytes_accumulated_on_test) {
            test_error(tester, "Target processed byte count does not match bytes accumulated on test.");
        }
    }

    tester->try_for_time = seconds_to_try * tester->cpu_freq;
    tester->tests_started_at = read_cpu_timer();
}

b32 is_testing(Repetition_Tester *tester) {
    if (tester->mode == TestMode_testing) {
        if (tester->open_block_count != tester->close_block_count) {

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
