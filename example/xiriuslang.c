#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>

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
    void lexer_forward(lexer_t* lexer) {
        if (lexer->index >= strlen(lexer->data))
            return;

        if (lexer->data[lexer->index] == '\n') {
            lexer->line++;
            lexer->colm = 1;
        } else {
            lexer->colm++;
        }

        lexer->index++;
    }

    static
    char* lexer_lookahead(lexer_t* lexer) {
        return xirius_str_format("%c", lexer->data[lexer->index]);
    }

    static
    bool lexer_is_eof(lexer_t* lexer) {
        return lexer->index >= strlen(lexer->data);
    }

    static
    bool lexer_is_whitespace(lexer_t* lexer) {
        char c = lexer->data[lexer->index];
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    static
    bool lexer_is_identifier(lexer_t* lexer) {
        // does not support utf-8
        char c = lexer->data[lexer->index];
        return (c == '_' ) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    static
    bool lexer_is_digit(lexer_t* lexer) {
        char c = lexer->data[lexer->index];
        return c >= '0' && c <= '9';
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

        if (lexer->data[lexer->index] == '.') {
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
    token_t* lexer_next_symbol(lexer_t* lexer) {
        size_t start_line = lexer->line, start_colm = lexer->colm;
        char* value = xirius_str_new(""), *old = NULL, *look = NULL;

        char c = lexer->data[lexer->index];
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

                if (lexer->data[lexer->index] == '=') {
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

                if (lexer->data[lexer->index] == '/' || lexer->data[lexer->index] == '*') {
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

                if (lexer->data[lexer->index] == '=') {
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

                if (lexer->data[lexer->index] == '+' || lexer->data[lexer->index] == '=') {
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

                if (lexer->data[lexer->index] == '-' || lexer->data[lexer->index] == '=') {
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

                if (lexer->data[lexer->index] == '<' || lexer->data[lexer->index] == '=') {
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

                if (lexer->data[lexer->index] == '>' || lexer->data[lexer->index] == '=') {
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

                if (lexer->data[lexer->index] == '=') {
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

                if (lexer->data[lexer->index] == '=') {
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

                if (lexer->data[lexer->index] == '&') {
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

                if (lexer->data[lexer->index] == '|') {
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

                if (lexer->data[lexer->index] == '^') {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                break;
            }
            default: {
                while (!lexer_is_eof(lexer) && !lexer_is_whitespace(lexer)) {
                    value = xirius_str_add(old = value, look = lexer_lookahead(lexer));
                    free(old); free(look);
                    lexer_forward(lexer);
                }

                ERROR_F(lexer->path, position_new(start_line, start_colm), "unexpected symbol \"%s\"!", value);
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
            printf("TOKEN: %s\n", token->value);
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
        AST_MAP,
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
        // 
        AST_EXPR_STMNT
    } ast_type_t;

    typedef struct ast_struct ast_t;
    typedef struct ast_struct {
        ast_type_t type;
        // 
        char* str_0;
        char* str_1;
        ast_t* data_0;
        ast_t* data_1;
        void* data_2;
        void** multi_0;
        void** multi_1;
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
    ast_t* ast_binary_expression(ast_type_t type, char* op, ast_t* lhs, ast_t* rhs, position_t* pos) {
        ast_t* ast = ast_init(type, pos);
        ast->str_0 = op;
        ast->data_0 = lhs;
        ast->data_1 = rhs;
        return ast;
    }

    static
    ast_t* ast_expr_stmnt(ast_t* expr, position_t* pos) {
        ast_t* ast = ast_init(AST_EXPR_STMNT, pos);
        ast->data_0 = expr;
        return ast;
    }
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
        if (CHECKTTYPE(TOKEN_IDN)) {
            ast_t* t = ast_terminal(AST_IDN, parser->lookahead->value, parser->lookahead->position);
            ACCPETTTYPE(TOKEN_IDN);
            return t;
        } else if (CHECKTTYPE(TOKEN_INT)) {
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
        }
        return NULL;
    }

    static
    ast_t* parser_mul(parser_t* parser) {
        ast_t* node = parser_terminal(parser);
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

            ast_t* right = parser_terminal(parser);
            if (right == NULL)
                ERROR_F(parser->lexer->path, parser->lookahead->position, "expected a number!", NULL);

           node = ast_binary_expression(
                ast_type, 
                op, node, right, 
                parser->previous->position
            );
        }

        return node;
    }

    static
    ast_t* parser_add(parser_t* parser) {
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
                ERROR_F(parser->lexer->path, parser->lookahead->position, "expected a number!", NULL);

           node = ast_binary_expression(
                ast_type, 
                op, node, right, 
                parser->previous->position
            );
        }

        return node;
    }

    static
    ast_t* parser_statement(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        ast_t* node = parser_add(parser);
        if (node == NULL) {
            return NULL;
        }
        ACCPETVALUE(";");
        ended = parser->previous->position;
        return ast_expr_stmnt(node, position_merge(start, ended));
    }

    static
    ast_t* parser_parse(parser_t* parser) {
        parser->lookahead = lexer_get_next(parser->lexer);
        parser->previous = parser->lookahead;
        return parser_statement(parser);
    }

    static
    void parser_free(parser_t* parser) {
        free(parser);
    }
#endif

#ifndef GENERATOR_H
#define GENERATOR_H
    
#endif

int main(int argc, char** argv) {
    
    char* file = "./example.xs";
    char* data = xirius_read_file(file);

    parser_t* parser = parser_new(file, data);
    parser_parse(parser);
    printf("PARSED!!!\n");
    parser_free(parser);

    return 0;
}