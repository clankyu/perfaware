#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int16_t ax = 0;
int16_t bx = 0;
int16_t cx = 0;
int16_t dx = 0;

int16_t al = 0;
int16_t bl = 0;
int16_t cl = 0;
int16_t dl = 0;

int16_t ah = 0;
int16_t bh = 0;
int16_t ch = 0;
int16_t dh = 0;

int16_t sp = 0;
int16_t bp = 0;
int16_t si = 0;
int16_t di = 0;

char *reg_field_table_str[2][8] = {
    { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" },
    { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" }
};

char *effective_address_table_str[3][8] = {
    { "[bx + si]", "[bx + di]", "[bp + si]", "[bp + di]",
      "[si]", "[di]", "[DIRECT ADDRESS]", "[bx]" },

    { "[bx + si + ", "[bx + di + ", "[bp + si + ", "[bp + di + ",
      "[si ", "[di + ", "[bp + ", "[bx + " },

    { "[bx + si + ", "[bx + di + ", "[bp + si + ", "[bp + di + ",
      "[si + ", "[di + ", "[bp + ", "[bx + " }
};

int main(int argc, char **argv) {
    printf("Argument count: %i\n", argc);
    if (argc < 2) {
        printf("No arguments given, exiting program.\n");
        return 0;
    }

    for (int i = 0; i < argc; i++) {
        printf("arg %i: %s\n", i, argv[i]);
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

    #define MOV_MASK 0b10001000
    #define IMMD_TO_REG_MOV 0b10110000

    for (size_t i = 0; i < file_size; i++) {
        printf("bit %i: ", (int) i);
        for (int bit = 7; bit >= 0; bit--) {
            printf("%d", (buffer[i] >> bit) & 1);
        }
        printf(" ");
    }
    printf("\n");

    printf("bits 16\n");
    for (int i = 0; i < file_size; i++) {
        //printf("%i\n", i);
        uint8_t byte1 = buffer[i];
        uint8_t byte2 = buffer[i + 1];
        i++;

        char *instruction = malloc(3);
        char *dest = malloc(30);
        char *source = malloc(30);

        int8_t d = 0;
        int8_t w = 0;
        int8_t mod = 0;
        int8_t reg = 0;
        int8_t rm = 0;

        if ((byte1 & 0b11111100) == MOV_MASK) {
            instruction = "mov";
            d = (byte1 >> 1) & 1;
            w = byte1 & 1;

            mod = byte2 >> 6;
            reg = (byte2 >> 3) & 0b111;
            rm = byte2 & 0b111;

            if (d) {
                if (mod == 0) {
                    if (rm == 7) {
                        int16_t displacement = (int16_t)buffer[i+1] | ((int16_t)buffer[i+2] << 8);
                        sprintf(source, "[%d]", displacement);
                        dest = reg_field_table_str[w][reg];
                    } else {
                        dest = reg_field_table_str[w][reg];
                        source = effective_address_table_str[mod][rm];
                    }
                } else if (mod == 1 || mod == 2) {
                    dest = reg_field_table_str[w][reg];
                    char *append = malloc(20);

                    if (mod == 2) {
                        int16_t displacement = (int16_t)buffer[i+1] | ((int16_t)buffer[i+2] << 8);
                        if (displacement == 0) {
                            char *s = malloc(4);
                            for (int i = 0; i < 3; i++) {
                                s[i] = effective_address_table_str[w][rm][i];
                            }
                            s[3] = ']';
                            sprintf(source, "%s", s);

                            free(s);
                        } else {
                            char *full_source = malloc(30);

                            sprintf(append, "%d]", displacement);
                            strcat(full_source, effective_address_table_str[mod][rm]);
                            strcat(full_source, append);

                            strcpy(source, full_source);

                            free(full_source);
                        }

                        i+=2;
                    } else {
                        int8_t displacement = (int8_t) buffer[i+1];
                        if (displacement == 0) {
                            char *s = malloc(4);
                            for (int i = 0; i < 3; i++) {
                                s[i] = effective_address_table_str[w][rm][i];
                            }
                            s[3] = ']';

                            sprintf(source, "%s", s);

                            free(s);
                        } else {
                            sprintf(append, "%d]", displacement);
                            char *full_source = malloc(30);

                            strcat(full_source, effective_address_table_str[mod][rm]);
                            strcat(full_source, append);

                            strcpy(source, full_source);

                            free(full_source);
                        }

                        i++;
                    }

                    free(append);
                } else if (mod == 3) {
                    dest = reg_field_table_str[w][reg];
                    source = reg_field_table_str[w][rm];
                }

                /*
                dest = reg_field_table_str[w][reg];
                source = reg_field_table_str[w][rm];
                reg_field_table[w][reg] = reg_field_table[w][rm];
                */
            } else {
                if (mod == 0) {
                    if (rm == 7) {
                        int16_t displacement = (int16_t)buffer[i+1] | ((int16_t)buffer[i+2] << 8);
                        sprintf(source, "[%d]", displacement);
                        dest = reg_field_table_str[w][reg];
                    } else {
                        dest = reg_field_table_str[w][reg];
                        source = effective_address_table_str[mod][rm];
                    }
                } else if (mod == 1 || mod == 2) {
                    dest = reg_field_table_str[w][reg];
                    char *append = malloc(20);

                    if (mod == 2) {
                        int16_t displacement = (int16_t)buffer[i+1] | ((int16_t)buffer[i+2] << 8);
                        if (displacement == 0) {
                            char *s = malloc(4);
                            for (int i = 0; i < 3; i++) {
                                s[i] = effective_address_table_str[w][rm][i];
                            }
                            s[3] = ']';
                            sprintf(source, "%s", s);

                            free(s);
                        } else {
                            char *full_source = malloc(30);

                            sprintf(append, "%d]", displacement);
                            strcat(full_source, effective_address_table_str[mod][rm]);
                            strcat(full_source, append);

                            strcpy(source, full_source);

                            free(full_source);
                        }

                        i+=2;
                    } else {
                        int8_t displacement = (int8_t) buffer[i+1];
                        if (displacement == 0) {
                            char *s = malloc(4);
                            for (int i = 0; i < 3; i++) {
                                s[i] = effective_address_table_str[mod][rm][i];
                            }
                            s[3] = ']';

                            sprintf(source, "%s", s);

                            free(s);
                        } else {
                            sprintf(append, "%d]", displacement);
                            char *full_source = malloc(30);

                            strcat(full_source, effective_address_table_str[mod][rm]);
                            strcat(full_source, append);

                            strcpy(source, full_source);

                            free(full_source);
                        }

                        i++;
                    }

                    free(append);
                } else if (mod == 3){
                    source = reg_field_table_str[w][rm];
                    dest = reg_field_table_str[w][reg];
                }

                char *temp = source;
                source = dest;
                dest = temp;
            }
        } else if ((byte1 & 0b11110000) == IMMD_TO_REG_MOV) {
            instruction = "mov";
            w = (byte1 >> 3) & 0b1;
            reg = byte1 & 0b111;

            dest = reg_field_table_str[w][reg];
            if (w == 1) {
                int16_t s = (int16_t)buffer[i] | ((int16_t)buffer[i+1] << 8);
                sprintf(source, "%d", s);
                i++;
            } else {
                sprintf(source, "%d", (int8_t) byte2);
            }
        } else {
            printf("unknown\n");
            continue;
        }


        printf("%s %s, %s\n", instruction, dest, source);
    }

    free(file);

    return 0;
}

