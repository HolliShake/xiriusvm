#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <ctype.h>
#include <uchar.h>

#if defined(_WIN32) || defined(_WIN64)
    #define OS_WINDOWS 1
    #define OS_MACOS 0
    #define OS_LINUX 0
    #define OS_BSD 0
#elif defined(__APPLE__)
    #define OS_WINDOWS 0
    #define OS_MACOS 1
    #define OS_LINUX 0
    #define OS_BSD 0
#elif defined(__linux__)
    #define OS_WINDOWS 0
    #define OS_MACOS 0
    #define OS_LINUX 1
    #define OS_BSD 0
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    #define OS_WINDOWS 0
    #define OS_MACOS 0
    #define OS_LINUX 0
    #define OS_BSD 1
#endif


#define ERROR(msg) {\
    fprintf(stderr, "%s::%s:[%d]: %s\n", __FILE__, __func__, __LINE__, msg);\
    exit(1);\
}\

#define ERROR_F(fpath, position, msg, ...) {\
    fprintf(stderr, "\033[31mdebug(%s::%s:[%d])\033[0m: [%s:%lld:%lld]: %s\n", __FILE__, __func__, __LINE__, fpath, position->start_line, position->start_colm, xirius_str_format(msg, __VA_ARGS__));\
    exit(1);\
}\

char* xirius_str_new(const char* str0) {
    size_t len = strlen(str0);
    char* result = malloc(len + 1);
    if (result == NULL)
        ERROR("failed to allocate memory for string!!!");

    strcpy(result, str0);
    return result;
}

char* xirius_str_copy(char* str0) {
    size_t len = strlen(str0);
    char* result = malloc(len + 1);
    if (result == NULL)
        ERROR("failed to allocate memory for string!!!");

    strcpy(result, str0);
    return result;
}

char* xirius_str_add(char* str0, char* str1) {
    size_t len0 = strlen(str0);
    size_t len1 = strlen(str1);
    char* result = malloc(len0 + len1 + 1);
    if (result == NULL)
        ERROR("failed to allocate memory for string!!!");

    strcpy(result, str0);
    strcat(result, str1);
    return result;
}

char* xirius_str_format(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char* buffer = malloc(1024 * sizeof(char));
    if (buffer == NULL)
        ERROR("failed to allocate memory for string!!!");

    vsprintf(buffer, format, args);
    va_end(args);
    return buffer;
}

bool xirius_str_equals(const char* str0, const char* str1) {
    return strcmp(str0, str1) == 0;
}

char* xirius_read_file(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL)
        ERROR("failed to open file!!!");

    char c, *buffer = xirius_str_new(""), *old = NULL;

    while ((c = fgetc(file)) != EOF) {
        buffer = xirius_str_add(old = buffer, (char[]) {c, '\0'});
        free(old);
    }
    
    return buffer;
}


#ifndef UTF8
#define UTF8

/*
_BYTE1 = 0b00000000,
    _BYTE2 = 0b11000000,
    _BYTE3 = 0b11100000,
    _BYTE4 = 0b11110000,

    _2BYTE_FOLLOW = 0b00011111,
    _3BYTE_FOLLOW = 0b00001111,
    _4BYTE_FOLLOW = 0b00000111,

    _VALID_TRAILING = 0b10000000,
    _MAX_TRAILING   = 0b00111111

*/

#define _BYTE1 0b10000000
#define _BYTE2 0b11000000
#define _BYTE3 0b11100000
#define _BYTE4 0b11110000

#define _2BYTE_FOLLOW 0b00011111
#define _3BYTE_FOLLOW 0b00001111
#define _4BYTE_FOLLOW 0b00000111

#define _MAX_TRAILING 0b00111111

static
int utf_size_of_utf(unsigned char firstByte) {
    if ((firstByte & _BYTE4) == _BYTE4)
        return 4;
    else if ((firstByte & _BYTE3) == _BYTE3)
        return 3;
    else if ((firstByte & _BYTE2) == _BYTE2)
        return 2;
    else if ((firstByte & _BYTE1) == 0)
        return 1;
    return 0;
}

static
int utf_size_of_codepoint(int codePoint) {
    if (codePoint < 0x80)
        return 1;
    else if (codePoint < 0x000800)
        return 2;
    else if (codePoint < 0x010000)
        return 3;
    else if (codePoint < 0x110000)
        return 4;
    return 0;
}

static
char* utf_codepoint_to_string(int codepoint) {
    char* utf8_str = malloc(utf_size_of_codepoint(codepoint) + 1);
    if (codepoint <= 0x7F) {
        // 1-byte UTF-8
        utf8_str[0] = codepoint;
        utf8_str[1] = '\0';
    } else if (codepoint <= 0x7FF) {
        // 2-byte UTF-8
        utf8_str[0] = 0xC0 | (codepoint >> 6);
        utf8_str[1] = 0x80 | (codepoint & 0x3F);
        utf8_str[2] = '\0';
    } else if (codepoint <= 0xFFFF) {
        // 3-byte UTF-8
        utf8_str[0] = 0xE0 | (codepoint >> 12);
        utf8_str[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        utf8_str[2] = 0x80 | (codepoint & 0x3F);
        utf8_str[3] = '\0';
    } else if (codepoint <= 0x10FFFF) {
        // 4-byte UTF-8
        utf8_str[0] = 0xF0 | (codepoint >> 18);
        utf8_str[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        utf8_str[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        utf8_str[3] = 0x80 | (codepoint & 0x3F);
        utf8_str[4] = '\0';
    } else {
        // Invalid codepoint
        utf8_str[0] = '\0';
    }
    return utf8_str;
}

static
int utf_to_codepoint(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4) {
    int ord = 0;
    switch (utf_size_of_utf(b1))
    {
        case 1:
            return b1;
            break;
        case 2:
            ord  = ((b1 & _2BYTE_FOLLOW) << 6);
            ord |= ((b2 & _MAX_TRAILING));
            break;
        case 3:
            ord  = ((b1 & _3BYTE_FOLLOW) << 12);
            ord |= ((b2 & _MAX_TRAILING) <<  6);
            ord |= ((b3 & _MAX_TRAILING));
            break;
        case 4:
            ord  = ((b1 & _4BYTE_FOLLOW) << 18);
            ord |= ((b2 & _MAX_TRAILING) << 12);
            ord |= ((b3 & _MAX_TRAILING) <<  6);
            ord |= ((b4 & _MAX_TRAILING));
            break;
        default:
            break;
    }
    
    return ord;
}

static
bool utf_is_letter(int codepoint) {
    return isalpha(codepoint);
}

static
bool utf_is_number(int codepoint) {
    return isdigit(codepoint);
}

#endif

#ifndef POSITION_H
#define POSITION_H
    typedef struct xirius_position_struct {
        size_t start_line;
        size_t start_colm;
        size_t end_line;
        size_t end_colm;
    } position_t;

    position_t* position_new(size_t start_line, size_t start_colm) {
        position_t* position = malloc(sizeof(position_t));
        if (position == NULL)
            ERROR("failed to allocate memory for position!!!");

        position->start_line = start_line;
        position->start_colm = start_colm;
        position->end_line   = start_line;
        position->end_colm   = start_colm;
        return position;
    }

    position_t* position_merge(position_t* start, position_t* end) {
        position_t* position = malloc(sizeof(position_t));
        if (position == NULL)
            ERROR("failed to allocate memory for position!!!");

        position->start_line = start->start_line;
        position->start_colm = start->start_colm;
        position->end_line   = end->end_line;
        position->end_colm   = start->start_colm;
        return position;
    }
#endif

#ifndef TOKEN_H
#define TOKEN_H
    typedef enum token_type {
        TOKEN_IDN,
        TOKEN_INT,
        TOKEN_FLT,
        TOKEN_STR,
        TOKEN_SYM,
        TOKEN_EOF
    } token_type_t;

    typedef struct xirius_token_struct {
        token_type_t type;
        char* value;
        position_t* position;
    } token_t;
    
    token_t* token_new(token_type_t type, char* value, position_t* position) {
        token_t* token = malloc(sizeof(token_t));
        if (token == NULL)
            ERROR("failed to allocate memory for token!!!");

        token->type = type;
        token->value = value;
        token->position = position;
        return token;
    }
#endif

#ifndef LEXER_H
#define LEXER_H
    typedef struct xirius_lexer_struct {
        char* path;
        char* data;
        size_t index;
        size_t line;
        size_t colm;
    } lexer_t;

    static
    lexer_t* lexer_new(const char* path, const char* data) {
        lexer_t* lexer = malloc(sizeof(lexer_t));
        if (lexer == NULL)
            ERROR("failed to allocate memory for lexer!!!");

        lexer->path = (char*) path;
        lexer->data = (char*) data;
        lexer->index = 0;
        lexer->line = 1;
        lexer->colm = 1;
        return lexer;
    }

    static
    int lexer_codepoint(lexer_t* lexer) {
        int codePoint = 0;
        int size = utf_size_of_utf((unsigned char) lexer->data[lexer->index]);

        if (size == 1)
            codePoint = utf_to_codepoint(
                lexer->data[lexer->index], 
                0, 0, 0
            );
        else if (size == 2)
            codePoint = utf_to_codepoint(
                lexer->data[lexer->index + 0], 
                lexer->data[lexer->index + 1], 
                0, 0
            );
        else if (size == 3)
            codePoint = utf_to_codepoint(
                lexer->data[lexer->index + 0], 
                lexer->data[lexer->index + 1], 
                lexer->data[lexer->index + 2], 
                0
            );
        else if (size == 4)
            codePoint = utf_to_codepoint(
                lexer->data[lexer->index + 0], 
                lexer->data[lexer->index + 1], 
                lexer->data[lexer->index + 2], 
                lexer->data[lexer->index + 3]
            );
        else {
            ERROR("invalid utf-8 character!!!");
        }
        return codePoint;
    }

    static
    char* lexer_lookahead(lexer_t* lexer) {
        return utf_codepoint_to_string(lexer_codepoint(lexer));
    }

    static
    void lexer_forward(lexer_t* lexer) {
        if (lexer->index >= strlen(lexer->data))
            return;

        int codepoint = lexer_codepoint(lexer);

        if (codepoint == '\n') {
            lexer->line++;
            lexer->colm = 1;
        } else {
            lexer->colm++;
        }

        lexer->index += utf_size_of_codepoint(codepoint);
    }

    static
    bool lexer_is_eof(lexer_t* lexer) {
        return lexer->index >= strlen(lexer->data);
    }

    static
    bool lexer_is_whitespace(lexer_t* lexer) {
        int c = lexer_codepoint(lexer);
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            return true;
        }

        return iswspace(c);
    }

    static
    bool lexer_is_identifier(lexer_t* lexer) {
        // does not support utf-8
        int c = lexer_codepoint(lexer);
        if ((c == '_' ) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            return true;
        }

        return utf_is_letter(c);
    }

    static
    bool lexer_is_digit(lexer_t* lexer) {
        int c = lexer_codepoint(lexer);
        if (c >= '0' && c <= '9') {
            return true;
        }

        return utf_is_number(c);
    }

    static
    bool lexer_is_string(lexer_t* lexer) {
        return lexer_codepoint(lexer) == '"';
    }

    static
    token_t* lexer_next_id(lexer_t* lexer) {
        size_t start_line = lexer->line, start_colm = lexer->colm;
        char* value = xirius_str_new(""), *old = NULL;
        // 
        while (!lexer_is_eof(lexer) && (lexer_is_identifier(lexer) || (strlen(value) > 0 && lexer_is_digit(lexer)))) {
            char* look = lexer_lookahead(lexer);
            value = xirius_str_add(old = value, look);
            free(old); free(look);
            lexer_forward(lexer);
        }

        return token_new(
            TOKEN_IDN, 
            value, 
            position_new(start_line, start_colm)
        );
    }

    static
    token_t* lexer_next_number(lexer_t* lexer) {
        size_t start_line = lexer->line, start_colm = lexer->colm;
        char* value = xirius_str_new(""), *old = NULL, *look = NULL;
        // does not support hex or other form of numbers etc: 0xff, 0b1010, 0o1234, 2e2, 2e+2.2
        while (!lexer_is_eof(lexer) && lexer_is_digit(lexer)) {
            value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
            free(old); free(look);
            lexer_forward(lexer);
        }

        if (lexer_codepoint(lexer) == '.') {
            value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
            free(old); free(look);
            lexer_forward(lexer);

            if (!lexer_is_digit(lexer))
                ERROR_F(lexer->path, position_new(lexer->line, lexer->colm), "expected a number!", NULL);

            while (!lexer_is_eof(lexer) && lexer_is_digit(lexer)) {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);
            }

            return token_new(
                TOKEN_FLT, 
                value, 
                position_new(start_line, start_colm)
            );
        }

        return token_new(
            TOKEN_INT, 
            value, 
            position_new(start_line, start_colm)
        );
    }

    static
    token_t* lexer_next_string(lexer_t* lexer) {
        size_t start_line = lexer->line, start_colm = lexer->colm;
        char* value = xirius_str_new(""), *old = NULL, *look = NULL;

        bool open = lexer_is_string(lexer), close = false;
        lexer_forward(lexer);
        close = lexer_is_string(lexer);

        while ((!lexer_is_eof(lexer)) && (open != close)) {
            if (lexer_codepoint(lexer) == '\n') {
                break;
            }

            if (lexer_codepoint(lexer) == '\\') {
                lexer_forward(lexer);

                switch (lexer_codepoint(lexer)) {
                    case 'b':
                        value = xirius_str_add(old = value, "\b");
                        break;
                    case 'n':
                        if (OS_WINDOWS || OS_MACOS)
                            value = xirius_str_add(old = value, "\r\n");
                        else 
                            value = xirius_str_add(old = value, "\n");
                        break;
                    case 'r':
                        value = xirius_str_add(old = value, "\r");
                        break;
                    case 't':
                        value = xirius_str_add(old = value, "\t");
                        break;
                    case '\"':
                        value = xirius_str_add(old = value, "\"");
                        break;
                    case '\'':
                        value = xirius_str_add(old = value, "\'");
                        break;
                    case '\\':
                        value = xirius_str_add(old = value, "\\\\");
                        break;
                    default:
                        ERROR_F(lexer->path, position_new(start_line, start_colm), "unexpected escape character \"\\%s\"!", lexer_lookahead(lexer));
                }
            } else {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
            }

            lexer_forward(lexer);
            close = lexer_is_string(lexer);
        }

        if (open != close)
            ERROR_F(lexer->path, position_new(start_line, start_colm), "expected a closing string!", NULL);

        lexer_forward(lexer);

        return token_new(
            TOKEN_STR, 
            value, 
            position_new(start_line, start_colm)
        );
    }

    static
    bool lexer_is_symbol_start(char c) {
        switch (c) {
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
            case '.':
            case ',':
            case ':':
            case ';':
            case '*':
            case '%':
            case '/':
            case '+':
            case '-':
            case '<':
            case '>':
            case '!':
            case '=':
            case '&':
            case '|':
            case '^':
                return true;
            
            default:
                return false;
        }
    }

    static
    token_t* lexer_next_symbol(lexer_t* lexer) {
        size_t start_line = lexer->line, start_colm = lexer->colm;
        char* value = xirius_str_new(""), *old = NULL, *look = NULL;
        

        char c = lexer_codepoint(lexer);
        switch (c) {
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
            case '.':
            case ',':
            case ':':
            case ';': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);
                break;
            }
            case '*': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer) == '=') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            case '/': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer)== '/' || lexer_codepoint(lexer) == '*') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            case '%': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer) == '=') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            case '+': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer) == '+' || lexer_codepoint(lexer) == '=') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            case '-': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer) == '-' || lexer_codepoint(lexer) == '=') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            case '<': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer) == '<' || lexer_codepoint(lexer) == '=') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            case '>': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer) == '>' || lexer_codepoint(lexer) == '=') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            case '=': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer) == '=') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            case '!': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer) == '=') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            case '&': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer) == '&') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            case '|': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer) == '|') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            case '^': {
                value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                free(old); free(look);
                lexer_forward(lexer);

                if (lexer_codepoint(lexer) == '^') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            default: {
                if (lexer_codepoint(lexer) > 0x80) {
                    while (!lexer_is_eof(lexer) && (lexer_codepoint(lexer) > 0x80)) {
                        value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                        free(old); free(look);
                        lexer_forward(lexer);
                    }
                    ERROR_F(lexer->path, position_new(start_line, start_colm), "unexpected symbol \"%s\"!", value);
                } else {
                    while (!lexer_is_eof(lexer) && !lexer_is_symbol_start(lexer_codepoint(lexer))) {
                        value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                        free(old); free(look);
                        lexer_forward(lexer);
                    }
                    ERROR_F(lexer->path, position_new(start_line, start_colm), "unexpected symbol \"%s\"!", value);
                }
            }
        }

        return token_new(
            TOKEN_SYM, 
            value, 
            position_new(start_line, start_colm)
        );
    }

    static
    token_t* lexer_get_next(lexer_t* lexer) {
        while (!lexer_is_eof(lexer)) {
            if (lexer_is_whitespace(lexer)) {
                lexer_forward(lexer);
                continue;
            } else if (lexer_is_identifier(lexer)) {
                return lexer_next_id(lexer);
            } else if (lexer_is_digit(lexer)) {
                return lexer_next_number(lexer);
            } else if (lexer_is_string(lexer)) {
                return lexer_next_string(lexer);
            } else {
                return lexer_next_symbol(lexer);
            }
        }

        return token_new(
            TOKEN_EOF, 
            xirius_str_new("EOF"), 
            position_new(lexer->line, lexer->colm)
        );
    }

    void lexer_dump(lexer_t* lexer) {
        token_t* token = NULL;
        do {
            token = lexer_get_next(lexer);
            printf("TOKEN: %d \"%s\"\n", token->type, token->value);
        } while (token->type != TOKEN_EOF);
    }

    void lexer_free(lexer_t* lexer) {
        free(lexer);
    }

#endif

#ifndef AST_H
#define AST_H
    typedef enum ast_type {
        AST_IDN,
        AST_INT,
        AST_FLT,
        AST_STR,
        AST_BOOL,
        AST_NULL,
        AST_ARRAY,
        // 
        AST_UNARY_PLUSPLUS,
        AST_UNARY_MINUSMINUS,
        // 
        AST_MAP,
        AST_INDEX,
        AST_CALL,
        AST_BIN_MUL,
        AST_BIN_DIV,
        AST_BIN_MOD,
        AST_BIN_ADD,
        AST_BIN_SUB,
        AST_BIN_LSHFT,
        AST_BIN_RSHFT,
        AST_CMP_LT,
        AST_CMP_LTE,
        AST_CMP_GT,
        AST_CMP_GTE,
        AST_CMP_EQ,
        AST_CMP_NEQ,
        AST_BIT_AND,
        AST_BIT_OR,
        AST_BIT_XOR,
        AST_LOG_AND,
        AST_LOG_OR,
        AST_ASSIGN,
        // 
        AST_FUNCTION,
        AST_VAR,
        AST_CONST,
        AST_LOCAL,
        AST_WHILE,
        AST_IF,
        AST_BLOCK,
        AST_EXPR_STMNT,
        AST_PROGRAM
    } ast_type_t;

    typedef struct ast_struct ast_t;
    typedef struct ast_struct {
        ast_type_t type;
        // 
        char* str_0;
        char* str_1;
        ast_t* data_0;
        ast_t* data_1;
        ast_t* data_2;
        ast_t** multi_0;
        ast_t** multi_1;
        // 
        position_t* position;
    } ast_t;

    static
    ast_t* ast_init(ast_type_t type, position_t* position) {
        ast_t* ast = malloc(sizeof(ast_t));
        if (ast == NULL)
            ERROR("failed to allocate memory for ast!!!");

        ast->type = type;
        ast->str_0 = NULL;
        ast->str_1 = NULL;
        // 
        ast->data_0 = NULL;
        ast->data_1 = NULL;
        ast->data_2 = NULL;
        // 
        ast->multi_0 = NULL;
        ast->multi_1 = NULL;
        // 
        ast->position = position;
        return ast;
    }

    static
    ast_t* ast_terminal(ast_type_t type, char* value, position_t* pos) {
        ast_t* ast = ast_init(type, pos);
        ast->str_0 = value;
        return ast;
    }

    static
    ast_t* ast_map(ast_t** keys, ast_t** values, position_t* pos) {
        ast_t* ast = ast_init(AST_MAP, pos);
        ast->multi_0 = keys;
        ast->multi_1 = values;
        return ast;
    }

    static
    ast_t* ast_index_expression(ast_t* object, ast_t* index, position_t* pos) {
        ast_t* ast = ast_init(AST_INDEX, pos);
        ast->data_0 = object;
        ast->data_1 = index;
        return ast;
    }

    static
    ast_t* ast_call_expression(ast_t* callee, ast_t** args, position_t* pos) {
        ast_t* ast = ast_init(AST_CALL, pos);
        ast->data_0 = callee;
        ast->multi_0 = args;
        return ast;
    }

    static
    ast_t* ast_unary_expression(ast_type_t type, char* op, ast_t* lhs, position_t* pos) {
        ast_t* ast = ast_init(type, pos);
        ast->str_0 = op;
        ast->data_0 = lhs;
        return ast;
    }

    static
    ast_t* ast_binary_expression(ast_type_t type, char* op, ast_t* lhs, ast_t* rhs, position_t* pos) {
        ast_t* ast = ast_init(type, pos);
        ast->str_0 = op;
        ast->data_0 = lhs;
        ast->data_1 = rhs;
        return ast;
    }

    static
    ast_t* ast_function_declaration(ast_t* name, ast_t** params, ast_t** body, position_t* pos) {
        ast_t* ast = ast_init(AST_FUNCTION, pos);
        ast->data_0 = name;
        ast->multi_0 = params;
        ast->multi_1 = body;
        return ast;
    }

    static
    ast_t* ast_var_decl(ast_t** names, ast_t** values, position_t* pos) {
        ast_t* ast = ast_init(AST_VAR, pos);
        ast->multi_0 = names;
        ast->multi_1 = values;
        return ast;
    }

    static
    ast_t* ast_const_decl(ast_t** names, ast_t** values, position_t* pos) {
        ast_t* ast = ast_init(AST_CONST, pos);
        ast->multi_0 = names;
        ast->multi_1 = values;
        return ast;
    }

    static
    ast_t* ast_local_decl(ast_t** names, ast_t** values, position_t* pos) {
        ast_t* ast = ast_init(AST_LOCAL, pos);
        ast->multi_0 = names;
        ast->multi_1 = values;
        return ast;
    }

    static
    ast_t* ast_while_statement(ast_t* condition, ast_t* body, position_t* pos) {
        ast_t* ast = ast_init(AST_WHILE, pos);
        ast->data_0 = condition;
        ast->data_1 = body;
        return ast;
    }

    static
    ast_t* ast_if_statement(ast_t* condition, ast_t* thenv, ast_t* elsev, position_t* pos) {
        ast_t* ast = ast_init(AST_IF, pos);
        ast->data_0 = condition;
        ast->data_1 = thenv;
        ast->data_2 = elsev;
        return ast;
    }

    static
    ast_t* ast_block(ast_t** statements, position_t* pos) {
        ast_t* ast = ast_init(AST_BLOCK, pos);
        ast->multi_0 = statements;
        return ast;
    }

    static
    ast_t* ast_expr_stmnt(ast_t* expr, position_t* pos) {
        ast_t* ast = ast_init(AST_EXPR_STMNT, pos);
        ast->data_0 = expr;
        return ast;
    }

    static
    ast_t* ast_program(ast_t** statements, position_t* pos) {
        ast_t* ast = ast_init(AST_PROGRAM, pos);
        ast->multi_0 = statements;
        return ast;
    }

    #define INIT_ARRAY(label) ast_t** label = malloc(sizeof(ast_t*)); size_t label##_size = 0; label[0] = NULL;
    #define PUSH_ARRAY(label, value) {\
        label[label##_size++] = value;\
        label = realloc(label, sizeof(ast_t*) * (label##_size + 1));\
        label[label##_size] = NULL;\
    }\

#endif

#ifndef PARSER_H
#define PARSER_H
    typedef struct xirius_parser_struct {
        lexer_t* lexer;
        token_t* lookahead;
        token_t* previous;
    } parser_t;

    static
    parser_t* parser_new(const char* path, const char* data) {
        parser_t* parser = malloc(sizeof(parser_t));
        if (parser == NULL)
            ERROR("failed to allocate memory for parser!!!");

        parser->lexer = lexer_new(path, data);
        parser->lookahead = parser->previous = NULL;
        return parser;
    }

    #define CHECKTTYPE(ttype) parser_check(parser, ttype, NULL, true)
    #define CHECKVALUE(value) parser_check(parser,    -1, value, false)
    #define KEYWORD(id) parser_check(parser, TOKEN_IDN, id, false)

    static
    bool parser_check(parser_t* parser, token_type_t type, const char* value, bool is_type) {
        if (is_type) {
            return (parser->lookahead->type == type);
        } else {
            return ((parser->lookahead->type == TOKEN_IDN) || (parser->lookahead->type == TOKEN_SYM)) &&
            xirius_str_equals(parser->lookahead->value, value);
        }
    }

    #define ACCPETTTYPE(ttype) parser_accept(parser, ttype, NULL, true)
    #define ACCPETVALUE(value) parser_accept(parser,    -1, value, false)

    static
    void parser_accept(parser_t* parser, token_type_t type, const char* value, bool is_type) {
        if (parser_check(parser, type, value, is_type)) {
            parser->previous = parser->lookahead;
            parser->lookahead = lexer_get_next(parser->lexer);
            return;
        }

        char* vv = (value != NULL) ? (char*) value : (
            (type == TOKEN_IDN) ? "identifier" :
            (type == TOKEN_INT) ?   "integer"  :
            (type == TOKEN_FLT) ?    "float"   :
            (type == TOKEN_STR) ?    "string"  :
            (type == TOKEN_SYM) ?    "symbol"  :
            "unknown"
        );

        ERROR_F(parser->lexer->path, parser->lookahead->position, "expected \"%s\" but got \"%s\"!", vv, parser->lookahead->value);
    }

    static
    ast_t* parser_terminal(parser_t* parser) {
        if (CHECKTTYPE(TOKEN_INT)) {
            ast_t* t = ast_terminal(AST_INT, parser->lookahead->value, parser->lookahead->position);
            ACCPETTTYPE(TOKEN_INT);
            return t;
        } else if (CHECKTTYPE(TOKEN_FLT)) {
            ast_t* t = ast_terminal(AST_FLT, parser->lookahead->value, parser->lookahead->position);
            ACCPETTTYPE(TOKEN_FLT);
            return t;
        } else if (CHECKTTYPE(TOKEN_STR)) {
            ast_t* t = ast_terminal(AST_STR, parser->lookahead->value, parser->lookahead->position);
            ACCPETTTYPE(TOKEN_STR);
            return t;
        } else if (KEYWORD("true") || KEYWORD("false")) {
            ast_t* t = ast_terminal(AST_BOOL, parser->lookahead->value, parser->lookahead->position);
            ACCPETTTYPE(TOKEN_IDN);
            return t;
        } else if (KEYWORD("null")) {
            ast_t* t = ast_terminal(AST_NULL, parser->lookahead->value, parser->lookahead->position);
            ACCPETTTYPE(TOKEN_IDN);
            return t;
        } else if (CHECKTTYPE(TOKEN_IDN)) {
            ast_t* t = ast_terminal(AST_IDN, parser->lookahead->value, parser->lookahead->position);
            ACCPETTTYPE(TOKEN_IDN);
            return t;
        }
        return NULL;
    }

    static
    ast_t* parser_expression(parser_t* parser);
    static
    ast_t* parser_mandatory_expression(parser_t* parser);
    static
    ast_t* parser_statement(parser_t* parser);

    static
    ast_t* parser_group(parser_t* parser) {
        if (CHECKVALUE("(")) {
            ACCPETVALUE("(");
            ast_t* node = parser_expression(parser);
            if (node == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an expression!", NULL);
            ACCPETVALUE(")");
            return node;
        }
        else if (CHECKVALUE("{")) {
            position_t* start = parser->lookahead->position, *ended = NULL;
            ACCPETVALUE("{");
            INIT_ARRAY(keys);
            INIT_ARRAY(vals);

            ast_t* key = parser_expression(parser), *val = NULL;
            PUSH_ARRAY(keys, key);
            while (key != NULL) {
                key = NULL;
                ACCPETVALUE(":");
                val = parser_expression(parser);
                if (val == NULL)
                    ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an expression!", NULL);
                PUSH_ARRAY(vals, val);
                if (CHECKVALUE(",")) {
                    ACCPETVALUE(",");
                    key = parser_expression(parser);
                    if (key == NULL)
                        ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an expression after \",\"!", NULL);
                    PUSH_ARRAY(keys, key);
                }
            }
            ACCPETVALUE("}");
            ended = parser->previous->position;
            return ast_map(keys, vals, position_merge(start, ended));
        }
        else if (KEYWORD("define")) {
            position_t* start = parser->lookahead->position, *ended = NULL;
            ACCPETVALUE("define");
            ACCPETVALUE("(");
            INIT_ARRAY(params);
            ast_t* paramN = parser_terminal(parser);
            while (paramN != NULL) {
                PUSH_ARRAY(params, paramN);
                paramN = NULL;
                if (CHECKVALUE(",")) {
                    ACCPETVALUE(",");
                    paramN = parser_terminal(parser);
                    if (paramN == NULL)
                        ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an identifier after \",\"!", NULL);
                }
            }
            ACCPETVALUE(")");

            ACCPETVALUE("{");
            INIT_ARRAY(statements);
            ast_t* statement = parser_statement(parser);
            while (statement != NULL) {
                PUSH_ARRAY(statements, statement);
                statement = NULL;
                statement = parser_statement(parser);
            }
            ACCPETVALUE("}");
            ended = parser->previous->position;
            return ast_function_declaration(NULL, params, statements, position_merge(start, ended));
        }

        return parser_terminal(parser);
    }

    static
    ast_t* parser_access_or_call(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        ast_t* node = parser_group(parser);
        if (node == NULL) {
            return NULL;
        }

        while (CHECKVALUE("[") || CHECKVALUE("(")) {
            if (CHECKVALUE("[")) {
                // access
                ACCPETVALUE("[");
                ast_t* index = parser_expression(parser);
                if (index == NULL)
                    ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an expression!", NULL);
                ACCPETVALUE("]");
                ended = parser->previous->position;
                node = ast_index_expression(
                    node, index, 
                    position_merge(start, ended)
                );
            }
            else if (CHECKVALUE("(")) {
                // call
                ACCPETVALUE("(");
                
                INIT_ARRAY(args);

                ast_t* argN = parser_expression(parser);
                while (argN != NULL) {
                    PUSH_ARRAY(args, argN);
                    argN = NULL;
                    if (CHECKVALUE(",")) {
                        ACCPETVALUE(",");
                        argN = parser_expression(parser);
                        if (argN == NULL)
                            ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an expression!", NULL);
                    }
                }
                ACCPETVALUE(")");
                ended = parser->previous->position;
                node = ast_call_expression(node, args, position_merge(start, ended));
            }
        }

        return node;
    }

    static
    ast_t* parser_unary(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        if (CHECKVALUE("++")) {
            ACCPETVALUE("++");
            ast_t* node = parser_unary(parser);
            if (node == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an expression!", NULL);
            ended = parser->previous->position;

            return ast_unary_expression(
                AST_UNARY_PLUSPLUS, 
                "++", 
                node,
                position_merge(start, ended)
            );
        } else if (CHECKVALUE("--")) {
            ACCPETVALUE("--");
            ast_t* node = parser_unary(parser);
            if (node == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an expression!", NULL);
            ended = parser->previous->position;

            return ast_unary_expression(
                AST_UNARY_MINUSMINUS, 
                "--", 
                node,
                position_merge(start, ended)
            );
        }
        return parser_access_or_call(parser);
    }

    static
    ast_t* parser_mul(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;

        ast_t* node = parser_unary(parser);
        if (node == NULL) {
            return NULL;
        }

        while (CHECKVALUE("*") || CHECKVALUE("/") || CHECKVALUE("%")) {
            ast_type_t ast_type;
            char* op = xirius_str_new(parser->lookahead->value);

            if (CHECKVALUE("*")) {
                ast_type = AST_BIN_MUL;
                ACCPETVALUE("*");
            } else if (CHECKVALUE("/")) {
                ast_type = AST_BIN_DIV;
                ACCPETVALUE("/");
            } else if (CHECKVALUE("%")) {
                ast_type = AST_BIN_MOD;
                ACCPETVALUE("%");
            }

            ast_t* right = parser_unary(parser);
            if (right == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "missing right-hand expression.", NULL);

            ended = parser->previous->position;

           node = ast_binary_expression(
                ast_type, 
                op, node, right, 
                position_merge(start, ended)
            );
        }

        return node;
    }

    static
    ast_t* parser_add(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;

        ast_t* node = parser_mul(parser);
        if (node == NULL) {
            return NULL;
        }

        while (CHECKVALUE("+") || CHECKVALUE("-")) {
            ast_type_t ast_type;
            char* op = xirius_str_new(parser->lookahead->value);

            if (CHECKVALUE("+")) {
                ast_type = AST_BIN_ADD;
                ACCPETVALUE("+");
            } else if (CHECKVALUE("-")) {
                ast_type = AST_BIN_SUB;
                ACCPETVALUE("-");
            }

            ast_t* right = parser_mul(parser);
            if (right == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "missing right-hand expression.", NULL);

            ended = parser->previous->position;

            node = ast_binary_expression(
                ast_type, 
                op, node, right, 
                position_merge(start, ended)
            );
        }

        return node;
    }

    static
    ast_t* parser_shift(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;

        ast_t* node = parser_add(parser);
        if (node == NULL) {
            return NULL;
        }

        while (CHECKVALUE("<<") || CHECKVALUE(">>")) {
            ast_type_t ast_type;
            char* op = xirius_str_new(parser->lookahead->value);

            if (CHECKVALUE("<<")) {
                ast_type = AST_BIN_LSHFT;
                ACCPETVALUE("<<");
            } else if (CHECKVALUE(">>")) {
                ast_type = AST_BIN_RSHFT;
                ACCPETVALUE(">>");
            }

            ast_t* right = parser_add(parser);
            if (right == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "missing right-hand expression.", NULL);

            ended = parser->previous->position;

            node = ast_binary_expression(
                ast_type, 
                op, node, right, 
                position_merge(start, ended)
            );
        }

        return node;
    }

    static
    ast_t* parser_relation(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;

        ast_t* node = parser_shift(parser);
        if (node == NULL) {
            return NULL;
        }

        while (CHECKVALUE("<") || CHECKVALUE("<=") || CHECKVALUE(">") || CHECKVALUE(">=")) {
            ast_type_t ast_type;
            char* op = xirius_str_new(parser->lookahead->value);

            if (CHECKVALUE("<")) {
                ast_type = AST_CMP_LT;
                ACCPETVALUE("<");
            } else if (CHECKVALUE("<=")) {
                ast_type = AST_CMP_LTE;
                ACCPETVALUE("<=");
            } else if (CHECKVALUE(">")) {
                ast_type = AST_CMP_GT;
                ACCPETVALUE(">");
            } else if (CHECKVALUE(">=")) {
                ast_type = AST_CMP_GTE;
                ACCPETVALUE(">=");
            }

            ast_t* right = parser_shift(parser);
            if (right == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "missing right-hand expression.", NULL);

            ended = parser->previous->position;

            node = ast_binary_expression(
                ast_type, 
                op, node, right, 
                position_merge(start, ended)
            );
        }

        return node;
    }

    static
    ast_t* parser_equality(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;

        ast_t* node = parser_relation(parser);
        if (node == NULL) {
            return NULL;
        }

        while (CHECKVALUE("==") || CHECKVALUE("!=")) {
            ast_type_t ast_type;
            char* op = xirius_str_new(parser->lookahead->value);

            if (CHECKVALUE("==")) {
                ast_type = AST_CMP_EQ;
                ACCPETVALUE("==");
            } else if (CHECKVALUE("!=")) {
                ast_type = AST_CMP_NEQ;
                ACCPETVALUE("!=");
            }

            ast_t* right = parser_relation(parser);
            if (right == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "missing right-hand expression.", NULL);

            ended = parser->previous->position;

            node = ast_binary_expression(
                ast_type, 
                op, node, right, 
                position_merge(start, ended)
            );
        }

        return node;
    }

    static
    ast_t* parser_bitwise(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;

        ast_t* node = parser_equality(parser);
        if (node == NULL) {
            return NULL;
        }

        while (CHECKVALUE("&") || CHECKVALUE("|") || CHECKVALUE("^")) {
            char* op = xirius_str_new(parser->lookahead->value);
            ast_type_t ast_type;

            if (CHECKVALUE("&")) {
                ast_type = AST_BIT_AND;
                ACCPETVALUE("&");
            } else if (CHECKVALUE("|")) {
                ast_type = AST_BIT_OR;
                ACCPETVALUE("|");
            } else if (CHECKVALUE("^")) {
                ast_type = AST_BIT_XOR;
                ACCPETVALUE("^");
            }

            ast_t* right = parser_equality(parser);
            if (right == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "missing right-hand expression.", NULL);

            ended = parser->previous->position;

            node = ast_binary_expression(
                ast_type, 
                op, node, right, 
                position_merge(start, ended)
            );
        }

        return node;
    }

    static
    ast_t* parser_logical(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;

        ast_t* node = parser_bitwise(parser);
        if (node == NULL) {
            return NULL;
        }

        while (CHECKVALUE("&&") || CHECKVALUE("||")) {
            char* op = xirius_str_new(parser->lookahead->value);
            ast_type_t ast_type;

            if (CHECKVALUE("&&")) {
                ast_type = AST_LOG_AND;
                ACCPETVALUE("&&");
            } else if (CHECKVALUE("||")) {
                ast_type = AST_LOG_OR;
                ACCPETVALUE("||");
            }

            ast_t* right = parser_bitwise(parser);
            if (right == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "missing right-hand expression.", NULL);

            ended = parser->previous->position;

            node = ast_binary_expression(
                ast_type, 
                op, node, right, 
                position_merge(start, ended)
            );
        }

        return node;
    }

    static
    ast_t* parser_assignment(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;

        ast_t* node = parser_logical(parser);
        if (node == NULL) {
            return NULL;
        }

        while (CHECKVALUE("=")) {
            char* op = xirius_str_new(parser->lookahead->value);
            ACCPETVALUE("=");

            ast_t* right = parser_logical(parser);
            if (right == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "missing right-hand expression.", NULL);

            ended = parser->previous->position;

            return ast_binary_expression(
                AST_ASSIGN, 
                op, node, right, 
                position_merge(start, ended)
            );
        }

        return node;
    }

    static
    ast_t* parser_expression(parser_t* parser) {
        return parser_assignment(parser);
    }

    static
    ast_t* parser_mandatory_expression(parser_t* parser) {
        ast_t* node = parser_expression(parser);
        if (node == NULL)
            ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an expression!", NULL);

        return node;
    }

    static
    ast_t* parser_statement(parser_t* parser);

    static
    ast_t* parser_define(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        ACCPETVALUE("define");
        ast_t* fname = parser_terminal(parser);
        if (fname == NULL)
            ERROR_F(parser->lexer->path, parser->lookahead->position, "expected a function name!", NULL);

        ACCPETVALUE("(");
        INIT_ARRAY(params);
        ast_t* paramN = parser_terminal(parser);
        while (paramN != NULL) {
            PUSH_ARRAY(params, paramN);
            paramN = NULL;
            if (CHECKVALUE(",")) {
                ACCPETVALUE(",");
                paramN = parser_terminal(parser);
                if (paramN == NULL)
                    ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an identifier after \",\"!", NULL);
            }
        }
        ACCPETVALUE(")");

        ACCPETVALUE("{");
        INIT_ARRAY(statements);
        ast_t* statement = parser_statement(parser);
        while (statement != NULL) {
            PUSH_ARRAY(statements, statement);
            statement = NULL;
            statement = parser_statement(parser);
        }
        ACCPETVALUE("}");
        ended = parser->previous->position;
        return ast_function_declaration(fname, params, statements, position_merge(start, ended));
    }

    static
    ast_t* parser_var(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        ACCPETVALUE("var");

        INIT_ARRAY(var_names);
        INIT_ARRAY(var_value);

        ast_t* id = parser_terminal(parser), *value = NULL;
        if (id == NULL)
            ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an identifier!", NULL);

        if (CHECKVALUE("=")) {
            ACCPETVALUE("=");
            value = parser_mandatory_expression(parser);

            PUSH_ARRAY(var_names, id);
            PUSH_ARRAY(var_value, value);
        }

        while (CHECKVALUE(",")) {
            ACCPETVALUE(",");
            id = parser_terminal(parser);
            value = NULL;

            if (id == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an identifier after \",\"!", NULL);

            if (CHECKVALUE("=")) {
                ACCPETVALUE("=");
                value = parser_mandatory_expression(parser);

                PUSH_ARRAY(var_names, id);
                PUSH_ARRAY(var_value, value);
            }
        }

        ACCPETVALUE(";");
        ended = parser->previous->position;
        return ast_var_decl(var_names, var_value, position_merge(start, ended));
    }

    static
    ast_t* parser_const(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        ACCPETVALUE("const");

        INIT_ARRAY(var_names);
        INIT_ARRAY(var_value);

        ast_t* id = parser_terminal(parser), *value = NULL;
        if (id == NULL)
            ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an identifier!", NULL);

        if (CHECKVALUE("=")) {
            ACCPETVALUE("=");
            value = parser_mandatory_expression(parser);

            PUSH_ARRAY(var_names, id);
            PUSH_ARRAY(var_value, value);
        }

        while (CHECKVALUE(",")) {
            ACCPETVALUE(",");
            id = parser_terminal(parser);
            value = NULL;

            if (id == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an identifier after \",\"!", NULL);

            if (CHECKVALUE("=")) {
                ACCPETVALUE("=");
                value = parser_mandatory_expression(parser);

                PUSH_ARRAY(var_names, id);
                PUSH_ARRAY(var_value, value);
            }
        }

        ACCPETVALUE(";");
        ended = parser->previous->position;
        return ast_const_decl(var_names, var_value, position_merge(start, ended));
    }

    static
    ast_t* parser_local(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        ACCPETVALUE("local");

        INIT_ARRAY(var_names);
        INIT_ARRAY(var_value);

        ast_t* id = parser_terminal(parser), *value = NULL;
        if (id == NULL)
            ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an identifier!", NULL);

        if (CHECKVALUE("=")) {
            ACCPETVALUE("=");
            value = parser_mandatory_expression(parser);

            PUSH_ARRAY(var_names, id);
            PUSH_ARRAY(var_value, value);
        }

        while (CHECKVALUE(",")) {
            ACCPETVALUE(",");
            id = parser_terminal(parser);
            value = NULL;

            if (id == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an identifier after \",\"!", NULL);

            if (CHECKVALUE("=")) {
                ACCPETVALUE("=");
                value = parser_mandatory_expression(parser);

                PUSH_ARRAY(var_names, id);
                PUSH_ARRAY(var_value, value);
            }
        }

        ACCPETVALUE(";");
        ended = parser->previous->position;
        return ast_local_decl(var_names, var_value, position_merge(start, ended));
    }

    static
    ast_t* parser_while(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        ACCPETVALUE("while");
        ACCPETVALUE("(");

        ast_t* condition = parser_mandatory_expression(parser);
        if (condition == NULL)
            ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an expression!", NULL);

        ACCPETVALUE(")");
        ast_t* body = parser_statement(parser);
        if (body == NULL)
            ERROR_F(parser->lexer->path, parser->lookahead->position, "expected a statement!", NULL);

        ended = parser->previous->position;
        return ast_while_statement(condition, body, position_merge(start, ended));
    }

    static
    ast_t* parser_if(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        ACCPETVALUE("if");
        ACCPETVALUE("(");

        ast_t* condition = parser_mandatory_expression(parser);
        if (condition == NULL)
            ERROR_F(parser->lexer->path, parser->lookahead->position, "expected an expression!", NULL);

        ACCPETVALUE(")");
        ast_t* thenv = parser_statement(parser);
        if (thenv == NULL)
            ERROR_F(parser->lexer->path, parser->lookahead->position, "expected a statement!", NULL);

        ast_t* elsev = NULL;
        if (CHECKVALUE("else")) {
            ACCPETVALUE("else");
            elsev = parser_statement(parser);
            if (elsev == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "expected a statement!", NULL);
        }

        ended = parser->previous->position;
        return ast_if_statement(condition, thenv, elsev, position_merge(start, ended));
    }

    static
    ast_t* parser_block(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        ACCPETVALUE("{");

        INIT_ARRAY(statements);
        ast_t* stmnt = parser_statement(parser);
        while (stmnt != NULL) {
            PUSH_ARRAY(statements, stmnt);
            stmnt = parser_statement(parser);
        }

        ACCPETVALUE("}");
        ended = parser->previous->position;
        return ast_block(statements, position_merge(start, ended));
    }

    static
    ast_t* parser_statement(parser_t* parser) {
        if (KEYWORD("define")) {
            return parser_define(parser);
        }
        else if (KEYWORD("var")) {
            return parser_var(parser);
        }
        else if (KEYWORD("const")) {
            return parser_const(parser);
        }
        else if (KEYWORD("local")) {
            return parser_local(parser);
        }
        else if (KEYWORD("while")) {
            return parser_while(parser);
        }
        else if (KEYWORD("if")) {
            return parser_if(parser);
        }
        else if (CHECKVALUE("{")) {
            return parser_block(parser);
        }

        position_t* start = parser->lookahead->position, *ended = NULL;
        ast_t* node = parser_expression(parser);
        if (node == NULL) {
            return NULL;
        }
        ACCPETVALUE(";");
        ended = parser->previous->position;
        return ast_expr_stmnt(node, position_merge(start, ended));
    }

    static
    ast_t* parser_program(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        INIT_ARRAY(statements);
        ast_t* stmnt = parser_statement(parser);
        while (stmnt != NULL) {
            PUSH_ARRAY(statements, stmnt);
            stmnt = parser_statement(parser);
        }
        ACCPETTTYPE(TOKEN_EOF);
        ended = parser->previous->position;
        return ast_program(statements, position_merge(start, ended));
    }

    static
    ast_t* parser_parse(parser_t* parser) {
        parser->lookahead = lexer_get_next(parser->lexer);
        parser->previous = parser->lookahead;
        return parser_program(parser);
    }

    static
    void parser_free(parser_t* parser) {
        free(parser);
    }
#endif

#ifndef SYMBTABLE_H
#define SYMBTABLE_H

    typedef struct xirius_symbol_table_struct symbol_table_t;
    typedef struct xirius_symbol_struct symbol_t;

    typedef struct xirius_symbol_table_struct {
        symbol_table_t* parent;
        size_t size;
        symbol_t** symbols;
    } symbol_table_t;

    typedef struct xirius_symbol_struct {
        char* name;
        size_t env_offset;
        size_t env_locals;
        bool is_global;
        bool is_const;
    } symbol_t;

    static
    symbol_table_t* symbol_table_new(symbol_table_t* parent) {
        symbol_table_t* table = malloc(sizeof(symbol_table_t));
        if (table == NULL)
            ERROR("failed to allocate memory for symbol table!!!");

        table->parent = parent;
        table->size = 0;
        table->symbols = malloc(sizeof(symbol_t*));
        if (table->symbols == NULL)
            ERROR("failed to allocate memory for symbol table symbols!!!");
        table->symbols[0] = NULL;
        return table;
    }

    static
    bool symbol_table_exists(symbol_table_t* table, const char* name) {
        size_t size = 0;
        symbol_t* symbol;
        while ((symbol = table->symbols[size++]) != NULL) {
            if (xirius_str_equals(symbol->name, name))
                return true;
        }
        return false;
    }

    static
    bool symbol_table_exists_global(symbol_table_t* table, const char* name) {
        symbol_table_t* current = table;
        while (current != NULL) {
            if (symbol_table_exists(current, name))
                return true;
            current = current->parent;
        }
        return false;
    }

    static
    bool symbol_table_insert(symbol_table_t* table, symbol_t* symbol) {
        if (symbol_table_exists(table, symbol->name)) {
            return false;
        }

        table->symbols[table->size++] = symbol;
        table->symbols = realloc(table->symbols, sizeof(symbol_t*) * (table->size + 1));
        table->symbols[table->size] = NULL;
        return true;
    }

    static
    symbol_t* symbol_table_lookup(symbol_table_t* table, const char* name) {
        if (!symbol_table_exists_global(table, name))
            ERROR("symbol does not exist in the table!!!");
    
        symbol_table_t* current = table;
        while (current != NULL) {
            if (symbol_table_exists(current, name)) {
                size_t size = 0;
                symbol_t* symbol;
                while ((symbol = current->symbols[size++]) != NULL) {
                    if (xirius_str_equals(symbol->name, name))
                        return symbol;
                }
            }
            current = current->parent;
        }
        return NULL;
    }

    static
    symbol_t* symbol_new(const char* name, size_t offset, size_t locals, bool is_global, bool is_const) {
        symbol_t* symbol = malloc(sizeof(symbol_t));
        if (symbol == NULL)
            ERROR("failed to allocate memory for symbol!!!");

        symbol->name = xirius_str_new(name);
        symbol->env_offset = offset;
        symbol->env_locals = locals;
        symbol->is_global = is_global;
        symbol->is_const = is_const;
        return symbol;
    }

    static
    void symbol_free(symbol_t* symbol);

    static
    void symbol_table_free(symbol_table_t* table) {
        size_t size = 0;
        symbol_t* symbol;
        while ((symbol = table->symbols[size++]) != NULL) {
            symbol_free(symbol);
        }
        free(table->symbols);
        free(table);
    }

    static
    void symbol_free(symbol_t* symbol) {
        free(symbol->name);
        free(symbol);
    }

#endif

#ifndef SCOPE_H
#define SCOPE_H
    typedef enum xirius_scope_type {
        SCOPE_GLOBAL,
        SCOPE_LOCAL,
        SCOPE_CONDITIONAL,
        SCOPE_SINGLE,
        SCOPE_LOOP,
        SCOPE_FUNCTION,
        SCOPE_AWAITABLE
    } scope_type_t;

    typedef struct xirius_scope_struct scope_t;
    typedef struct xirius_scope_struct {
        scope_t* parent;
        scope_type_t type;
    } scope_t;

    static
    scope_t* scope_new(scope_t* parent, scope_type_t type) {
        scope_t* scope = malloc(sizeof(scope_t));
        if (scope == NULL)
            ERROR("failed to allocate memory for scope!!!");

        scope->parent = parent;
        scope->type = type;
        return scope;
    }

    static
    bool scope_is_global(scope_t* scope) {
        return scope->type == SCOPE_GLOBAL;
    }

    static
    bool scope_is_local(scope_t* scope) {
        return scope->type == SCOPE_LOCAL;
    }

    static
    bool scope_is_loop(scope_t* scope) {
        return scope->type == SCOPE_LOOP;
    }

    static
    bool scope_is_function(scope_t* scope) {
        return scope->type == SCOPE_FUNCTION;
    }

    static
    bool scope_is_awaitable(scope_t* scope) {
        return scope->type == SCOPE_AWAITABLE;
    }

    static
    void scope_free(scope_t* scope) {
        free(scope);
    }
#endif

#include "../src/xirius.h"
#ifndef GENERATOR_H
#define GENERATOR_H
    // No optimization for now
    typedef struct xirius_generator_struct {
        parser_t* parser;
        symbol_table_t* table;
        XS_context* context;
        XS_store* store;
        size_t env_offset;
        size_t env_locals;
    } generator_t;

    static
    generator_t* generator_new(const char* path, const char* data) {
        generator_t* generator = malloc(sizeof(generator_t));
        if (generator == NULL)
            ERROR("failed to allocate memory for generator!!!");

        generator->parser = parser_new(path, data);
        generator->table = symbol_table_new(NULL);
        generator->context = XS_context_new(XS_runtime_new());
        generator->store = XS_store_new();
        generator->env_offset = 0;
        generator->env_locals = 0;
        return generator;
    }

    static
    void generator_assignment_0(generator_t* generator, ast_t* left, bool is_postfix) {
        switch (left->type) {
            case AST_IDN: {
                if (!symbol_table_exists_global(generator->table, left->str_0))
                    ERROR_F(generator->parser->lexer->path, left->position, "symbol \"%s\" does not exist in the table!!!", left->str_0);

                symbol_t* symbol = symbol_table_lookup(generator->table, left->str_0);
                
                if (symbol->is_const)
                    ERROR_F(generator->parser->lexer->path, left->position, "symbol \"%s\" is a constant and cannot be reassigned!!!", left->str_0);
                
                if (symbol->is_global) {
                    XS_opcode_set_global_property(generator->store, left->str_0);
                } else {
                    XS_opcode_store_name(generator->store, left->str_0);
                }
                break;
            }
            default:
                ERROR_F(generator->parser->lexer->path, left->position, "invalid left-hand expression!!!", NULL);
        }
    }

    #define ASSERT_IDENTIFIER(node) {\
        if (node->type != AST_IDN) {\
            ERROR_F(generator->parser->lexer->path, node->position, "expected an identifier!", NULL);\
        }\
    }\

    static
    void generator_expression(generator_t* generator, scope_t* scope, ast_t* node);

    static
    void generator_statement(generator_t* generator, scope_t* scope, ast_t* node);

    static
    void generator_inplace_assignment_0(generator_t* generator, scope_t* scope, ast_t* node) {
        switch (node->type) {
            case AST_IDN: {
                generator_expression(generator, scope, node);
                break;
            }
            case AST_INDEX: {
                ast_t* object = node->data_0, *index = node->data_1;
                generator_expression(generator, scope, index);
                generator_expression(generator, scope, object);
                XS_opcode_dup2(generator->store);
                XS_opcode_get_attribute(generator->store);
                break;
            }
            default:
                ERROR_F(generator->parser->lexer->path, node->position, "invalid left-hand expression!!!", NULL);
        }
    }

    static
    void generator_inplace_assignment_1(generator_t* generator, scope_t* scope, ast_t* node, bool is_postfix) {
        switch (node->type) {
            case AST_IDN: {
                XS_opcode_increment(generator->store);
                XS_opcode_store_name(generator->store, node->str_0);
                break;
            }
            case AST_INDEX: {
                ast_t* object = node->data_0, *index = node->data_1;
                XS_opcode_increment(generator->store);
                XS_opcode_set_attribute(generator->store);
                break;
            }
            default:
                ERROR_F(generator->parser->lexer->path, node->position, "invalid right-hand expression!!!", NULL);
        }
    }

    static
    void generator_expression(generator_t* generator, scope_t* scope, ast_t* node) {
        switch (node->type) {
            case AST_IDN: {
                if (!symbol_table_exists_global(generator->table, node->str_0)) {
                    // If the symbol is not in the table, then redirect to global property property
                    XS_opcode_get_global_property(generator->store, node->str_0);
                    // Cleanup
                    free(node);
                    return;
                }

                symbol_t* symbol = symbol_table_lookup(generator->table, node->str_0);

                if (!symbol->is_global) {
                    XS_opcode_load_name(generator->store, symbol->name);
                    // Cleanup
                    free(node);
                    return;
                }  
                
                XS_opcode_get_global_property(generator->store, symbol->name);
                // Cleanup
                free(node);
                break;
            }
            case AST_INT: {
                XS_opcode_push_const(generator->store, XS_value_new_int(generator->context, strtoll(node->str_0, NULL, 10)));
                // Cleanup
                free(node);
                break;
            }
            case AST_FLT: {
                XS_opcode_push_const(generator->store, XS_value_new_flt(generator->context, strtod(node->str_0, NULL)));
                // Cleanup
                free(node);
                break;
            }
            case AST_STR: {
                XS_opcode_push_const(generator->store, XS_value_new_str(generator->context, node->str_0));
                // Cleanup
                free(node);
                break;
            }
            case AST_BOOL: {
                XS_opcode_push_const(generator->store, XS_value_new_bit(generator->context, xirius_str_equals(node->str_0, "true")));
                // Cleanup
                free(node);
                break;
            }
            case AST_NULL: {
                XS_opcode_push_const(generator->store, XS_value_new_nil(generator->context));
                // Cleanup
                free(node);
                break;
            }
            case AST_MAP: {
                ast_t** keys = node->multi_0, **values = node->multi_1;
                size_t i, j;
                for (i = 0; values[i] != NULL; i++);

                printf("Map size: %zu\n", i);
                
                j = i;
                while (i > 0) {
                    generator_expression(generator, scope, keys[i-1]);
                    generator_expression(generator, scope, values[i-1]);
                    i--;
                }

                XS_opcode_make_object(generator->store, j);
                break;
            }
            case AST_FUNCTION: {
                ast_t** params = node->multi_0;
                ast_t** statements = node->multi_1;

                scope_t* function_scope = scope_new(scope, SCOPE_FUNCTION);

                // Save the current store
                XS_store* store = generator->store, *current = NULL;
                generator->store = XS_store_new();

                symbol_table_t* current_table = generator->table;
                generator->table = symbol_table_new(current_table);

                size_t paramc = 0;
                while (*params != NULL) {
                    ++paramc;
                    ASSERT_IDENTIFIER((*params));
                    XS_opcode_store_name_immediate(generator->store, (const char*) ((*params)->str_0));
                    
                    symbol_table_insert(
                        generator->table, 
                        symbol_new(
                            (*params)->str_0, 
                            generator->env_offset, 
                            generator->env_locals++, 
                            false, 
                            false
                        )
                    );
                    params++;
                }

                scope_t* local_scope = scope_new(function_scope, SCOPE_LOCAL);
                while (*statements != NULL) {
                    generator_statement(generator, local_scope, *statements);
                    statements++;
                }

                XS_opcode_push_const(generator->store, XS_value_new_nil(generator->context));
                XS_opcode_return(generator->store);

                // Restore the store
                current = generator->store;
                generator->store = store;

                // Restore the table
                symbol_table_free(generator->table);
                generator->table = current_table;

                // XS_value_new_fun
                XS_value* fn = XS_value_new_function(generator->context, current, false, false, "anon", paramc);
                XS_opcode_push_callback(generator->store, fn);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_CALL: {
                ast_t* callee = node->data_0;
                ast_t** args = node->multi_0;

                size_t i, j = 0;
                for (i = 0; args[i] != NULL; i++);

                j = i;
                while (i > 0) {
                    generator_expression(generator, scope, args[i - 1]);
                    i--;
                }

                generator_expression(generator, scope, callee);
                XS_opcode_call(generator->store, j);
                // Cleanup
                free(node->position);
                free(node->multi_0);
                free(node);
                break;
            }
            case AST_INDEX: {
                ast_t* object = node->data_0, *index = node->data_1;
                generator_expression(generator, scope, index);
                generator_expression(generator, scope, object);
                XS_opcode_get_attribute(generator->store);
                break;
            }
            case AST_UNARY_PLUSPLUS: {
                generator_inplace_assignment_0(generator, scope, node->data_0);
                generator_inplace_assignment_1(generator, scope, node->data_0, false);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_MUL: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_binary_mul(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_DIV: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_binary_div(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_MOD: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_binary_mod(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_ADD: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_binary_add(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_SUB: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_binary_sub(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_LSHFT: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_binary_lshift(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_RSHFT: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_binary_rshift(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_LT: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_compare_less(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_LTE: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_compare_less_equal(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_GT: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_compare_greater(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_GTE: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_compare_greater_equal(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_EQ: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_compare_equal(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_NEQ: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_compare_not_equal(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_BIT_AND: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_binary_and(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_BIT_OR: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_binary_or(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_BIT_XOR: {
                generator_expression(generator, scope, node->data_0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_binary_xor(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_LOG_AND: {
                generator_expression(generator, scope, node->data_0);
                XS_instruction* j0 =
                XS_opcode_jump_if_false_or_pop(generator->store, 0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_jump_to_current_offset(generator->store, j0);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_LOG_OR: {
                generator_expression(generator, scope, node->data_0);
                XS_instruction* j0 =
                XS_opcode_jump_if_true_or_pop(generator->store, 0);
                generator_expression(generator, scope, node->data_1);
                XS_opcode_jump_to_current_offset(generator->store, j0);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_ASSIGN: {
                // Value
                generator_expression(generator, scope, node->data_1);
                // Left
                generator_assignment_0(generator, node->data_0, false);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            default: 
                ERROR_F(generator->parser->lexer->path, node->position, "[NOT IMPLEMENTED (%d)]", node->type);
        }
    }

    static
    void generator_statement(generator_t* generator, scope_t* scope, ast_t* node) {
        switch (node->type) {
            case AST_FUNCTION: {
                ast_t* name = node->data_0;
                ast_t** params = node->multi_0;
                ast_t** statements = node->multi_1;

                scope_t* function_scope = scope_new(scope, SCOPE_FUNCTION);
                ASSERT_IDENTIFIER(name);

                if (symbol_table_exists_global(generator->table, name->str_0))
                    ERROR_F(generator->parser->lexer->path, name->position, "symbol \"%s\" already exists in the table!!!", name->str_0);

                // Save the current store
                XS_store* store = generator->store, *current = NULL;
                generator->store = XS_store_new();

                symbol_table_t* current_table = generator->table;
                generator->table = symbol_table_new(current_table);

                size_t paramc = 0;
                while (*params != NULL) {
                    ++paramc;
                    ASSERT_IDENTIFIER((*params));
                    XS_opcode_store_name_immediate(generator->store, (const char*) ((*params)->str_0));
                    
                    symbol_table_insert(
                        generator->table, 
                        symbol_new(
                            (*params)->str_0, 
                            generator->env_offset, 
                            generator->env_locals++, 
                            false, 
                            false
                        )
                    );

                    params++;
                }

                scope_t* local_scope = scope_new(function_scope, SCOPE_LOCAL);
                while (*statements != NULL) {
                    generator_statement(generator, local_scope, *statements);
                    statements++;
                }

                XS_opcode_push_const(generator->store, XS_value_new_nil(generator->context));
                XS_opcode_return(generator->store);

                // Restore the store
                current = generator->store;
                generator->store = store;

                // Restore the table
                symbol_table_free(generator->table);
                generator->table = current_table;

                // XS_value_new_fun
                XS_value* fn = XS_value_new_function(generator->context, current, false, false, name->str_0, paramc);
                XS_opcode_push_const(generator->store, fn);
                XS_opcode_set_global_property(generator->store, name->str_0);
                XS_opcode_pop_top(generator->store);
                break;
            }
            case AST_VAR: {
                if (!scope_is_global(scope))
                    ERROR_F(generator->parser->lexer->path, node->position, "variable declaration is only allowed in global scope!", NULL);

                size_t i = 0;
                while (node->multi_0[i] != NULL) {
                    ast_t* name  = node->multi_0[i];
                    ast_t* value = node->multi_1[i];
                    ASSERT_IDENTIFIER(name);
                    if (value == NULL) {
                        XS_opcode_push_const(generator->store, XS_value_new_nil(generator->context));
                    } else {
                        generator_expression(generator, scope, value);
                    }
                    
                    XS_opcode_set_global_property(generator->store, name->str_0);
                    XS_opcode_pop_top(generator->store);
                    
                    if (symbol_table_exists(generator->table, name->str_0))
                        ERROR_F(generator->parser->lexer->path, name->position, "variable \"%s\" already exists!", name->str_0);

                    bool success = 
                    symbol_table_insert(
                        generator->table, 
                        symbol_new(
                            name->str_0, 
                            generator->env_offset, 
                            generator->env_locals++, 
                            true, 
                            false
                        )
                    );

                    if (!success) {
                        ERROR_F(generator->parser->lexer->path, name->position, "variable \"%s\" already exists!", name->str_0);
                    }
                    i++;
                    // Cleanup
                    free(name->position);
                    free(name);
                }
                // Cleanup
                free(node->position);
                free(node->multi_0);
                free(node->multi_1);
                free(node);
                break;
            }
            case AST_CONST: {
                size_t i = 0;
                while (node->multi_0[i] != NULL) {
                    ast_t* name  = node->multi_0[i];
                    ast_t* value = node->multi_1[i];
                    ASSERT_IDENTIFIER(name);
                    if (value == NULL) {
                        XS_opcode_push_const(generator->store, XS_value_new_nil(generator->context));
                    } else {
                        generator_expression(generator, scope, value);
                    }
                    
                    if (scope_is_global(scope)) {
                        XS_opcode_set_global_property(generator->store, name->str_0);
                        XS_opcode_pop_top(generator->store);
                    } else {
                        XS_opcode_store_name_immediate(generator->store, name->str_0);
                    }

                    if (symbol_table_exists(generator->table, name->str_0))
                        ERROR_F(generator->parser->lexer->path, name->position, "variable \"%s\" already exists!", name->str_0);

                    bool success = 
                    symbol_table_insert(
                        generator->table, 
                        symbol_new(
                            name->str_0, 
                            generator->env_offset, 
                            generator->env_locals++, 
                            (bool) scope_is_global(scope), 
                            true
                        )
                    );

                    if (!success) {
                        ERROR_F(generator->parser->lexer->path, name->position, "variable \"%s\" already exists!", name->str_0);
                    }
                    i++;
                    // Cleanup
                    free(name->position);
                    free(name);
                }
                // Cleanup
                free(node->position);
                free(node->multi_0);
                free(node->multi_1);
                free(node);
                break;
            }
            case AST_LOCAL: {
                if (!scope_is_local(scope))
                    ERROR_F(generator->parser->lexer->path, node->position, "local declaration is only allowed in local scope!", NULL);

                size_t i = 0;
                while (node->multi_0[i] != NULL) {
                    ast_t* name  = node->multi_0[i];
                    ast_t* value = node->multi_1[i];
                    ASSERT_IDENTIFIER(name);
                    if (value == NULL) {
                        XS_opcode_push_const(generator->store, XS_value_new_nil(generator->context));
                    } else {
                        generator_expression(generator, scope, value);
                    }
                    
                    XS_opcode_store_name_immediate(generator->store, (const char*) name->str_0);
                    
                    if (symbol_table_exists(generator->table, name->str_0))
                        ERROR_F(generator->parser->lexer->path, name->position, "variable \"%s\" already exists!", name->str_0);

                    bool success = 
                    symbol_table_insert(
                        generator->table, 
                        symbol_new(
                            name->str_0, 
                            generator->env_offset, 
                            generator->env_locals++, 
                            false, 
                            false
                        )
                    );

                    if (!success) {
                        ERROR_F(generator->parser->lexer->path, name->position, "variable \"%s\" already exists!", name->str_0);
                    }
                    i++;
                    // Cleanup
                    free(name->position);
                    free(name);
                }
                // Cleanup
                free(node->position);
                free(node->multi_0);
                free(node->multi_1);
                free(node);
                break;
            }
            case AST_WHILE: {
                ast_t* condition = node->data_0;
                ast_t* body = node->data_1;

                size_t start = XS_opcode_get_current_jump_offset(generator->store);
                XS_instruction* j0, *j1 = NULL;

                switch (condition->type) {
                    case AST_LOG_AND: {
                        generator_expression(generator, scope, condition->data_0);
                        j0 = 
                        XS_opcode_pop_jump_if_false(generator->store, 0);
                        generator_expression(generator, scope, condition->data_1);
                        j1 =
                        XS_opcode_pop_jump_if_false(generator->store, 0);
                        // while body
                        generator_statement(generator, scope, body);
                        XS_opcode_jump_absolute(generator->store, start);
                        XS_opcode_jump_to_current_offset(generator->store, j0);
                        XS_opcode_jump_to_current_offset(generator->store, j1);
                        break;
                    }
                    case AST_LOG_OR: {
                        generator_expression(generator, scope, condition->data_0);
                        j0 = 
                        XS_opcode_pop_jump_if_true(generator->store, 0);
                        generator_expression(generator, scope, condition->data_1);
                        j1 =
                        XS_opcode_pop_jump_if_false(generator->store, 0);
                        // while body
                        XS_opcode_jump_to_current_offset(generator->store, j0);
                        generator_statement(generator, scope, body);
                        XS_opcode_jump_absolute(generator->store, start);
                        XS_opcode_jump_to_current_offset(generator->store, j1);
                        break;
                    }
                    default: {
                        generator_expression(generator, scope, condition);
                        j0 = 
                        XS_opcode_pop_jump_if_false(generator->store, 0);
                        generator_statement(generator, scope, body);
                        XS_opcode_jump_absolute(generator->store, start);
                        XS_opcode_jump_to_current_offset(generator->store, j0);
                        break;
                    }
                }
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_IF: {
                ast_t* condition = node->data_0;
                ast_t* thenv = node->data_1;
                ast_t* elsev = node->data_2;

                XS_instruction* j0, *j1 = NULL, *j2 = NULL;
                scope_t* conditional_scope = scope_new(scope, SCOPE_CONDITIONAL),
                *single_scope = scope_new(scope, SCOPE_SINGLE);

                switch (condition->type) {
                    case AST_LOG_AND: {
                        generator_expression(generator, scope, condition->data_0);
                        j0 =
                        XS_opcode_pop_jump_if_false(generator->store, 0);
                        generator_expression(generator, scope, condition->data_1);
                        j1 = 
                        XS_opcode_pop_jump_if_false(generator->store, 0);
                        // then
                        generator_statement(generator, conditional_scope, thenv);
                        j2 = 
                        XS_opcode_jump_forward(generator->store, 0); // forward to endif

                        XS_opcode_jump_to_current_offset(generator->store, j0);
                        XS_opcode_jump_to_current_offset(generator->store, j1);
                        // else?
                        if (elsev != NULL) {
                            generator_statement(generator, single_scope, elsev);
                        }
                        XS_opcode_jump_to_current_offset(generator->store, j2);
                        break;
                    }
                    case AST_LOG_OR: {
                        generator_expression(generator, scope, condition->data_0);
                        j0 =
                        XS_opcode_pop_jump_if_true(generator->store, 0);
                        generator_expression(generator, scope, condition->data_1);
                        j1 = 
                        XS_opcode_pop_jump_if_false(generator->store, 0);
                        // then
                        XS_opcode_jump_to_current_offset(generator->store, j0);
                        generator_statement(generator, conditional_scope, thenv);
                        j2 = 
                        XS_opcode_jump_forward(generator->store, 0); // forward to endif
                        XS_opcode_jump_to_current_offset(generator->store, j1);
                        // else?
                        if (elsev != NULL) {
                            generator_statement(generator, single_scope, elsev);
                        }
                        XS_opcode_jump_to_current_offset(generator->store, j2);
                        break;
                    }
                    default: {
                        generator_expression(generator, scope, condition);
                        j0 = XS_opcode_pop_jump_if_false(generator->store, 0);
                        j1 = NULL;
                        generator_statement(generator, conditional_scope, thenv);
                        j1 = XS_opcode_jump_forward(generator->store, 0);

                        XS_opcode_jump_to_current_offset(generator->store, j0);
                        if (elsev != NULL) {
                            generator_statement(generator, single_scope, elsev);
                        }
                        XS_opcode_jump_to_current_offset(generator->store, j1);
                        break;
                    }
                }
                scope_free(conditional_scope);
                scope_free(single_scope);

                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_BLOCK: {
                scope_t* local = scope_new(scope, SCOPE_LOCAL);
                ast_t** statements = node->multi_0;

                // Save current table
                symbol_table_t* parent = generator->table, *current = NULL;
                generator->table = symbol_table_new(parent);

                XS_opcode_initialize_block(generator->store);
                while (*statements != NULL) {
                    generator_statement(generator, local, *statements);
                    statements++;
                }
                XS_opcode_end_block(generator->store);
                
                // Clean table
                current = generator->table;
                generator->table = parent;
                symbol_table_free(current);

                // Cleanup
                scope_free(local);
                free(node->position);
                free(node->multi_0);
                free(node);
                break;
            }
            case AST_EXPR_STMNT: {
                generator_expression(generator, scope, node->data_0);
                XS_opcode_pop_top(generator->store);
                // Cleanup
                free(node->position);
                free(node);
                break;
            }
            case AST_PROGRAM: {
                scope_t* global = scope_new(NULL, SCOPE_GLOBAL);
                ast_t** statements = node->multi_0;
                while (*statements != NULL) {
                    generator_statement(generator, global, *statements);
                    statements++;
                }
                XS_opcode_push_const(generator->store, XS_value_new_nil(generator->context));
                XS_opcode_return(generator->store);
                // Clean table
                symbol_table_free(generator->table); 
                
                // Cleanup
                scope_free(global);
                free(node->position);
                free(node->multi_0);
                free(node);
                break;
            }
            default: 
                ERROR_F(generator->parser->lexer->path, node->position, "[NOT IMPLEMENTED (%d)]", node->type);
        }
    }

    static
    XS_store* generator_generate(generator_t* generator) {
        generator_statement(generator, NULL, parser_parse(generator->parser));
        return generator->store;
    }
#endif


XS_value* println(XS_context* context, XS_environment* environment, XS_value** argv, int argc) {
    for (int i = 0; i < argc; i++) {
        XS_value* arg = argv[i];
        printf("%s", XS_value_to_const_string(arg));

        if (i < argc - 1) {
            printf(" ");
        }
    }
    printf("\n");
    return XS_value_new_nil(context);
}

int main(int argc, char** argv) {
    char* file = "./example.xs";
    char* data = xirius_read_file(file);

    generator_t* generator = generator_new(file, data);
    XS_store* store = generator_generate(generator);

    XS_value* println_fn = XS_value_new_cfunction(generator->context, println, false, true, "println", 1);

    object_set(generator->context->global_object->value.obj_value, XS_STR(generator->context, "println"), println_fn);

    XS_runtime_execute(generator->context, store);
    printf("DONE!\n");
    return 0;
}