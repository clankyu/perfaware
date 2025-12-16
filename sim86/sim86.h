#pragma once

#include <stdio.h>
#include <stdint.h>

#define array_count(array) (sizeof(array) / sizeof((array)[0]))

void print_bits(uint8_t *data, uint32_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("bit %i: ", (int) i);
        for (int bit = 7; bit >= 0; bit--) {
            printf("%d", (data[i] >> bit) & 1);
        }
        printf(" ");
    }
}

