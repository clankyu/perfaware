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
uint32_t dissasemble_8086(Seg_Mem *main_memory, Seg_Mem *instruction_stream, Instruction *dest);
void generate_asm_file(Instruction *instructions, uint16_t instruction_count, bool print);

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
#define RESULT_ASM_MEM_MAX_SIZE 1000000
#define MAX_DECODED_INSTRUCTIONS 1000
#define MAX_JUMP_LOCATIONS 8

uint32_t decoded_instruction_count = 0;

uint32_t sim_flags = 0;

int16_t jump_locations[MAX_JUMP_LOCATIONS] = {0};
uint16_t jump_location_count = 0;

int main(int argc, char **argv) {
    uint8_t program_mem[PROGRAM_MEM_SIZE];
    uint8_t result_binary_mem[RESULT_BINARY_MAX_SIZE];
    uint8_t result_asm_mem[RESULT_ASM_MEM_MAX_SIZE];
    Instruction decoded_instructions[MAX_DECODED_INSTRUCTIONS];

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
        print_flags_state();

        printf("\nPrinting decoded instructions:\n");
        uint16_t instruction_count = dissasemble_8086(&main_memory, &instruction_stream, decoded_instructions);
        generate_asm_file(decoded_instructions, instruction_count, true);
    } else {
        printf("\nPrinting decoded instructions:\n");
        uint16_t instruction_count = dissasemble_8086(&main_memory, &instruction_stream, decoded_instructions);
        generate_asm_file(decoded_instructions, instruction_count, true);
        print_bits(buffer, file_size);
    }

    printf("\n");
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

            printf("\nOriginal binary bits:\n");
            print_bits(instruction_stream.memory, instruction_stream.size);

            printf("\nResult binary bits:\n");
            print_bits(file_result.memory, file_result.size);
            printf("\n");
        }
    }

    fclose(file);

    return 0;
}

void sim8086(Seg_Mem *main_memory, Seg_Mem *byte_data) {
    bool pattern_matched = false;

    while (registers_state.ip < byte_data->size) {
        uint8_t opcode_byte = main_memory->memory[registers_state.cs + registers_state.ip];
        pattern_matched = false;

        for (int j = 0; j < sizeof(opcode_patterns) / sizeof(Opcode_Pattern); j++) {
            if ((opcode_byte & opcode_patterns[j].mask) == opcode_patterns[j].opcode) {
                pattern_matched = true;

                Flags_State flags_state = {};
                flags_state.before = registers_state.flags;

                Instruction instruction = opcode_patterns[j].decode_fn(main_memory->memory + registers_state.cs, registers_state.ip);
                Operand_State instruction_state = execute_instruction(main_memory, instruction);

                flags_state.after = registers_state.flags;
                Instruction_String_Expression expression = get_instruction_str(&instruction);
                print_instruction_and_operand_state(&instruction, &expression, instruction_state, flags_state);

                registers_state.ip += instruction.size;

                break;
            }
        }

        if (!pattern_matched) {
            printf("Instruction not found. instruction byte: ");
            print_bits(&opcode_byte, 1);
            printf("\n");
            registers_state.ip++;
        }
    }
}

uint32_t dissasemble_8086(Seg_Mem *main_memory, Seg_Mem *instruction_stream, Instruction *dest) {
    bool pattern_matched = false;
    uint32_t instruction_count = 0;

    for (int i = 0; i < instruction_stream->size;) {
        uint8_t byte = instruction_stream->memory[instruction_stream->base + i];
        pattern_matched = false;

        for (int j = 0; j < sizeof(opcode_patterns) / sizeof(Opcode_Pattern); j++) {
            if ((byte & opcode_patterns[j].mask) == opcode_patterns[j].opcode) {
                pattern_matched = true;

                Instruction instruction = opcode_patterns[j].decode_fn(instruction_stream->memory + instruction_stream->base, i + registers_state.cs);
                instruction.address = i + registers_state.cs;

                i += instruction.size;

                assert(instruction_count < MAX_DECODED_INSTRUCTIONS);
                dest[instruction_count] = instruction;
                instruction_count++;

                if (opcode_is_jump_type(byte)) {
                    uint16_t target = i + (int8_t)(instruction.dest.immediate.val & 0xFF);
                    bool already_exists = false;

                    for (int k = 0; k < MAX_JUMP_LOCATIONS; k++) {
                        if (jump_locations[k] == target) {
                            assert((target) != 0); // this is bad but i want to finish
                            already_exists = true;
                        }
                    }

                    if (!already_exists) {
                        assert(jump_location_count < 8);
                        jump_locations[jump_location_count] = target;
                        jump_location_count++;
                    }
                }

                break;
            }
        }

        if (!pattern_matched) {
            printf("Instruction not found. instruction byte: ");
            print_bits(&byte, 1);
            printf("\n");
            i++;
        }
    }

    return instruction_count;
}

void generate_asm_file(Instruction *instructions, uint16_t instruction_count, bool print) {
    FILE *result_asm_file = fopen("results/result.asm", "w");
    fprintf(result_asm_file, "bits 16\n");
    fprintf(result_asm_file, "cpu 8086\n");

    for (int i = 0; i < instruction_count; i++) {
        Instruction instruction = instructions[i];
        Instruction_String_Expression expression = get_instruction_str(&instruction);

        uint16_t jump_location = 0;
        for (int j = 0; j < jump_location_count; j++) {
            if (jump_locations[j] == instruction.address) {
                if (print) {printf(".L%u:\n", instruction.address);}
                fprintf(result_asm_file, ".L%u:\n", instruction.address);
            }
        }

        if (instruction.op_type >= Op_je && instruction.op_type <= Op_jcxz) {
            fprintf(result_asm_file, "%s .L%u\n", expression.mnemonic, instruction.address + instruction.size + (int8_t)(instruction.dest.immediate.val & 0xFF));
        } else if (instruction.source.type == Operand_None) {
            fprintf(result_asm_file, "%s %s\n", expression.mnemonic, expression.dest);
        } else {
            fprintf(result_asm_file, "%s %s, %s\n", expression.mnemonic, expression.dest, expression.source);
        }

        if (print) {
            instruction_print(instruction, &expression);
        }
    }

    fclose(result_asm_file);
}

bool data_is_the_same(Seg_Mem *data1, Seg_Mem *data2) {
    bool same = true;

    if (data1->size != data2->size) {
        same = false;
    }

    for (int i = 0; i < data1->size; i++) {
        uint8_t byte = data2->memory[i];
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
    char *nasm[] = {"nasm", "-f", "bin", "-O0", "-w+all", "-DCPU=8086", "results/result.asm", "-o", "results/result.bin", 0x0};
    return run_cmd(nasm);
}

void compile_result_asm_unsafe() {
    system("nasm -f bin -O0 -w+all -DCPU=8086 results/result.asm -o results/result.bin");
}

void init_main_memory(Seg_Mem *main_memory, uint8_t *program_mem) {
    main_memory->memory = program_mem;
    main_memory->base = 0;
    main_memory->size = PROGRAM_MEM_SIZE;
    main_memory->mask = PROGRAM_MEM_SIZE;
}
