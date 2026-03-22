#pragma once

#include "repetition_tester.h"

void write_to_all_bytes_test(Repetition_Tester *tester, Read_Parameters *parameters);
void write_to_all_bytes_backwards_test(Repetition_Tester *tester, Read_Parameters *parameters);

void mov_all_bytes_test(Repetition_Tester *tester, Read_Parameters *parameters);
void nop_all_bytes_test(Repetition_Tester *tester, Read_Parameters *parameters);
void cmp_all_bytes_test(Repetition_Tester *tester, Read_Parameters *parameters);
void dec_all_bytes_test(Repetition_Tester *tester, Read_Parameters *parameters);

extern void mov_all_bytes_asm(u64 count, u8 *data);
extern void nop_all_bytes_asm(u64 count);
extern void cmp_all_bytes_asm(u64 count);
extern void dec_all_bytes_asm(u64 count);
#pragma comment (lib, "nop_loop")

