#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "haversine_generator.h"

#define MIN_PAIRS 10
#define MAX_PAIRS 1000000
u32 program_flags = 0;
static FILE *Open(long long unsigned PairCount, char const *Label, char const *Extension);

int main(int argc, char **argv) {
    printf("Starting haversine generator.\n");
    if (argc < 4) {
        fprintf(stderr, "Failed to start haversine generator. Not enough arguments were given.\nExample: ./program -cluster 10000\n");
    }

    if (strcmp(argv[1], "-cluster") == 0) {
        program_flags |= CLUSTER_FLAG;
    } else if (strcmp(argv[1], "-uniform") == 0) {
        fprintf(stderr, "Using unimplemented form '-uniform'\n");
        program_flags |= CLUSTER_FLAG;
    } else {
        program_flags |= CLUSTER_FLAG;
    }

    u64 pair_count = 0;
    pair_count = atoll(argv[2]);
    if (pair_count < MIN_PAIRS) {
        fprintf(stderr, "Error: input pair count is less than minimum (%u)\n", MIN_PAIRS);
    }
    if (pair_count > MAX_PAIRS) {
        fprintf(stderr, "Error: input pair count is more than maximum (%u)\n", MAX_PAIRS);
    }

    u64 seed = atoll(argv[3]);
    random_series series = Seed(seed);
    u64 cluster_count_max = 1 + (pair_count / 64);

    if (pair_count <= MAX_PAIRS) {
        FILE *distances_file = Open(pair_count, "haversine_distances", "f64");
        FILE *json_file = Open(pair_count, "haversine_pairs", "json");

        if (distances_file && json_file) {
            fprintf(json_file, "{\"pairs\":[\n");
            f64 sum = 0;
            f64 sum_coef = 1.0 / (f64)pair_count;

            f64 x_cluster, y_cluster;

            x_cluster = RandomInRange(&series, X_MIN, X_MAX);
            y_cluster = RandomInRange(&series, Y_MIN, Y_MAX);

            for (u32 i = 0; i < pair_count; i++) {
                // TODO: implement cluster pair generation
            }
        }
    }

exit:

    return 0;
}

static FILE *Open(long long unsigned PairCount, char const *Label, char const *Extension)
{
    char Temp[256];
    sprintf(Temp, "data_%llu_%s.%s", PairCount, Label, Extension);
    FILE *Result = fopen(Temp, "wb");
    if(!Result)
    {
        fprintf(stderr, "Unable to open \"%s\" for writing.\n", Temp);
    }

    return Result;
}

