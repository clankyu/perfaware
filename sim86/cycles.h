#pragma once

#include <stdint.h>

#include "instruction.h"

typedef struct {
    uint16_t base;
    uint16_t ea;
} Estimated_Cycles;

typedef enum {
    FORM_R_R,
    FORM_R_M,
    FORM_M_R,
    FORM_IMMD,
    FORM_M_IMMD,
    FORM_ACCUM_IMMD
} Instruction_Form;

static uint8_t ea_components[7];

Estimated_Cycles calculate_instruction_estimated_cycles(Instruction instruction);
uint16_t calculate_base_estimated_cyles(Instruction instruction);
uint16_t calculate_ea(Effective_Address_Expression ea);

Instruction_Form classify_form(Instruction instruction);
