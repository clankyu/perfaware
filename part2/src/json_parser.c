#include "json_parser.h"
#include "haversine_generator.h"

#include "util.h"

u64 parse_haversine_pairs(Buffer *source, Haversine_Pair *pairs) {
    u64 result = 0;

    for (u32 at = 0; at < source->count; at++) {
        switch (source->data[at]) {
            case '{': {
                Json_Value value = {0};
                value.type = Value_object;
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

b32 is_json_whitespace(Buffer source, u64 at) {

}
