#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "decoder.h"
#include "memory.h"
#include "execute.h"
#include "instruction.h"
#include "sim86.h"
#include "text.h"

Seg_Mem dissasemble_8086(Seg_Mem *main_memory, Seg_Mem *byte_data);
void run_8086(Seg_Mem *main_memory, Seg_Mem *instructions_mem);
void init_main_memory(Seg_Mem *main_memory, uint8_t *program_mem);

int main(int argc, char **argv) {
    uint8_t program_mem[PROGRAM_MEM_SIZE];

    printf("Argument count: %i\n", argc);
    if (argc < 2) {
        printf("No arguments given, exiting program.\n");
        return 0;
    }

    int32_t file_size = 0;
    char *file_name = malloc(100);
    file_name = argv[1];

    FILE *file = fopen(file_name, "rb");
    if (!file) {
        printf("Failed to open file\n");
        return 1;
    }

    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);
    rewind(file);
    printf("%s size: %i\n", file_name, file_size);

    uint8_t buffer[file_size];
    fread(buffer, file_size, 1, file);

    Seg_Mem main_memory;
    init_main_memory(&main_memory, program_mem);

    Seg_Mem instruction_stream = memory_alloc(&main_memory, file_size, main_memory.base);
    memcpy(instruction_stream.memory, buffer, file_size);

    Seg_Mem instructions_mem = dissasemble_8086(&main_memory, &instruction_stream);

    Instruction *instructions = (void*) access_memory(&instructions_mem, 0);
    for (int i = 0; i < instructions_mem.size / sizeof(Instruction); i++) {
        Instruction instruction = instructions[i];
        Instruction_String_Expression expr = get_instruction_str(&instruction);
        instruction_print(&expr, instruction);
    }


    free(file);

    return 0;
}

// basically this function allocates directly into main memory the Instruction struct's data,
// and then at the end we manually create a Seg_Mem that is all of the instructions, very ugly
Seg_Mem dissasemble_8086(Seg_Mem *main_memory, Seg_Mem *byte_data) {
    bool pattern_matched = false;
    uint32_t memory_base = byte_data->base + byte_data->size;
    uint32_t instruction_count = 0;

    for (int i = 0; i < byte_data->size;) {
        uint8_t byte = byte_data->memory[byte_data->base + i];
        pattern_matched = false;

        for (int j = 0; j < sizeof(opcode_patterns) / sizeof(Opcode_Pattern); j++) {
            if ((byte & opcode_patterns[j].mask) == opcode_patterns[j].opcode) {
                pattern_matched = true;

                Instruction instruction = opcode_patterns[j].decode_fn(byte_data->memory + byte_data->base, i);
                memcpy(main_memory->memory + memory_base, &instruction, sizeof(Instruction));
                memory_base += sizeof(Instruction);
                i += instruction.size;
                instruction_count++;

                break;
            }
        }

        if (!pattern_matched) {
            printf("Instruction not found. instruction byte");
            print_bits(&byte, 1);
            i++;
        }
    }

    Seg_Mem instructions_mem;
    instructions_mem.memory = main_memory->memory;
    instructions_mem.base = byte_data->size;
    instructions_mem.size = sizeof(Instruction) * instruction_count;
    instructions_mem.mask = 0xFFFF;

    return instructions_mem;
}

void run_8086(Seg_Mem *main_memory, Seg_Mem *instructions_mem) {
    int32_t instruction_count = instructions_mem->size / sizeof(Instruction);
    printf("executing all instructions, instruction count %i\n", instruction_count);

    execute_instructions(main_memory, instructions_mem);
}

void init_main_memory(Seg_Mem *main_memory, uint8_t *program_mem) {
    main_memory->memory = program_mem;
    main_memory->base = 0;
    main_memory->size = PROGRAM_MEM_SIZE;
    main_memory->mask = PROGRAM_MEM_SIZE;
}
