#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "decoder.h"
#include "memory.h"
#include "execute.h"
#include "instruction.h"
#include "text.h"

void sim8086(Seg_Mem *main_memory, Seg_Mem *byte_data);
Seg_Mem dissasemble_8086(Seg_Mem *main_memory, Seg_Mem *byte_data);
void run_8086(Seg_Mem *main_memory, Seg_Mem *instructions_mem);
void init_main_memory(Seg_Mem *main_memory, uint8_t *program_mem);
bool data_is_the_same(Seg_Mem *data1, Seg_Mem *data_2);
int compile_result_asm();
int run_cmd(char *cmd[]);
void compile_result_asm_unsafe();

#define SIMULATE_FLAG 1
#define ASM_FILE_NAME "result.asm"
#define ASM_COMPILE_RESULT_NAME "result.bin"
#define RESULT_BINARY_MAX_SIZE 1000000

uint32_t sim_flags = 0;

int main(int argc, char **argv) {
    uint8_t program_mem[PROGRAM_MEM_SIZE];
    uint8_t result_binary_mem[RESULT_BINARY_MAX_SIZE];

    if (argc < 2) {
        printf("No arguments given, exiting program.\n");
        return 0;
    }

    int32_t file_size = 0;
    char *file_name = argv[1];

    for (int i = 1; i < argc; i++) {
        if (strcmp("-sim", argv[i]) == 0) {
            sim_flags |= SIMULATE_FLAG;
        }
    }

    FILE *file = fopen(file_name, "rb");
    if (!file) {
        printf("Failed to open file\n");
        return 1;
    }

    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    uint8_t buffer[file_size];
    fread(buffer, file_size, 1, file);

    Seg_Mem main_memory;
    init_main_memory(&main_memory, program_mem);

    Seg_Mem instruction_stream = memory_alloc(&main_memory, file_size, main_memory.base);
    memcpy(instruction_stream.memory, buffer, file_size);

    bool byte_matches = true;

    if (sim_flags & SIMULATE_FLAG) {
        printf("Simulating 8086: \n");

        registers_state.cs = 0;
        registers_state.ds = instruction_stream.size;;
        registers_state.ss = PROGRAM_MEM_SIZE - 1000;
        registers_state.sp = registers_state.ss;
        sim8086(&main_memory, &instruction_stream);
        printf("\nFinal registers:\n");
        print_registers_state();
    } else {
        Seg_Mem instructions_mem = dissasemble_8086(&main_memory, &instruction_stream);
        print_bits(buffer, file_size);

        FILE *file_result = fopen("results/result.asm", "w");
        fprintf(file_result, "bits 16\n");

        printf("bits 16\n");
        Instruction *instructions = (void*) access_memory(&instructions_mem, 0);
        printf("DECODING RESULTS:\n");
        for (int i = 0; i < instructions_mem.size / sizeof(Instruction); i++) {
            Instruction instruction = instructions[i];
            Instruction_String_Expression expr = get_instruction_str(&instruction);
            instruction_print(&expr, instruction);

            if (instruction.source.type == Operand_None) {
                fprintf(file_result, "%s %s\n", expr.mnemonic, expr.dest);
            } else {
                fprintf(file_result, "%s %s, %s\n", expr.mnemonic, expr.dest, expr.source);
            }
        }
        printf("\n");

        fclose(file_result);
    }

    printf("Compiling generated assembly:\n");
    if (compile_result_asm() != 0) {
        printf("Failed to compile generated assembly.\n");
    } else {
        Seg_Mem file_result;
        file_result.memory = result_binary_mem;
        load_memory_from_file("results/result.bin", &file_result);
        if (data_is_the_same(&instruction_stream, &file_result)) {
            printf("Compiled assembly result.bin is the same as %s\nSuccessfully dissasembled binary.\n", file_name);
        } else {
            printf("Failed to dissasemble binary with accuracy.\n");
            printf("Compiled assembly result.bin is not the same as %s\nSuccessfully dissasembled binary.\n", file_name);
        }
    }

    fclose(file);

    return 0;
}

void sim8086(Seg_Mem *main_memory, Seg_Mem *byte_data) {
    FILE *file_result = fopen("results/result.asm", "w");
    fprintf(file_result, "bits 16\n");

    bool pattern_matched = false;

    while (registers_state.ip < byte_data->size) {
        uint8_t opcode_byte = main_memory->memory[registers_state.cs + registers_state.ip];
        pattern_matched = false;

        for (int j = 0; j < sizeof(opcode_patterns) / sizeof(Opcode_Pattern); j++) {
            if ((opcode_byte & opcode_patterns[j].mask) == opcode_patterns[j].opcode) {
                pattern_matched = true;

                Instruction instruction = opcode_patterns[j].decode_fn(main_memory->memory + registers_state.cs, registers_state.ip);
                Operand_State instruction_state = execute_instruction(main_memory, instruction);
                Instruction_String_Expression expression = get_instruction_str(&instruction);
                print_instruction_and_operand_state(&instruction, &expression, instruction_state);

                if (instruction.source.type == Operand_None) {
                    fprintf(file_result, "%s %s\n", expression.mnemonic, expression.dest);
                } else {
                    fprintf(file_result, "%s %s, %s\n", expression.mnemonic, expression.dest, expression.source);
                }

                registers_state.ip += instruction.size;

                break;
            }
        }

        if (!pattern_matched) {
            printf("Instruction not found. instruction byte");
            print_bits(&opcode_byte, 1);
            registers_state.ip++;
        }
    }

    fclose(file_result);
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

bool data_is_the_same(Seg_Mem *data1, Seg_Mem *data2) {
    bool same = true;

    if (data1->size != data2->size) {
        same = false;
    }

    for (int i = 0; i < data1->size; i++) {
        if (data1->memory[i] != data2->memory[i]) {
            same = false;
            break;
        }
    }

    return same;
}

int run_cmd(char *cmd[]) {
    pid_t pid = fork();

    if (pid == 0) {
        execvp(cmd[0], cmd);
        perror(cmd[0]);
        _exit(1);
    }

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return 1;
}

int compile_result_asm() {
    char *nasm[] = {"nasm", "-f", "bin", "results/result.asm", "-o", "results/result.bin", 0x0};
    return run_cmd(nasm);
}

void compile_result_asm_unsafe() {
    system("nasm -f bin results/result.asm -o results/result.bin");
}

void init_main_memory(Seg_Mem *main_memory, uint8_t *program_mem) {
    main_memory->memory = program_mem;
    main_memory->base = 0;
    main_memory->size = PROGRAM_MEM_SIZE;
    main_memory->mask = PROGRAM_MEM_SIZE;
}
