#include <stdio.h>
#include <stdlib.h>
#include "repetition_tester.h"
#include "read_overhead_test.h"
#include "util.h"

#define READ_FILE_MAX_SIZE 1500000000

int main(int argc, char **argv) {
    u32 seconds_to_try = 10;

    if (argc > 1) {
        printf("arg[1]: %s\n", argv[1]);
        if (argc > 2) {
            seconds_to_try = (u32)atoi(argv[2]);
            printf("seconds to try: %u\n", seconds_to_try);
        }

        Read_Test functions[] = {
            (Read_Test) { .name = "fread", .function = fread_test },
        };

        Read_Parameters read_parameters = {0};
        read_parameters.dest = allocate_buffer(READ_FILE_MAX_SIZE);
        read_parameters.name = argv[1];

        Repetition_Tester testers[array_count(functions)] = {0};
        for (u32 function_index = 0; function_index < array_count(functions); ++function_index) {
            Repetition_Tester *tester = testers + function_index;
            Read_Test test_function = functions[function_index];

            printf("\n--- %s ---\n", test_function.name);
            new_test_wave(tester, &read_parameters, seconds_to_try);
            test_function.function(tester, &read_parameters);
            if (tester->mode == TestMode_error) {
                goto end;
            }
        }
    } else {
        fprintf(stderr, "Not enough arguments. File name needed.");
    }

end:
    return 0;
}
