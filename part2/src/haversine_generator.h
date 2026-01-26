#pragma once

#include "util.h"
#include <stdio.h>

#define CLUSTER_RADIUS ((f64)10.0f)
#define X_MAX ((f64)180.0f)
#define X_MIN ((f64)-180.0f)
#define Y_MAX ((f64)90.0f)
#define Y_MIN ((f64)-90.0f)

#define MIN_PAIRS 1000
#define MAX_PAIRS 10000000

typedef struct {
    f64 x0, y0;
    f64 x1, y1;
} Haversine_Pair;

typedef struct
{
    u64 A, B, C, D;
} random_series;

Haversine_Pair generate_haversine_pair(random_series *series, u64 seed, f64 x_origin, f64 y_origin);
f64 haversine_distance(Haversine_Pair *pair);

u64 RotateLeft(u64 V, int Shift);
u64 RandomU64(random_series *Series);
random_series Seed(u64 Value);
f64 RandomInRange(random_series *Series, f64 Min, f64 Max);
f64 RandomDegree(random_series *Series, f64 Center, f64 Radius, f64 MaxAllowed);
