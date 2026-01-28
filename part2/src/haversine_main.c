#include <stdio.h>

#include "util.h"
#include "buffer.h"
#include "haversine_generator.h"
#include "json_parser.h"

int main(int argc, char** argv) {
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

    test_parsing();

    Buffer json_input = buffer_from_file(json_input_name);
    Buffer parsed_values = allocate_buffer(MAX_PAIRS * sizeof(Haversine_Pair));

    if (parsed_values.count && json_input.count) {
        Haversine_Pair *pairs = (Haversine_Pair*)parsed_values.data;
        u64 pair_count = parse_haversine_pairs(&json_input, pairs);
    } else {
        fprintf(stderr, "Failed to create buffer for json input and/or parsed values.\nparsed_values.count = %u\njson_input.count = %u\n", parsed_values.count, json_input.count);
    }

    return 0;
}

