#include <stdio.h>

#include "util.h"
#include "buffer.h"
#include "haversine_generator.h"
#include "json_parser.h"
#include "os_performance_metrics.h"

int main(int argc, char** argv) {
    Profiler startup_profiler = init_profiler();
    Profiler read_profiler = init_profiler();
    Profiler parse_profiler = init_profiler();
    Profiler sum_profiler = init_profiler();
    Profiler output_profiler = init_profiler();
    Profiler program_profiler = init_profiler();

    profiler_start(&program_profiler);
    profiler_start(&startup_profiler);
    printf("Starting haversine processor.\n");

    char *json_input_name;
    char *answers_name;

    if (argc == 3) {
        json_input_name = argv[1];
        answers_name = argv[2];
    } else {
        fprintf(stderr, "Error. Not enough arguments given. Example: ./run json_input.json answers.f64\n");
        return -1;
    }
    profiler_end(&startup_profiler);

    //test_parsing();

    profiler_start(&read_profiler);
    Buffer json_input = buffer_from_file(json_input_name);
    Buffer parsed_values = allocate_buffer(MAX_PAIRS * sizeof(Haversine_Pair));
    profiler_end(&read_profiler);

    f64 sum = 0;
    f64 average = 0;
    u64 pair_count = 0;

    profiler_start(&parse_profiler);
    if (parsed_values.count && json_input.count) {
        Haversine_Pair *pairs = (Haversine_Pair*)parsed_values.data;
        pair_count = parse_haversine_pairs(json_input, pairs);
        profiler_end(&parse_profiler);

        profiler_start(&sum_profiler);
        for (u32 i = 0; i < pair_count; i++) {
            Haversine_Pair pair = pairs[i];
            sum += haversine_distance(&pair);
        }

        average = sum / (f64)pair_count;

        profiler_end(&sum_profiler);
    } else {
        fprintf(stderr, "Failed to create buffer for json input and/or parsed values.\nparsed_values.count = %u\njson_input.count = %u\n", parsed_values.count, json_input.count);
    }

    profiler_start(&output_profiler);
    printf("Sum: %lf\n", sum);
    printf("Average: %lf\n", average);
    printf("Pair count: %lu\n", pair_count);
    profiler_end(&output_profiler);

    profiler_end(&program_profiler);

    printf("\n");
    printf("Total time elapsed: %lf seconds\n", program_profiler.time_elapsed_seconds);
    print_profiler_stats(startup_profiler, "Startup", program_profiler.time_elapsed_seconds);
    print_profiler_stats(read_profiler, "Read", program_profiler.time_elapsed_seconds);
    print_profiler_stats(parse_profiler, "Parse", program_profiler.time_elapsed_seconds);
    print_profiler_stats(sum_profiler, "Sum", program_profiler.time_elapsed_seconds);
    print_profiler_stats(output_profiler, "Output", program_profiler.time_elapsed_seconds);
    print_profiler_stats(program_profiler, "Program", program_profiler.time_elapsed_seconds);

    return 0;
}

