#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>

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
                tester->target_processed_byte_count = file_size;
            } else {
                test_error(tester, "fread failed");
            }

            fclose(file);
        } else {
            test_error(tester, "fopen failed");
        }
    }
}

void mmap_test(Repetition_Tester *tester, Read_Parameters *parameters) {
    while (is_testing(tester)) {
        s32 fd = open(parameters->name, O_RDONLY);
        struct stat st;

        if (fstat(fd, &st) == 0) {
            off_t size = st.st_size;

            start_time(tester);
            u8 *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
            memcpy(parameters->dest.data, data, size);
            end_time(tester);

            if (munmap(data, size) == -1) {
                fprintf(stderr, "Munmap failed.");
            }

            close(fd);

            count_bytes(tester, size);
            tester->target_processed_byte_count = size;
        } else {
            test_error(tester, "Failed to open file for mmap test.");
        }
    }
}
