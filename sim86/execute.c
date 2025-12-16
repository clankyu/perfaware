#include "execute.h"
#include "instruction.h"

void execute_instructions(Seg_Mem *main_memory, Seg_Mem *instructions_mem) {
    uint32_t operand_state_base_ptr = instructions_mem->base + instructions_mem->size;
    const int32_t instruction_count = instructions_mem->size / sizeof(Instruction);


    Instruction *instructions = (void*)access_memory(instructions_mem, 0);
    for (int i = 0; i < instructions_mem->size / sizeof(Instruction); i++) {
        Seg_Mem operand_state_mem = memory_alloc(main_memory, sizeof(Operand_State), operand_state_base_ptr);
        Instruction *instruction = &instructions[i];
        Operand_State operand_state = {};
        //operand_state.operand =
    }

    // i need to refactor so Instruction structs now have Isntruction_Operand so i actually know what type of operand it is
    // so that i can know whether it's a register or a memory address
}

