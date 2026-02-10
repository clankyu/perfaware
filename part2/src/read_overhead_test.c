#include <stdio.h>
#include <stdint.h>

#include "repetition_tester.h"
#include "read_overhead_test.h"

void fread_test(Repetition_Tester *tester, Read_Parameters *parameters) {
    while (is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;
        FILE *file = fopen(parameters->name, "rb");

        if (file) {
            u32 file_size = 0;
            fseek(file, 0L, SEEK_END);
            file_size = ftell(file);
            rewind(file);
            dest_buffer.count = file_size;

            start_time(tester);
            size_t result = fread(dest_buffer.data, dest_buffer.count, 1, file);
            end_time(tester);

            if (result == 1) {
                count_bytes(tester, dest_buffer.count);
            } else {
                test_error(tester, "fread failed");
            }

            fclose(file);
        } else {
            test_error(tester, "fopen failed");
        }
    }
}
