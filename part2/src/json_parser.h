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

typedef struct Json_Element Json_Element;
struct Json_Element {
    Buffer key;
    Buffer value;
    Json_Element *sub_child;
    Json_Element *next_sibling;
};

u64 parse_haversine_pairs(Buffer input_json, Haversine_Pair *pairs);

Json_Element *parse_json(Buffer input_json);
void free_json(Json_Element *element);

Json_Token get_json_token(Parser *parser);

Json_Element *parse_json_list(Parser *parser, Json_Token starting_token, Json_Token_Type end_type, b32 has_key);
Json_Element *parse_json_element(Parser *parser, Buffer key, Json_Token value);
Json_Element *lookup_json_element(Json_Element *object, Buffer element_name);

f64 convert_json_sign(Buffer source, u64 *at_result);
f64 convert_json_number(Buffer source, u64 *at_result);
f64 convert_element_to_f64(Json_Element *element, Buffer element_name);

void parse_keyword(Buffer source, u64 *at, Json_Token_Type type, Buffer keyword, Json_Token *result);
Json_Token_Type check_token_type(Buffer *source, u32 at);

void error(Parser *parser, Json_Token token, char const *message);
b32 is_json_digit(Buffer source, u64 at);
b32 is_parsing(Parser *parser);
b32 is_in_bounds(Buffer source, u64 at);
b32 is_json_whitespace(Buffer source, u64 at);

void test_parsing();
