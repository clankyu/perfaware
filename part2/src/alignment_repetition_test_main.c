#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "repetition_tester.h"
#include "buffer.h"
#include "util.h"
#include "os_performance_metrics.h"

#define READ_FILE_MAX_SIZE 1500000000

extern void read_256x8_alignment(u64 count, u8 *data, u64 alignment_padding);
#pragma comment (lib, "nop_loop")

int main(int argc, char **argv) {
    initialize_os_metrics();

    f64 seconds_to_try = 10.0f;
    u64 max_alignment_padding = 64;

    if (argc > 1) {
        seconds_to_try = (f64)atof(argv[1]);
        printf("seconds to try: %lf\n", seconds_to_try);

        if (argc > 2) {
            max_alignment_padding = (u64)atoi(argv[2]);
            printf("max alignment padding: %llu\n", max_alignment_padding);
        }

        Buffer dest = allocate_buffer(1024*1024*1024);
        Test_Parameters parameters = {0};
        parameters.dest = dest;
        parameters.allocation_type = AllocType_none;
        parameters.branch_pattern = Branch_Pattern_none;
        parameters.name = "Alignment read test";

        for (u64 byte_index = 0; byte_index < dest.count; ++byte_index) {
            dest.data[byte_index] = (u8)byte_index;
        }

        Repetition_Tester testers[64] = {0};
        for (;;) {
            for (u64 alignment_padding = 0; alignment_padding < max_alignment_padding; ++alignment_padding) {
                Repetition_Tester *tester = testers + alignment_padding;
                tester->results = (Repetition_Test_Results){0};

                char function_name[512];
                snprintf(function_name, 512, "read_256x8 %llu alignment padding", alignment_padding);

                printf("\n--- %s%s%s ---\n", get_allocation_str(parameters.allocation_type), parameters.allocation_type ? " + " : "", function_name);
                new_test_wave(tester, &parameters, seconds_to_try);

                while(is_testing(tester)) {
                    Buffer dest_buffer = parameters.dest;

                    start_time(tester);
                    // subtracting 256 just to save myself the trouble of accessing accidentally outer bounds of buffer
                    read_256x8_alignment(dest_buffer.count - 256, dest_buffer.data, alignment_padding);
                    end_time(tester);
                    count_bytes(tester, dest_buffer.count);
                }
                if (tester->mode == TestMode_error) {
                    goto end;
                }
            }

            free_buffer(&dest);
        }
    }

end:
    return 0;
}
