#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "decoder.h"

int main(int argc, char **argv) {
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

    print_bits(buffer, file_size);
    printf("\n");

    bool pattern_matched = false;

    printf("bits 16\n");
    for (int i = 0; i < file_size;) {
        uint8_t byte = buffer[i];
        pattern_matched = false;

        for (int j = 0; j < sizeof(opcode_patterns) / sizeof(Opcode_Pattern); j++) {
            if ((byte & opcode_patterns[j].mask) == opcode_patterns[j].opcode) {
                pattern_matched = true;
                Instruction instruction = opcode_patterns[j].decode_fn(buffer, i);
                //printf("index: %i, ", i);
                instruction_print(&instruction);
                i += instruction.size;

                break;
            }
        }

        if (!pattern_matched) {
            Instruction def = default_instruction(buffer, i);
            instruction_print(&def);
            i++;
        }
    }

    free(file);

    return 0;
}

