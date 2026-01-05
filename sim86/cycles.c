#include "cycles.h"

static uint8_t ea_components[7] = {
    6, 5, 9, 7, 8, 11, 12
};

// im done with this project im actually putting this to sleep, it's basically done
// i honestly just want to move on with other things i can't bother with this anymore
// this is gonna remain private so it doesn't even matter

Estimated_Cycles calculate_instruction_estimated_cycles(Instruction instruction) {
    Estimated_Cycles ec = {};
    int8_t ea = 0;
    if (instruction.dest.type == Operand_Memory) ea = calculate_ea(instruction.dest.address);
    if (instruction.source.type == Operand_Memory) ea = calculate_ea(instruction.source.address);



    return ec;
}

#define dest(operand_type) (instruction.dest.type == operand_type)
#define source(operand_type) (instruction.source.type == operand_type)
uint16_t calculate_base_estimated_cyles(Instruction instruction) {
    if (instruction.op_type == Op_mov) {
        if (dest(Operand_Register) && source(Operand_Register)) return 3;
        if (dest(Operand_Register) && source(Operand_Memory)) return 9;
        if (dest(Operand_Memory) && source(Operand_Register)) return 16;
        if (dest(Operand_Register) && source(Operand_Immediate)) return 4;
        if (dest(Operand_Memory) && source(Operand_Immediate)) return 17;
        if (dest(Operand_Memory) && source(Operand_Immediate)) return 17;
        if (dest(Operand_Register) && instruction.dest.reg.w && instruction.dest.reg.reg_rm == 0 && source(Operand_Immediate)) return 4;
    }

    return 0;
}
#undef dest
#undef source

uint16_t calculate_ea(Effective_Address_Expression ea) {
    // this is horrible, could probably make some tables to simply index to but whatever
    uint16_t result = 0;
    if (ea.mod == 0b0 && ea.rm == 0b110) return 6;
    if (ea.mod == 0) {
        if (ea.rm >= 0b100 && ea.rm <= 0b111) {
            return 5;
        } else if (ea.rm == 0b0 || ea.rm == 0b011) {
            return 7;
        } else if (ea.rm == 0b001 || ea.rm == 0b010) {
            return 8;
        }
    }
    if (ea.mod == 0b01 || ea.mod == 0b10) {
        if (ea.rm == 0b0 || ea.rm == 0b011) {
            return 11;
        } else if (ea.rm == 0b001 || ea.rm == 0b010) {
            return 12;
        } else if (ea.rm >= 0b100 || ea.rm <= 0b111) {
            return 9;
        }
    }

    return result;
}

/*
Instruction_Form classify_form(Instruction instruction) {
    Instruction_Form form = {};

    form
}
*/
