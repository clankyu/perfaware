#pragma once

#include "util.h"
#include "buffer.h"
#include "haversine_generator.h"

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

typedef struct {
    Buffer source;
    u64 at;
    b32 had_error;
} Parser;

typedef struct {
    Json_Token_Type type;
    Buffer value;
} Json_Token;

typedef struct Json_Element {
    Buffer key;
    Buffer value;
    Json_Element *first_element;
    Json_Element *next_sibling;
} Json_Element;

u64 parse_haversine_pairs(Buffer *source, Haversine_Pair *pairs);
Json_Token_Type check_token_type(Buffer *source, u32 at);
b32 is_json_whitespace(Buffer source, u64 at);
