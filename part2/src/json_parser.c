#include "json_parser.h"

#include "util.h"

Buffer parse_haversine_pairs_from_json(Buffer *source) {
    Buffer result = allocate_buffer(HAVERSINE_PAIR_BUFFER_SIZE);

    for (u32 at = 0; at < source->count; at++) {
        switch (source->data[at]) {
            case '{': {
                Json_Value value = {0};
                value.type = Value_object;
                for (;;) {

                }
            }
        }
    }

    return result;
}

Json_Value get_value_from_key(Buffer *source, u32 at, char const *key) {
    Json_Value result = {0};


    return result;
}

Json_Token_Type check_token_type(Buffer *source, u32 at) {
    return Token_error;
}
