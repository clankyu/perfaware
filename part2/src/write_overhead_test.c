#include "write_overhead_test.h"

void write_to_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        start_time(tester);
        for (u64 i = 0; i < dest_buffer.count; ++i) {
            dest_buffer.data[i] = (u8)i;
        }
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void write_to_all_bytes_backwards_test(Repetition_Tester *tester, Test_Parameters *parameters) {
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

void mov_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        start_time(tester);
        mov_all_bytes_asm(dest_buffer.count, dest_buffer.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void nop_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        start_time(tester);
        nop_all_bytes_asm(dest_buffer.count);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void cmp_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        start_time(tester);
        cmp_all_bytes_asm(dest_buffer.count);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void dec_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        start_time(tester);
        dec_all_bytes_asm(dest_buffer.count);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void nop_1x1_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        start_time(tester);
        nop_1x1_all_bytes_asm(dest_buffer.count);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void nop_1x3_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        start_time(tester);
        nop_1x3_all_bytes_asm(dest_buffer.count);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void nop_1x9_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        start_time(tester);
        nop_1x9_all_bytes_asm(dest_buffer.count);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void conditional_nop_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        conditional_nop_asm(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}
