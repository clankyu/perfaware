#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "decoder.h"
#include "memory.h"

int main(int argc, char **argv) {
    printf("Argument count: %i\n", argc);
    if (argc < 2) {
        printf("No arguments given, exiting program.\n");
        return 0;
    }

    char file_name[100];
    strcpy(file_name, argv[1]);

    label_init();

    Memory memory;
    load_memory_from_file(file_name, &memory);

    print_bits(memory.bytes, memory.offset);
    printf("\n");

    bool pattern_matched = false;
    int32_t number_instructions = 0;
    int32_t instruction_start_idx = memory.offset + 1;
    int32_t number_labels = 0;

    printf("bits 16\n");
    for (int i = 0; i < memory.offset;) {
        uint8_t byte = memory.bytes[i];
        pattern_matched = false;

        for (int j = 0; j < sizeof(opcode_patterns) / sizeof(Opcode_Pattern); j++) {
            if ((byte & opcode_patterns[j].mask) == opcode_patterns[j].opcode) {
                pattern_matched = true;
                Instruction instruction = opcode_patterns[j].decode_fn(memory.bytes, i);
                Instruction *instr_ptr = memory_alloc(&memory, sizeof(Instruction));
                *instr_ptr = instruction;

                //instruction_print(&instruction);
                i += instruction.size;
                number_instructions++;

                break;
            }
        }

        if (!pattern_matched) {
            //Instruction def = default_instruction(memory.bytes, i);
            //instruction_print(&def);
            i++;
        }
    }

    int32_t byte_offset = 0;
    for (int i = 0; i < number_instructions; i++) {
        Instruction *instruction = (void*) &memory.bytes[instruction_start_idx];
        byte_offset += instruction->size;
        label_print(byte_offset);
        instruction_print(instruction);
        instruction_start_idx += sizeof(Instruction);
    }

    return 0;
}

