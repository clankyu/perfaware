#include "util.h"
#include <cstdlib>
#include <stdio.h>

typedef struct {
    f64 x0, y0;
    f64 x1, y1;
} Haversine_Pair;

Haversine_Pair generate_haversine_pair();


void write_file(u8 *data, u32 count);

