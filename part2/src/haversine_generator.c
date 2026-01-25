#include "haversine_generator.h"
#include "haversine_formula.h"
#include "util.h"

#define U64Max UINT64_MAX

Haversine_Pair generate_haversine_pair(random_series *series, u64 seed, f64 x_origin, f64 y_origin) {
    Haversine_Pair result = {0};

    f64 x0 = RandomDegree(series, x_origin, CLUSTER_RADIUS, X_MAX);
    f64 y0 = RandomDegree(series, y_origin, CLUSTER_RADIUS, Y_MAX);
    f64 x1 = RandomDegree(series, x_origin, CLUSTER_RADIUS, X_MAX);
    f64 y1 = RandomDegree(series, y_origin, CLUSTER_RADIUS, Y_MAX);

    result.x0 = x0;
    result.y0 = y0;
    result.x1 = x1;
    result.y1 = y1;

    return result;
}

f64 haversine_distance(Haversine_Pair *pair) {
    return ReferenceHaversine(pair->x0, pair->y0, pair->x1, pair->y1, EARTH_RADIUS);
}

u64 RotateLeft(u64 V, int Shift)
{
    u64 Result = ((V << Shift) | (V >> (64-Shift)));
    return Result;
}

u64 RandomU64(random_series *Series)
{
    u64 A = Series->A;
    u64 B = Series->B;
    u64 C = Series->C;
    u64 D = Series->D;

    u64 E = A - RotateLeft(B, 27);

    A = (B ^ RotateLeft(C, 17));
    B = (C + D);
    C = (D + E);
    D = (E + A);

    Series->A = A;
    Series->B = B;
    Series->C = C;
    Series->D = D;


    return D;
}


random_series Seed(u64 Value)
{
    random_series Series = {0};

    // NOTE(casey): This is the seed pattern for JSF generators, as per the original post
    Series.A = 0xf1ea5eed;
    Series.B = Value;
    Series.C = Value;
    Series.D = Value;

    u32 Count = 20;
    while(Count--)
    {
        RandomU64(&Series);
    }


    return Series;
}

f64 RandomInRange(random_series *Series, f64 Min, f64 Max)
{

    f64 t = (f64)RandomU64(Series) / (f64)U64Max;
    f64 Result = (1.0 - t)*Min + t*Max;


    return Result;
}

f64 RandomDegree(random_series *Series, f64 Center, f64 Radius, f64 MaxAllowed)
{
    f64 MinVal = Center - Radius;
    if(MinVal < -MaxAllowed)

    {
        MinVal = -MaxAllowed;
    }

    f64 MaxVal = Center + Radius;
    if(MaxVal > MaxAllowed)
    {
        MaxVal = MaxAllowed;
    }


    f64 Result = RandomInRange(Series, MinVal, MaxVal);
    return Result;
}
