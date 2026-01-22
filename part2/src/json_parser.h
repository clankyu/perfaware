#pragma once

#include "util.h"
#include "buffer.h"

#define HAVERSINE_PAIR_BUFFER_SIZE 1000000

typedef enum {
    Token_end_of_stream,
    Token_error,

    Token_open_brace,
    Token_open_bracket,
    Token_close_brace,
    Token_close_bracket,
    Token_comma,
    Token_colon,
    Token_semi_colon,
    Token_string_literal,
    Token_number,
    Token_true,
    Token_false,
    Token_null,

    Token_count,
} Json_Token_Type;

typedef enum {
    Value_object,
    Value_array,
    Value_string,
    Value_number,
    Value_true,
    Value_false,
    Value_null
} Json_Value_Type;

typedef enum {
    Number_integer,
    Number_floating_point,
    Number_scientific_notation
} Json_Number_Type;

typedef enum {
    Sign_Positive,
    Sign_Negative
} Json_Sign;

typedef struct Json_Value Json_Value;

typedef struct {
    char *string;
    Json_Value *value;
} Json_Pair;

typedef struct {
    Json_Pair *pairs;
    u32 count;
} Json_Object;

typedef struct {
    Json_Value *values;
    u32 count;
} Json_Array;

typedef struct {
    bool boolean;
} Json_Boolean;

typedef struct {} Json_Null;

struct Json_Value {
    Json_Value_Type type;
    union {
        Json_Object object;
        Json_Array array;
        char *string;
        f64 number;
        Json_Boolean boolean;
        Json_Null null;
    };
};

Buffer parse_haversine_pairs_from_json(Buffer *source);
Json_Value get_value_from_key(Buffer *source, u32 at, char const *key);
Json_Token_Type check_token_type(Buffer *source, u32 at);
