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

extern void mov_all_bytes_asm(u64 count, u8 *data);
extern void nop_all_bytes_asm(u64 count);
extern void cmp_all_bytes_asm(u64 count);
extern void dec_all_bytes_asm(u64 count);
extern void nop_1x1_all_bytes_asm(u64 count);
extern void nop_1x3_all_bytes_asm(u64 count);
extern void nop_1x9_all_bytes_asm(u64 count);
extern void conditional_nop_asm(u64 count, u8 *data);
#pragma comment (lib, "nop_loop")

