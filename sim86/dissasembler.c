#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

    for (size_t i = 0; i < file_size; i++) {
        printf("bit %i: ", (int) i);
        for (int bit = 7; bit >= 0; bit--) {
            printf("%d", (buffer[i] >> bit) & 1);
        }
        printf(" ");
    }
    printf("\n");

    printf("bits 16\n");
    for (int i = 0; i < file_size;) {
        uint8_t byte = buffer[i];

        for (int j = 0; j < sizeof(opcode_patterns) / sizeof(Opcode_Pattern); j++) {
            if ((byte & opcode_patterns[j].mask) == opcode_patterns[j].opcode) {
                Instruction instruction = opcode_patterns[j].decode_fn(buffer, i);
                instruction_print(&instruction);
                i += instruction.size;

                continue;
            }
        }

        default_instruction(buffer, i); // if pattern not found
        i++;
    }

    free(file);

    return 0;
}

