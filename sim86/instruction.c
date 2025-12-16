#include "instruction.h"

Instruction_Operand register_operand(Instruction_Params params) {
    Instruction_Operand result = {};
    result.type = Operand_Register;
    result.reg.w = params.w;
    result.reg.reg = params.reg;

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

Instruction_Operand immediate_operand(int16_t value) {
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
