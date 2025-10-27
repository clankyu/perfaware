#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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

int16_t *reg_field_table[2][8] = {
    { &al, &cl, &dl, &bl, &ah, &ch, &dh, &bh },
    { &ax, &cx, &dx, &bx, &sp, &bp, &si, &di }
};

void update_regs(void);

int main(int argc, char **argv) {
    if (argc < 1) {
        printf("No arguments given, exiting program.\n");
        return 0;
    }

    int32_t file_size = 0;
    char *file_name = malloc(100);
    file_name = argv[0];

    FILE *file = fopen(file_name, "rb");
    if (!file) {
        printf("Failed to open file\n");
        return 1;
    }

    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);
    rewind(file);
    printf("%s size: %i", file_name, file_size);

    uint8_t buffer[file_size];
    fread(buffer, file_size, 1, file);

    return 0;
}

void update_regs(void) {
    al = ax & 0xFF;
    bl = bx & 0xFF;
    cl = cx & 0xFF;
    dl = dx & 0xFF;

    ah = ax >> 8;
    bh = bx >> 8;
    ch = cx >> 8;
    dh = dx >> 8;
}
