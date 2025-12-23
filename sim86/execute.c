#include <stdbool.h>
#include <stdio.h>
#include "instruction.h"
#include "memory.h"
#include "text.h"
#include "execute.h"

static Regs_8086 registers_state = {};

#define reg(field) { true, (uint8_t*)&registers_state.field }
#define none { false, 0x0 }
static Effective_Address_Operand effective_address_expr_table[8][2] = {
    { reg(bx), reg(si) },
    { reg(bx), reg(di) },
    { reg(bp), reg(si) },
    { reg(bp), reg(di) },
    { reg(si), none },
    { reg(di), none },
    { reg(bp), none },
    { reg(bx), none },
};
#undef reg
#undef none

void execute_instructions(Seg_Mem *main_memory, Seg_Mem *instructions_mem) {
    uint32_t operand_state_base_ptr = instructions_mem->base + instructions_mem->size;
    const int32_t instruction_count = instructions_mem->size / sizeof(Instruction);

    Instruction *instructions = (void*)access_memory(instructions_mem, 0);
    for (int i = 0; i < instructions_mem->size / sizeof(Instruction); i++) {
        Seg_Mem operand_state_mem = memory_alloc(main_memory, sizeof(Operand_State), operand_state_base_ptr);
        Instruction instruction = instructions[i];
        Operand_State operand_state = execute_instruction(main_memory, &registers_state, instruction);
        Instruction_String_Expression expr = get_instruction_str(&instruction);
        print_instruction_and_operand_state(&instruction, &expr, operand_state);
    }

    printf("\n");
    printf("Final registers:\n");
    print_registers_state();
}

static Operand_State execute_instruction(Seg_Mem *main_memory, Regs_8086 *regs, Instruction instruction) {
    Operand_State state = {};
    state.operand = instruction.dest;
    state.before = get_operand_value(main_memory, instruction.dest);

    switch (instruction.op_type) {
        case Op_mov:
            set_operand_value(main_memory, instruction.dest, get_operand_value(main_memory, instruction.source));
            break;
        case Op_add:
            break;
        case Op_sub:
            break;
        case Op_cmp:
            break;
        case Op_jo:
            break;
        case Op_jno:
            break;
        case Op_jb:
            break;
        case Op_jnb:
            break;
        case Op_je:
            break;
        case Op_jne:
            break;
        case Op_jbe:
            break;
        case Op_ja:
            break;
        case Op_js:
            break;
        case Op_jns:
            break;
        case Op_jp:
            break;
        case Op_jnp:
            break;
        case Op_jl:
            break;
        case Op_jnl:
            break;
        case Op_jle:
            break;
        case Op_jg:
            break;
        case Op_loop:
            break;
        case Op_loopz:
            break;
        case Op_loopnz:
            break;
        case Op_jcxz:
            break;
        case Op_Unimplemented:
            break;
        default:
            printf("UNIMPLEMENTED\n");
    }

    state.after = get_operand_value(main_memory, instruction.dest);
    return state;
}

void op_mov(Seg_Mem *main_memory, Instruction instruction) {

}

static void set_operand_value(Seg_Mem *main_memory, Instruction_Operand operand, uint16_t val) {
    if (operand.type == Operand_Register) {
        Seg_Mem reg_mem = get_register_ref(operand.reg);
        uint16_t value = 0;
        uint8_t *ptr = reg_mem.memory;

        if (reg_mem.size == 1) {
            if (val <= 255) {
                write_u8(ptr, val);
            } else {
                printf("Error: attempting to assign value greater than 8 bits to an 8 bit register, val: %d\n", val);
            }
        } else {
            write_u16(ptr, val);
        }
    } else if (operand.type == Operand_Memory) {
        uint16_t address = calculate_effective_address(operand.address);
        uint8_t *ptr = access_memory(main_memory, address);

        if (operand.address.is_word) {
            write_u16(ptr, val);
        } else {
            if (val <= 255) {
                write_u8(ptr, val);
            } else {
                printf("Error: attempting to assign value greater than 8 bits to an 8 bit register, val: %d\n", val);
            }
        }
    }
}

static uint16_t get_operand_value(Seg_Mem *main_memory, Instruction_Operand operand) {
    Seg_Mem mem = get_operand_ref(main_memory, operand);

    if (mem.size == 1) {
        return read_u8(mem.memory);
    } else {
        return read_u16(mem.memory);
    }

    return 0;
}

static Seg_Mem get_operand_ref(Seg_Mem *main_memory, Instruction_Operand operand) {
    Seg_Mem mem = {};
    if (operand.type == Operand_Register) {
        mem = get_register_ref(operand.reg);
    } else if (operand.type == Operand_Memory) { // gonna have to implement byte and word accesses
        uint16_t address = calculate_effective_address(operand.address);
        mem.memory = access_memory(main_memory, address);
        mem.base = address;
        mem.mask = 0xFF;
        mem.size = 1;
    } else {
        mem.memory = (uint8_t*)&operand.immediate.val;
        mem.base = 0;
        mem.mask = 0;
        if (operand.immediate.val <= 255) {
            mem.size = 1;
        } else {
            mem.size = 2;
        }
    }

    return mem;
}

static Seg_Mem get_register_ref(Reg_Access reg_access) {
    Seg_Mem mem = {};
    if (reg_access.w_mod == 0) {
        mem.memory = &registers_state.reg8[reg_access.reg_rm][reg_access.w_mod];
        mem.base = 0;
        mem.mask = 0xFF;
        mem.size = 1;
    } else {
        mem.memory = (uint8_t*)&registers_state.reg16[reg_access.reg_rm];
        mem.base = 0;
        mem.mask = 0xFF;
        mem.size = 2;
    }

    return mem;
}

uint16_t calculate_effective_address(Effective_Address_Expression expr) {
    uint16_t result = 0;
    result += expr.displacement;
    Effective_Address_Operand *terms = effective_address_expr_table[expr.rm];

    for (int i = 0; i < 2; i++) {
        Effective_Address_Operand term = terms[i];
        if (term.exists) {
            result += *(uint16_t*)term.reg; // surely this won't cause any problems
        }
    }

    return result;
}

#define REG_FMT(reg) #reg ": 0x%X (%i)\n"
#define REG_ARGS(reg) registers_state.reg, registers_state.reg
void print_registers_state() {
    printf(REG_FMT(ax), REG_ARGS(ax));
    printf(REG_FMT(bx), REG_ARGS(bx));
    printf(REG_FMT(cx), REG_ARGS(cx));
    printf(REG_FMT(dx), REG_ARGS(dx));
    printf(REG_FMT(sp), REG_ARGS(sp));
    printf(REG_FMT(bp), REG_ARGS(bp));
    printf(REG_FMT(si), REG_ARGS(si));
    printf(REG_FMT(di), REG_ARGS(di));
}
