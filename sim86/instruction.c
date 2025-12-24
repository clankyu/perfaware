#include "instruction.h"
#include <stdint.h>

Instruction_Operand general_register_operand(Instruction_Params params, bool uses_rm) {
    Instruction_Operand result = {};
    result.type = Operand_Register;
    result.reg.type = Register_General;
    if (!uses_rm) {
        result.reg.w = params.w;
        result.reg.reg_rm = params.reg;
    } else {
        result.reg.w = params.w;
        result.reg.reg_rm = params.rm;
    }

    return result;
}

Instruction_Operand segment_register_operand(uint8_t sr) {
    Instruction_Operand result = {};
    result.type = Operand_Register;
    result.reg.type = Register_Segment;
    result.reg.sr = sr;

    return result;
}

Instruction_Operand effective_address_operand(Instruction_Params params, uint16_t displacement) {
    Instruction_Operand result = {};
    result.type = Operand_Memory;
    result.address.displacement = displacement;
    result.address.mod = params.mod;
    result.address.rm = params.rm;

    return result;
}

Instruction_Operand immediate_operand(uint16_t value) {
    Instruction_Operand result = {};
    result.type = Operand_Immediate;
    result.immediate.val = value;

    return result;
}

Instruction_Operand none_operand() {
    Instruction_Operand result = {};
    result.type = Operand_None;

    return result;
}
