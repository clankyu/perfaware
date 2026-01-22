#include "util.h"
#include "buffer.h"

enum Json_Token_Type {
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
};

enum Json_Value_Type {
    Value_object,
    Value_array,
    Value_string,
    Value_number,
    Value_true,
    Value_false,
    Value_null
};

Json_Token_Type check_token_type(Buffer *source, u32 at);
