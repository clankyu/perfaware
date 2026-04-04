#pragma once

#include "repetition_tester.h"

void write_to_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters);
void write_to_all_bytes_backwards_test(Repetition_Tester *tester, Test_Parameters *parameters);

void mov_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters);
void nop_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters);
void cmp_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters);
void dec_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters);
void nop_1x1_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters);
void nop_1x3_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters);
void nop_1x9_all_bytes_test(Repetition_Tester *tester, Test_Parameters *parameters);
void conditional_nop_test(Repetition_Tester *tester, Test_Parameters *parameters);
void nop_all_bytes_unaligned_63_test(Repetition_Tester *tester, Test_Parameters *parameters);
void Read_x1_test(Repetition_Tester *tester, Test_Parameters *parameters);
void Read_x1_backwards_test(Repetition_Tester *tester, Test_Parameters *parameters);
void Read_x2_test(Repetition_Tester *tester, Test_Parameters *parameters);
void Read_x3_test(Repetition_Tester *tester, Test_Parameters *parameters);
void Read_x4_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_4x3_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_8x3_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_16x3_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_32x3_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_32x2_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_32x4_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_32x1_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_128_12_bits_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_128_14_bits_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_128_15_bits_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_128_16_bits_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_128_18_bits_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_128_19_bits_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_128_20_bits_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_128_22_bits_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_128_24_bits_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_128_25_bits_test(Repetition_Tester *tester, Test_Parameters *parameters);
void read_128_26_bits_test(Repetition_Tester *tester, Test_Parameters *parameters);

extern void mov_all_bytes_asm(u64 count, u8 *data);
extern void nop_all_bytes_asm(u64 count);
extern void cmp_all_bytes_asm(u64 count);
extern void dec_all_bytes_asm(u64 count);
extern void nop_1x1_all_bytes_asm(u64 count);
extern void nop_1x3_all_bytes_asm(u64 count);
extern void nop_1x9_all_bytes_asm(u64 count);
extern void conditional_nop_asm(u64 count, u8 *data);
extern void nop_all_bytes_unaligned_63_asm(u64 count);
extern void Read_x1_asm(u64 count, u8 *data);
extern void Read_x1_backwards_asm(u64 count, u8 *data);
extern void Read_x2_asm(u64 count, u8 *data);
extern void Read_x3_asm(u64 count, u8 *data);
extern void Read_x4_asm(u64 count, u8 *data);
extern void read_4x3(u64 count, u8 *data);
extern void read_8x3(u64 count, u8 *data);
extern void read_16x3(u64 count, u8 *data);
extern void read_32x3(u64 count, u8 *data);
extern void read_32x2(u64 count, u8 *data);
extern void read_32x4(u64 count, u8 *data);
extern void read_32x1(u64 count, u8 *data);
extern void read_128_x_bits(u64 count, u8 *data);
#pragma comment (lib, "nop_loop")

