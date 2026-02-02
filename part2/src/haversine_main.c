#include <stdio.h>

#include "util.h"
#include "buffer.h"
#include "haversine_generator.h"
#include "json_parser.h"
#include "profiler.h"

int main(int argc, char** argv) {
    begin_profiler();
    printf("Starting haversine processor.\n");

    char *json_input_name;
    char *answers_name;

    {
        time_block("setup");
        if (argc == 3) {
            json_input_name = argv[1];
            answers_name = argv[2];
        } else {
            fprintf(stderr, "Error. Not enough arguments given. Example: ./run json_input.json answers.f64\n");
            return -1;
        }
    }

    Buffer json_input;
    Buffer parsed_values;

    {
        time_block("read");

        json_input = buffer_from_file(json_input_name);
        parsed_values = allocate_buffer(MAX_PAIRS * sizeof(Haversine_Pair));
    }

    f64 sum = 0;
    f64 average = 0;
    u64 pair_count = 0;

    if (parsed_values.count && json_input.count) {
        Haversine_Pair *pairs = (Haversine_Pair*)parsed_values.data;
        pair_count = parse_haversine_pairs(json_input, pairs);

        {
            time_block("sum");
            for (u32 i = 0; i < pair_count; i++) {
                Haversine_Pair pair = pairs[i];
                sum += haversine_distance(&pair);
            }

            average = sum / (f64)pair_count;
        }
    } else {
        fprintf(stderr, "Failed to create buffer for json input and/or parsed values.\nparsed_values.count = %u\njson_input.count = %u\n", parsed_values.count, json_input.count);
    }

    {
        time_block("output");
        printf("Sum: %lf\n", sum);
        printf("Average: %lf\n", average);
        printf("Pair count: %lu\n", pair_count);
    }
    printf("\n");

    end_profiler();

    profiler_stats();

    return 0;
}

