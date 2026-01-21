#pragma once

#include "util.h"

#define EARTH_RADIUS 6372.8f

f64 Square(f64 A);
f64 RadiansFromDegrees(f64 Degrees);
f64 ReferenceHaversine(f64 X0, f64 Y0, f64 X1, f64 Y1, f64 EarthRadius);
