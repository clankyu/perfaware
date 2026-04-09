#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "repetition_tester.h"
#include "buffer.h"
#include "write_overhead_test.h"
#include "util.h"
#include "os_performance_metrics.h"

#define READ_FILE_MAX_SIZE 1500000000

int main(int argc, char **argv) {
    initialize_os_metrics();

    u32 seconds_to_try = 10;
    u64 file_size = 0;
    Allocation_Type allocation_type = AllocType_none;

    if (argc > 1) {
        struct stat st;
        stat(argv[1], &st);
        file_size = st.st_size;

        if (argc > 2) {
            seconds_to_try = (u32)atoi(argv[2]);
        }

        Repetition_Test functions[] = {
            (Repetition_Test) { .name = "read 128 12 bits (4 kib) cache size", .function = read_128_12_bits_test },
            (Repetition_Test) { .name = "read 128 12 bits unrolled (4 kib) cache size", .function = read_128_12_bits_unrolled_test },
        };

        if (file_size) {
            Test_Parameters read_parameters = {0};
            //read_parameters.dest = allocate_buffer(file_size);
            read_parameters.dest = allocate_buffer(1024*1024*1024);
            read_parameters.name = argv[1];

            Repetition_Tester testers[array_count(functions)] = {0};
            for (;;) {
                for (u32 function_index = 0; function_index < array_count(functions); ++function_index) {
                    // remember the - 1 in the condition, just for testing without malloc
                    for (u32 alloc_type = 0; alloc_type < AllocType_count - 1; ++alloc_type) {
                        Repetition_Tester *tester = testers + function_index;
                        tester->results = (Repetition_Test_Results){0};
                        Repetition_Test test_function = functions[function_index];

                        read_parameters.allocation_type = alloc_type;
                        read_parameters.branch_pattern = Branch_Pattern_none;

                        printf("\n--- %s%s%s ---\n", get_allocation_str(alloc_type), read_parameters.allocation_type ? " + " : "", test_function.name);
                        new_test_wave(tester, &read_parameters, seconds_to_try);
                        test_function.function(tester, &read_parameters);
                        if (tester->mode == TestMode_error) {
                            goto end;
                        }
                    }
                }
            }

            free_buffer(&read_parameters.dest);
        } else {
            fprintf(stderr, "Unable to retrieve file size.");
        }
    } else {
        fprintf(stderr, "Not enough arguments. File name needed.");
    }

end:
    return 0;
}
