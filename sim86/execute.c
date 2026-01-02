#include <stdbool.h>
#include <stdio.h>
#include "instruction.h"
#include "memory.h"
#include "text.h"
#include "execute.h"

Regs_8086 registers_state = {};

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

Operand_State execute_instruction(Seg_Mem *main_memory, Instruction instruction) {
    Operand_State state = {};
    uint16_t dest_value = get_operand_value(main_memory, instruction.dest);
    uint16_t source_value = get_operand_value(main_memory, instruction.source);

    Seg_Mem dest_ptr = get_operand_ref(main_memory, instruction.dest);
    uint8_t dest_size = dest_ptr.size;

    int8_t displacement = (int8_t) (dest_value & 0xFF);

    state.operand = instruction.dest;
    state.before = dest_value;

    uint8_t CF = (registers_state.flags & FLAG_CF) != 0;
    uint8_t PF = (registers_state.flags & FLAG_PF) != 0;
    uint8_t AF = (registers_state.flags & FLAG_AF) != 0;
    uint8_t ZF = (registers_state.flags & FLAG_ZF) != 0;
    uint8_t SF = (registers_state.flags & FLAG_SF) != 0;
    uint8_t OF = (registers_state.flags & FLAG_OF) != 0;
    uint8_t IF = (registers_state.flags & FLAG_IF) != 0;
    uint8_t DF = (registers_state.flags & FLAG_DF) != 0;
    uint8_t TF = (registers_state.flags & FLAG_TF) != 0;

    switch (instruction.op_type) {
        case (Op_mov): {
            mov(main_memory, instruction);
        } break;

        case (Op_add): {
            add(main_memory, instruction);
            state.after = get_operand_value(main_memory, instruction.dest);
            check_common_flags(main_memory, instruction.dest);
            if (dest_size == 1)
                UPDATE_FLAG(FLAG_CF, (dest_value + source_value) & ~0xFFu);
            else
                UPDATE_FLAG(FLAG_CF, (dest_value + source_value) & ~0xFFFFu);
            uint16_t sign_bit = get_signed_bit(dest_size);
            uint16_t of = ((~(dest_value ^ source_value) & (dest_value ^ state.after)) & sign_bit) != 0;
            uint16_t af = ((dest_value ^ source_value ^ state.after) & 0x10) != 0;
            UPDATE_FLAG(FLAG_OF, of);
            UPDATE_FLAG(FLAG_AF, af);
        } break;

        case (Op_sub): {
            sub(main_memory, instruction);
            state.after = get_operand_value(main_memory, instruction.dest);
            check_common_flags(main_memory, instruction.dest);
            UPDATE_FLAG(FLAG_CF, source_value > dest_value);

            uint16_t sign_bit = get_signed_bit(dest_size);
            uint16_t of = (((dest_value ^ source_value) & (dest_value ^ state.after)) & sign_bit) != 0;
            uint16_t af = ((dest_value ^ source_value ^ state.after) & 0x10) != 0;
            UPDATE_FLAG(FLAG_OF, of);
            UPDATE_FLAG(FLAG_AF, af);
        } break;

        case (Op_cmp): {
            uint16_t result = dest_value - source_value;
            state.after = result;
            Instruction_Operand result_operand = immediate_operand(result, false);

            check_common_flags(main_memory, result_operand);
            UPDATE_FLAG(FLAG_CF, source_value > dest_value);

            uint16_t sign_bit = get_signed_bit(dest_size);
            uint16_t of = (((dest_value ^ source_value) & (dest_value ^ state.after)) & sign_bit) != 0;
            uint16_t af = ((dest_value ^ source_value ^ state.after) & 0x10) != 0;
            UPDATE_FLAG(FLAG_OF, of);
            UPDATE_FLAG(FLAG_AF, af);
        } break;

        case (Op_jo): {
            conditional_jump(main_memory, displacement, OF);
        } break;

        case (Op_jno): {
            conditional_jump(main_memory, displacement, !OF);
        } break;

        case (Op_jb): {
            conditional_jump(main_memory, displacement, CF);
        } break;

        case (Op_jnb): {
            conditional_jump(main_memory, displacement, !CF);
        } break;

        case (Op_je): {
            conditional_jump(main_memory, displacement, ZF);
        } break;

        case (Op_jne): {
            conditional_jump(main_memory, displacement, !ZF);
        } break;

        case (Op_jbe): {
            conditional_jump(main_memory, displacement, CF || ZF);
        } break;

        case (Op_ja): {
            conditional_jump(main_memory, displacement, !CF && !ZF);
        } break;

        case (Op_js): {
            conditional_jump(main_memory, displacement, SF);
        } break;

        case (Op_jns): {
            conditional_jump(main_memory, displacement, !SF);
        } break;

        case (Op_jp): {
            conditional_jump(main_memory, displacement, PF);
        } break;

        case (Op_jnp): {
            conditional_jump(main_memory, displacement, !PF);
        } break;

        case (Op_jl): {
            conditional_jump(main_memory, displacement, SF != OF);
        } break;

        case (Op_jnl): {
            conditional_jump(main_memory, displacement, SF == OF);
        } break;

        case (Op_jle): {
            conditional_jump(main_memory, displacement, ZF || (SF != OF));
        } break;

        case (Op_jg): {
            conditional_jump(main_memory, displacement, !ZF && (SF == OF));
        } break;

        case (Op_loop): {
            if (--registers_state.cx != 0) {
                jump(displacement);
            }
        } break;

        case (Op_loopz): {
            if (--registers_state.cx != 0 && ZF) {
                jump(displacement);
            }
        } break;

        case (Op_loopnz): {
            if (--registers_state.cx != 0 && !ZF) {
                jump(displacement);
            }
        } break;

        case (Op_jcxz): {
            if (registers_state.cx == 0) {
                jump(displacement);
            }
        } break;

        case (Op_Unimplemented): {
        } break;

        default: {
            printf("UNIMPLEMENTED SIMULATION\n");
        } break;
    }

    state.after = get_operand_value(main_memory, instruction.dest);
    return state;
}

static void check_common_flags(Seg_Mem *main_memory, Instruction_Operand result) {
    uint16_t operand_value = 0;
    Seg_Mem operand_mem = get_operand_ref(main_memory, result);
    if (operand_mem.size == 1) {
        uint8_t val = read_u8(operand_mem.memory);
        operand_value = val;
        UPDATE_FLAG(FLAG_SF, val >> 7);
    } else {
        uint16_t val = read_u16(operand_mem.memory);
        operand_value = val;
        UPDATE_FLAG(FLAG_SF, val >> 15);
    }

    UPDATE_FLAG(FLAG_ZF, operand_value == 0);

    uint16_t bits_set = 0;
    for (int i = 0; i < 8; i++) {
        if ((operand_value >> i) & 1) {
            bits_set++;
        }
    }
    UPDATE_FLAG(FLAG_PF, bits_set % 2 == 0);
}

uint16_t get_signed_bit(uint16_t size) {
    return (size == 1) ? 0x80 : 0x8000;
}

static void mov(Seg_Mem *main_memory, Instruction instruction) {
    set_operand_value(main_memory, instruction.dest, get_operand_value(main_memory, instruction.source));
}

static void add(Seg_Mem *main_memory, Instruction instruction) {
    uint16_t dest_value = get_operand_value(main_memory, instruction.dest);
    uint16_t source_value = get_operand_value(main_memory, instruction.source);
    uint16_t result = dest_value + source_value;
    set_operand_value(main_memory, instruction.dest, result);
}

static void sub(Seg_Mem *main_memory, Instruction instruction) {
    uint16_t dest_value = get_operand_value(main_memory, instruction.dest);
    uint16_t source_value = get_operand_value(main_memory, instruction.source);
    uint16_t result = dest_value - source_value;
    set_operand_value(main_memory, instruction.dest, result);
}

static void cmp(Seg_Mem *main_memory, Instruction instruction) {
    uint16_t dest_value = get_operand_value(main_memory, instruction.dest);
    uint16_t source_value = get_operand_value(main_memory, instruction.source);
    uint16_t result = dest_value + source_value;
    Instruction_Operand result_operand = immediate_operand(result, false);
    check_common_flags(main_memory, result_operand);
}

static void conditional_jump(Seg_Mem *main_memory, int8_t displacement, bool should_jump) {
    uint16_t new_ip = registers_state.ip + displacement;
    registers_state.ip = should_jump ? new_ip : registers_state.ip;
}

static void jump(int8_t displacement) {
    registers_state.ip += displacement;
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
        uint16_t address = calculate_effective_address(operand.address) + registers_state.ds;
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

    return (mem.size == 1) ? read_u8(mem.memory) : read_u16(mem.memory);
}

static Seg_Mem get_operand_ref(Seg_Mem *main_memory, Instruction_Operand operand) {
    Seg_Mem mem = {};
    if (operand.type == Operand_Register) {
        mem = get_register_ref(operand.reg);
    } else if (operand.type == Operand_Memory) { // gonna have to implement byte and word accesses
        uint16_t address = calculate_effective_address(operand.address) + registers_state.ds;
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
    if (reg_access.type == Register_General) {
        if (reg_access.w == 0) {
            uint8_t access = 0;
            // i spent way too long on this dumb ass thing just THINK MARK, THINK
            if (reg_access.reg_rm < 4) {
                access = reg_access.reg_rm * 2;
            } else {
                access = (reg_access.reg_rm - 4) * 2 + 1;
            }

            mem.memory = (uint8_t*)&registers_state.reg8[access];
            mem.base = 0;
            mem.mask = 0xFF;
            mem.size = 1;
        } else {
            mem.memory = (uint8_t*)&registers_state.reg16[reg_access.reg_rm];
            mem.base = 0;
            mem.mask = 0xFF;
            mem.size = 2;
        }
    } else {
        mem.memory = (uint8_t*)&registers_state.reg16[SEGMENT_REGISTER_START + reg_access.sr];
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
    printf(REG_FMT(cs), REG_ARGS(cs));
    printf(REG_FMT(es), REG_ARGS(es));
    printf(REG_FMT(ss), REG_ARGS(ss));
    printf(REG_FMT(ds), REG_ARGS(ds));
    printf(REG_FMT(ip), REG_ARGS(ip));
}

void print_real_registers_state() {
    for (int i = 0; i < REG_COUNT; i++) {
        printf("ALL: 0x%X ", registers_state.reg16[i]);
        printf("HIGH: 0x%X ", registers_state.reg8[i*2+1]);
        printf("LOW: 0x%X\n", registers_state.reg8[i*2]);
    }
}

void print_flags_state() {
    char flags[32];
    get_flags_str(flags, registers_state.flags);
    printf("flags: %s\n", flags);
}
