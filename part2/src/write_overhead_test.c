#include <stdio.h>
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

void nop_all_bytes_unaligned_63_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        nop_all_bytes_unaligned_63_asm(parameters->dest.count);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void Read_x1_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        Read_x1_asm(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void Read_x1_backwards_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        Read_x1_backwards_asm(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void Read_x2_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        Read_x2_asm(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void Read_x3_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        Read_x3_asm(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void Read_x4_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        Read_x4_asm(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_4x3_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        read_4x3(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_8x3_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        read_8x3(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_16x3_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        read_16x3(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_32x3_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        read_32x3(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_32x2_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        read_32x2(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_32x4_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        read_32x4(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_32x1_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        read_32x1(parameters->dest.count, parameters->dest.data);
        end_time(tester);
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_12_bits_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits(rep_count, parameters->dest.data, 0xFFF / 256);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0xFFF * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_14_bits_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits(rep_count, parameters->dest.data, 0x4000 / 256);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0x4000 * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_15_bits_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits(rep_count, parameters->dest.data, 0x8000 / 256);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0x8000 * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_16_bits_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits(rep_count, parameters->dest.data, 0x10000);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0x10000 * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_18_bits_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits(rep_count, parameters->dest.data, 0x40000);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0x40000 * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_19_bits_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits(rep_count, parameters->dest.data, 0x80000 / 256);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0x80000 * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_20_bits_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits(rep_count, parameters->dest.data, 0x100000 / 256);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0x100000 * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_22_bits_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits(rep_count, parameters->dest.data, 0x400000 / 256);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0x400000 * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_24_bits_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits(rep_count, parameters->dest.data, 0x1000000 / 256);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0x1000000 * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_25_bits_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits(rep_count, parameters->dest.data, 0x2000000 / 256);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0x2000000 * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_26_bits_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits(rep_count, parameters->dest.data, 0x4000000 / 256);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0x4000000 * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}

void read_128_12_bits_unrolled_test(Repetition_Tester *tester, Test_Parameters *parameters) {
    while(is_testing(tester)) {
        Buffer dest_buffer = parameters->dest;

        handle_allocation(parameters, &dest_buffer);
        fill_with_branch_pattern(&dest_buffer, parameters->branch_pattern);
        start_time(tester);
        u64 stride_size = 262144;
        u64 rep_count = parameters->dest.count / stride_size;
        read_128_x_bits_unrolled(rep_count, parameters->dest.data, 0xFFF / 256);
        u64 leftover_count = parameters->dest.count - (rep_count * stride_size);
        read_leftover_bits(leftover_count, parameters->dest.data + parameters->dest.count - leftover_count);
        end_time(tester);
        u64 total_bytes = rep_count * 0xFFF * 256;
        count_bytes(tester, dest_buffer.count);
        handle_deallocation(parameters, &dest_buffer);
    }
}
