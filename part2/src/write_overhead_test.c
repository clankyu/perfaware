#include "write_overhead_test.h"

void write_to_all_bytes_test(Repetition_Tester *tester, Read_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        start_time(tester);
        for (u32 i = 0; i < dest_buffer.count; ++i) {
            dest_buffer.data[i] = (u8)i;
        }
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void write_to_all_bytes_backwards_test(Repetition_Tester *tester, Read_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        start_time(tester);
        for (u32 i = 0; i < dest_buffer.count; ++i) {
            dest_buffer.data[dest_buffer.count - i - 1] = (u8)i;
        }
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}
