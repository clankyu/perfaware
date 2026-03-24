#include "buffer.h"
#include "os_performance_metrics.h"
#include "repetition_tester.h"
#include "util.h"
#include "write_overhead_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define BUFFER_MAX_SIZE 1500000000

int main(int argc, char **argv) {
    initialize_os_metrics();

    u32 seconds_to_try = 10;
    u32 buffer_size = 0;
    Branch_Pattern branch_pattern = Branch_Pattern_never_taken;

    if (argc > 1) {
        buffer_size = (u32)atoi(argv[1]) * 1024 * 1024 * 1024;

        if (argc > 2) {
            seconds_to_try = (u32)atoi(argv[2]);
        }

        Repetition_Test functions[] = {
            (Repetition_Test){.name = "conditional_nop",
                .function = conditional_nop_test},
        };

        if (buffer_size) {
            Test_Parameters test_parameters = {0};
            test_parameters.dest = allocate_buffer(buffer_size);
            test_parameters.name = argv[1];

            Repetition_Tester testers[array_count(functions)] = {0};
            for (;;) {
                for (u32 function_index = 0; function_index < array_count(functions); ++function_index) {
                    for (u32 branch_pattern = 0; branch_pattern < Branch_Pattern_count; ++branch_pattern) {
                        Repetition_Tester *tester = testers + function_index;
                        tester->results = (Repetition_Test_Results){0};
                        Repetition_Test test_function = functions[function_index];

                        test_parameters.branch_pattern = branch_pattern;
                        test_parameters.allocation_type = AllocType_none;

                        printf("\n--- %s%s%s ---\n", get_branch_pattern_str(branch_pattern),
                               test_parameters.branch_pattern ? " + " : "",
                               test_function.name);
                        new_test_wave(tester, &test_parameters, seconds_to_try);
                        test_function.function(tester, &test_parameters);
                        if (tester->mode == TestMode_error) {
                            goto end;
                        }
                    }
                }
            }

            free_buffer(&test_parameters.dest);
        } else {
            fprintf(stderr, "Unable to retrieve buffer size.");
        }
    } else {
        fprintf(stderr, "Not enough arguments. Buffer size needed.");
    }

end:
    return 0;
}
