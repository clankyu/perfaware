#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>
#include <malloc.h>

#ifdef __linux__
#include <unistd.h>
#include <sys/mman.h>
#endif

#include "repetition_tester.h"
#include "read_overhead_test.h"

void fread_test(Repetition_Tester *tester, Read_Parameters *parameters) {
    while (is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;
        FILE *file = fopen(parameters->name, "rb");

        if (file) {
            handle_allocation(parameters, &dest_buffer);
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
            handle_deallocation(parameters, &dest_buffer);
        } else {
            test_error(tester, "fopen failed");
        }
    }
}

#ifdef __linux__
void mmap_test(Repetition_Tester *tester, Read_Parameters *parameters) {
    while (is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;
        s32 fd = open(parameters->name, O_RDONLY);
        struct stat st;

        if (fstat(fd, &st) == 0) {
            handle_allocation(parameters, &dest_buffer);
            off_t size = st.st_size;

            u8 *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
            if (data != MAP_FAILED) {
                start_time(tester);
                memcpy(dest_buffer.data, data, size);
                end_time(tester);
                munmap(data, size);
            } else {
                test_error(tester, "mmap error.");
            }

            handle_deallocation(parameters, &dest_buffer);
            count_bytes(tester, size);
        } else {
            test_error(tester, "Failed to open file for mmap test.");
        }

        close(fd);
    }
}
#endif
