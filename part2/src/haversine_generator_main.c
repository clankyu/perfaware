#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "haversine_generator.h"

u32 program_flags = 0;
b32 program_failed = 0;

static FILE *Open(long long unsigned PairCount, char const *Label, char const *Extension);

int main(int argc, char **argv) {
    printf("Starting haversine generator.\n");

    char *method_name;
    u64 pair_count = 0;
    u64 seed = 0;

    if (argc < 4) {
        fprintf(stderr, "Failed to start haversine generator. Not enough arguments were given.\nExample: ./program -cluster 10000 (pairs) 4123124 (seed)\n");
        program_failed = true;
    } else {
        method_name = argv[1];
        if (strcmp(argv[1], "-cluster") == 0) {
            program_flags |= CLUSTER_FLAG;
        } else if (strcmp(argv[1], "-uniform") == 0) {
            fprintf(stderr, "Using unimplemented form '-uniform'\n");
            program_flags |= CLUSTER_FLAG;
        } else {
            program_flags |= CLUSTER_FLAG;
        }

        pair_count = atoll(argv[2]);
        if (pair_count < MIN_PAIRS) {
            fprintf(stderr, "Error: input pair count is less than minimum (%u)\n", MIN_PAIRS);
            program_failed = true;
        }
        if (pair_count > MAX_PAIRS) {
            fprintf(stderr, "Error: input pair count is more than maximum (%u)\n", MAX_PAIRS);
            program_failed = true;
        }

        seed = atoll(argv[3]);
    }

    random_series series = Seed(seed);
    const u32 pairs_per_cluster = pair_count / 1000;

    f64 sum = 0;

    if (pair_count <= MAX_PAIRS && pair_count >= MIN_PAIRS) {
        FILE *distances_file = Open(pair_count, "haversine_distances", "f64");
        FILE *json_file = Open(pair_count, "haversine_pairs", "json");

        u32 cluster_count = 0;

        if (distances_file && json_file) {
            fprintf(json_file, "{\"pairs\":[\n");
            f64 sum_coef = 1.0 / (f64)pair_count;

            f64 x_cluster = RandomInRange(&series, X_MIN, X_MAX);
            f64 y_cluster = RandomInRange(&series, Y_MIN, Y_MAX);

            for (u32 i = 0; i < pair_count; i++) {
                if (i % pairs_per_cluster == 0) {
                    cluster_count++;
                    x_cluster = RandomInRange(&series, X_MIN, X_MAX);
                    y_cluster = RandomInRange(&series, Y_MIN, Y_MAX);
                    //printf("Adding a new cluster. x: %lf, y: %lf, cluster_count: %u\n", x_cluster, y_cluster, cluster_count);
                }

                Haversine_Pair pair = generate_haversine_pair(&series, seed, x_cluster, y_cluster);
                f64 distance_result = haversine_distance(&pair);

                sum += sum_coef*distance_result;
                fwrite(&distance_result, sizeof(f64), 1, distances_file);

                char *json_sep = (i == (pair_count - 1)) ? "\n" : ",\n";
                fprintf(json_file, "    {\"x0\":%.16f, \"y0\":%.16f, \"x1\":%.16f, \"y1\":%.16f}%s", pair.x0, pair.y0, pair.x1, pair.y1, json_sep);
            }

            fprintf(json_file, "]}\n");
        } else {
            program_failed = true;
        }

        if (distances_file) fclose(distances_file);
        if (json_file) fclose(json_file);
    } else {
        program_failed = true;
    }

    if (!program_failed) {
        printf("Successfully generated json haversine pairs and binary distances\n");
        printf("Random seed: %lu\n", seed);
        printf("Method: %s\n", method_name);
        printf("Pair count: %lu\n", pair_count);
        printf("Expected sum: %lf\n", sum);
    } else {
        printf("Program failed to execute.\n");
    }

    return 0;
}

static FILE *Open(long long unsigned PairCount, char const *Label, char const *Extension)
{
    char Temp[256];
    sprintf(Temp, "data/data_%llu_%s.%s", PairCount, Label, Extension);
    FILE *Result = fopen(Temp, "wb");
    if(!Result)
    {
        fprintf(stderr, "Unable to open \"%s\" for writing.\n", Temp);
    }

    return Result;
}

