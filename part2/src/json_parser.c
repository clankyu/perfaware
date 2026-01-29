#include <stdlib.h>
#include <math.h>
#include "json_parser.h"
#include "haversine_generator.h"

#include "util.h"

u64 parse_haversine_pairs(Buffer input_json, Haversine_Pair *pairs) {
    u64 pair_count = 0;

    Json_Element *parsed_json = parse_json(input_json);
    /*
    Json_Element *pairs_array = lookup_json_element(parsed_json, CONSTANT_STRING("pairs"));
    if (pairs_array) {
        for (Json_Element *element = pairs_array->sub_child;
            element && pair_count < MAX_PAIRS;
            element = element->next_sibling)
        {
            Haversine_Pair *pair = pairs + pair_count++;

            pair->x0 = convert_element_to_f64(element, CONSTANT_STRING("x0"));
            pair->y0 = convert_element_to_f64(element, CONSTANT_STRING("y0"));
            pair->x1 = convert_element_to_f64(element, CONSTANT_STRING("x1"));
            pair->y1 = convert_element_to_f64(element, CONSTANT_STRING("y1"));
        }
    }

    free_json(parsed_json);
*/

    return pair_count;
}

Json_Element *parse_json(Buffer input_json) {
    Parser parser = {0};
    parser.source = input_json;

    Json_Element *result = parse_json_element(&parser, (Buffer){0}, get_json_token(&parser));

    return result;
}

void free_json(Json_Element *element) {
    while (element) {
        Json_Element *free_element = element;
        element = free_element->next_sibling;

        free_json(free_element->sub_child);
        free(free_element);
    }
}

Json_Token get_json_token(Parser *parser) {
    Json_Token result;

    Buffer source = parser->source;
    u64 at = parser->at;

    while (is_json_whitespace(source, at)) {
        at++;
    }

    if (is_in_bounds(source, at)) {
        result.type = Token_error;
        result.value.count = 0;
        result.value.data = source.data + at;

        switch (parser->source.data[at]) {
            case '{': {result.type = Token_open_brace; at++;} break;
            case '[': {result.type = Token_open_bracket; at++;} break;
            case '}': {result.type = Token_close_brace; at++;} break;
            case ']': {result.type = Token_close_bracket; at++;} break;
            case ',': {result.type = Token_comma; at++;} break;
            case ':': {result.type = Token_colon; at++;} break;
            case ';': {result.type = Token_semi_colon; at++;} break;

            case 'f': {
                parse_keyword(source, &at, Token_false, CONSTANT_STRING("alse"), &result);
            } break;
            case 't': {
                parse_keyword(source, &at, Token_true, CONSTANT_STRING("rue"), &result);
            } break;
            case 'n': {
                parse_keyword(source, &at, Token_null, CONSTANT_STRING("ull"), &result);
            } break;

            case '"': {
                result.type = Token_string_literal;

                u64 string_start = at;
                at++;
                while (is_in_bounds(source, at) && source.data[at] != '"') {
                    if (is_in_bounds(source, at + 1) && source.data[at] == '\\' && source.data[at + 1] == '"') {
                        at++;
                    }

                    at++;
                }

                result.value.data = source.data + string_start;
                result.value.count = at - string_start;

                if (is_in_bounds(source, at)) {
                    at++;
                }
            } break;

            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                result.type = Token_number;
                u64 start = at;
                if (is_in_bounds(source, at) && source.data[at] == '-') { at++; }
                if (is_in_bounds(source, at) && source.data[at] != '0') {
                    while (is_json_digit(source, at)) {
                        at++;
                    }
                }

                if (is_in_bounds(source, at) && source.data[at] == '.') {
                    at++;
                    while (is_json_digit(source, at)) {
                        at++;
                    }
                }

                if (is_in_bounds(source, at) && ((source.data[at] == 'e') || (source.data[at] == 'E'))) {
                    at++;
                    if (is_in_bounds(source, at) && (source.data[at] == '+' || (source.data[at] == '-'))) {
                        at++;
                    }

                    while (is_json_digit(source, at)) {
                        at++;
                    }
                }

                result.value.count = at - start;
                printf("count for %c: %u\n", source.data[at], result.value.count);
            } break;

            default: {} break;
        }
    }

    parser->at = at;

    return result;
}

Json_Element *parse_json_element(Parser *parser, Buffer key, Json_Token value) {
    Json_Element *result = 0;
    Json_Element *sub_child = 0;
    b32 valid = true;

    if (value.type == Token_open_brace) {
        sub_child = parse_json_list(parser, value, Token_close_brace, true);
    } else if (value.type == Token_open_bracket) {
        sub_child = parse_json_list(parser, value, Token_close_bracket, false);
    } else if ((value.type == Token_string_literal) ||
            (value.type == Token_true) ||
            (value.type == Token_false) ||
            (value.type == Token_null) ||
            (value.type == Token_number))
    {
        // nothing
    } else {
        valid = false;
    }

    if (valid) {
        result = (Json_Element*)malloc(sizeof(Json_Element));
        result->sub_child = sub_child;
        result->key = key;
        result->value = value.value;
        result->next_sibling = 0;
    }

    return result;
}

Json_Element *parse_json_list(Parser *parser, Json_Token starting_token, Json_Token_Type end_type, b32 has_key) {
    Json_Element *first_element = 0;
    Json_Element *last_element = 0;

    while (is_parsing(parser)) {
        Buffer key = {0};
        Json_Token value = get_json_token(parser);
        if (has_key) {
            if (value.type == Token_string_literal) {
                key = value.value;
                Json_Token colon = get_json_token(parser);

                if (colon.type == Token_colon) {
                    value = get_json_token(parser);
                } else {
                    error(parser, colon, "Found unexepected token type while parsing. Expected Token_colon.");
                }
            } else if (value.type != end_type) {
                // NOTE: Error here
                error(parser, value, "Unexpected token in JSON");
            }
        }

        Json_Element *element = parse_json_element(parser, key, value);
        if (element) {
            if (last_element) {
                last_element->next_sibling = element;
                last_element = last_element->next_sibling;
            } else {
                first_element = element;
                last_element = element;
            }
        } else if (value.type == end_type) {
            break;
        } else {
            error(parser, value, "Unexpected token in JSON\n");
        }

        Json_Token comma = get_json_token(parser);
        if (comma.type == end_type) {
            break;
        } else if (comma.type != Token_comma) {
            if (!last_element->sub_child) {
                error(parser, comma, "Expected comma while parsing JSON\n");
            }
        }
    }

    return first_element;
}

Json_Element *lookup_json_element(Json_Element *object, Buffer element_name) {
    Json_Element *result = 0;

    if (object) {
        for (Json_Element *search = object->sub_child; search; search = search->next_sibling) {
            if (buffers_are_equal(search->key, element_name)) {
                result = search;
                break;
            }
        }
    }

    return result;
}

void parse_keyword(Buffer source, u64 *at, Json_Token_Type type, Buffer keyword, Json_Token *result) {
    Buffer check = source;

    check.data += *at;
    check.count = keyword.count;
    if (buffers_are_equal(check, keyword)) {
        result->type = type;
        result->value.count += check.count;
        *at += keyword.count;
    }
}

f64 convert_json_sign(Buffer source, u64 *at_result) {
    f64 result = 1.0f;
    u64 at = *at_result;

    if (is_in_bounds(source, at) && (source.data[at] == '-')) {
        result = -1.0f;
        at += 1;
    }

    *at_result = at;

    return result;
}

f64 convert_json_number(Buffer source, u64 *at_result) {
    f64 result = 0;
    u64 at = *at_result;

    while (is_in_bounds(source, at)) {
        u8 number = source.data[at] - (u8)'0';
        if (number < 10) {
            result = 10.0f*result + (f64)number;
            at++;
        } else {
            break;
        }
    }

    *at_result = at;

    return result;
}

f64 convert_element_to_f64(Json_Element *object, Buffer element_name) {
    f64 result = 0.0f;

    Json_Element *element = lookup_json_element(object, element_name);
    if(element) {
        u64 at = 0;
        f64 sign = convert_json_sign(element->value, &at);
        f64 number = convert_json_number(element->value, &at);
        f64 decimal = 0.0f;

        if (is_in_bounds(element->value, at) && element->value.data[at] == '.') {
            at++;
            for (u32 exponent = 1; is_in_bounds(element->value, at); at++) {
                u8 d = element->value.data[at] - (u8)'0';
                if (d < 10) {
                    decimal += d*pow(10, -exponent);
                    at++;
                } else {
                    break;
                }
            }
        }

        number += decimal;
        if(is_in_bounds(element->value, at) && (element->value.data[at] == 'e') ||(element->value.data[at] == 'E')) {
            at++;
            if (is_in_bounds(element->value, at) && (element->value.data[at] == '+')) {
                at++;
            }

            f64 exponent_sign = convert_json_sign(element->value, &at);
            f64 exponent = exponent_sign*convert_json_number(element->value, &at);

            number *= pow(10, exponent);
        }

        result = sign*number;
    }

    return result;
}

void error(Parser *parser, Json_Token token, char const *message) {
    parser->had_error = true;
    fprintf(stderr, "Parsing error. Message: %s\n", message);

    char error_char = parser->source.data[parser->at];
    char error_char_prev = parser->source.data[parser->at-1];
    char error_char_after = parser->source.data[parser->at+1];

    printf("Before error char: %c\n", error_char_prev);
    printf("Error char: %c\n", error_char);
    printf("After error char: %c\n", error_char_after);
    fprintf(stderr, "Debug info:\ntoken type: %i\nparser->source.data[%lu]: %c\nparser->source.count: %u\n", token.type, parser->at, parser->source.data[parser->at], parser->source.count);
    //fprintf(stderr, "Parsing error: %s\n - Count: %u, Data: %s", message, (u32) token.value.count, (char*) token.value.data);
}

b32 is_json_digit(Buffer source, u64 at) {
    b32 result = false;;

    if (is_in_bounds(source, at)) {
        u8 val = source.data[at];
        result = ((val >= '0') && (val <= '9'));
    }

    return result;
}

b32 is_parsing(Parser *parser) {
    b32 result = (!parser->had_error && is_in_bounds(parser->source, parser->at)) ? true : false;
    if (!parser->had_error && is_in_bounds(parser->source, parser->at)) {
        return true;
    } else {
        return false;
    }
}

b32 is_in_bounds(Buffer source, u64 at) {
    return (at < source.count) ? true : false;
}

b32 is_json_whitespace(Buffer source, u64 at) {
     b32 result = false;
    if (is_in_bounds(source, at)) {
        u8 val = source.data[at];
        result = ((val == ' ') || (val == '\t') || (val == '\n') || (val == '\r'));
    }

    return result;
}

void test_parsing() {
    Buffer test_string = CONSTANT_STRING("true");
    u64 at = 0;

    Json_Token test_token;
    test_token.value.data = test_string.data + at;
    test_token.value.count = 0;

    parse_keyword(test_string, &at, Token_true, CONSTANT_STRING("true"), &test_token);

    if(test_token.value.count) {
        printf("Test parse results:\n");
        printf("Parse result: %.*s\n", test_token.value.count, test_token.value.data);
    } else {
        fprintf(stderr, "Failed to parse test token.\n");
    }
}
