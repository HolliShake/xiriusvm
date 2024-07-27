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
        // 
        AST_VAR,
        AST_IF,
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
    ast_t* ast_call_expression(ast_t* callee, ast_t** args, position_t* pos) {
        ast_t* ast = ast_init(AST_CALL, pos);
        ast->data_0 = callee;
        ast->multi_0 = args;
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
    ast_t* ast_var_decl(ast_t** names, ast_t** values, position_t* pos) {
        ast_t* ast = ast_init(AST_VAR, pos);
        ast->multi_0 = names;
        ast->multi_1 = values;
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
    ast_t* parser_access_or_call(parser_t* parser) {
        position_t* start = parser->lookahead->position, *ended = NULL;
        ast_t* node = parser_terminal(parser);
        if (node == NULL) {
            return NULL;
        }

        while (CHECKVALUE("(")) {
            if (CHECKVALUE("(")) {
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
    ast_t* parser_mul(parser_t* parser) {
        ast_t* node = parser_access_or_call(parser);
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

            ast_t* right = parser_access_or_call(parser);
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
    ast_t* parser_shift(parser_t* parser) {
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
    ast_t* parser_relation(parser_t* parser) {
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
    ast_t* parser_equality(parser_t* parser) {
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
    ast_t* parser_bitwise(parser_t* parser) {
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
    ast_t* parser_logical(parser_t* parser) {
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
    ast_t* parser_expression(parser_t* parser) {
        return parser_logical(parser);
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
    ast_t* parser_statement(parser_t* parser) {
        if (KEYWORD("var")) {
            return parser_var(parser);
        }
        else if (KEYWORD("if")) {
            return parser_if(parser);
        }

        position_t* start = parser->lookahead->position, *ended = NULL;
        ast_t* node = parser_logical(parser);
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
        if (!symbol_table_exists(table, name)) {
            ERROR("symbol does not exist in the table!!!");
        }
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
    void generator_expression(generator_t* generator, ast_t* node) {
        switch (node->type) {
            case AST_IDN: {
                if (!symbol_table_exists_global(generator->table, node->str_0)) {
                    // If the symbol is not in the table, then redirect to global property property
                    XS_opcode_get_global_property(generator->store, node->str_0);
                    return;
                }

                symbol_t* symbol = symbol_table_lookup(generator->table, node->str_0);

                if (symbol->is_global) {
                    XS_opcode_get_global_property(generator->store, symbol->name);
                    return;
                }
                XS_opcode_load_name(generator->store, symbol->env_offset, symbol->env_locals, symbol->name);
                break;
            }
            case AST_INT: {
                XS_opcode_push_const(generator->store, XS_value_new_int(generator->context, strtoll(node->str_0, NULL, 10)));
                break;
            }
            case AST_FLT: {
                XS_opcode_push_const(generator->store, XS_value_new_flt(generator->context, strtod(node->str_0, NULL)));
                break;
            }
            case AST_STR: {
                XS_opcode_push_const(generator->store, XS_value_new_str(generator->context, node->str_0));
                break;
            }
            case AST_BOOL: {
                XS_opcode_push_const(generator->store, XS_value_new_bit(generator->context, xirius_str_equals(node->str_0, "true")));
                break;
            }
            case AST_NULL: {
                XS_opcode_push_const(generator->store, XS_value_new_nil(generator->context));
                break;
            }
            case AST_CALL: {
                ast_t* callee = node->data_0;
                ast_t** args = node->multi_0;

                size_t i, j = 0;
                for (i = 0; args[i] != NULL; i++);

                j = i;
                while (i > 0) {
                    generator_expression(generator, args[i - 1]);
                    i--;
                }

                generator_expression(generator, callee);
                XS_opcode_call(generator->store, j);
                break;
            }
            case AST_BIN_MUL: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_binary_mul(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_DIV: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_binary_div(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_MOD: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_binary_mod(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_ADD: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_binary_add(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_SUB: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_binary_sub(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_LSHFT: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_binary_lshift(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_BIN_RSHFT: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_binary_rshift(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_LT: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_compare_less(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_LTE: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_compare_less_equal(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_GT: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_compare_greater(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_GTE: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_compare_greater_equal(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_EQ: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_compare_equal(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_CMP_NEQ: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_compare_not_equal(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_BIT_AND: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_binary_and(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_BIT_OR: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_binary_or(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_BIT_XOR: {
                generator_expression(generator, node->data_0);
                generator_expression(generator, node->data_1);
                XS_opcode_binary_xor(generator->store);
                free(node->position);
                free(node);
                break;
            }
            case AST_LOG_AND: {
                generator_expression(generator, node->data_0);
                XS_instruction* j0 =
                XS_opcode_jump_if_false_or_pop(generator->store, 0);
                generator_expression(generator, node->data_1);
                XS_opcode_jump_to_current_offset(generator->store, j0);
                free(node->position);
                free(node);
                break;
            }
            case AST_LOG_OR: {
                generator_expression(generator, node->data_0);
                XS_instruction* j0 =
                XS_opcode_jump_if_true_or_pop(generator->store, 0);
                generator_expression(generator, node->data_1);
                XS_opcode_jump_to_current_offset(generator->store, j0);
                free(node->position);
                free(node);
                break;
            }
            default: 
                ERROR_F(generator->parser->lexer->path, node->position, "[NOT IMPLEMENTED (%d)]", node->type);
        }
    }

    #define ASSERT_IDENTIFIER(node) {\
        if (node->type != AST_IDN) {\
            ERROR_F(generator->parser->lexer->path, node->position, "expected an identifier!", NULL);\
        }\
    }\

    static
    void generator_statement(generator_t* generator, ast_t* node) {
        switch (node->type) {
            case AST_VAR: {
                size_t i = 0;
                while (node->multi_0[i] != NULL) {
                    ast_t* name  = node->multi_0[i];
                    ast_t* value = node->multi_1[i];
                    ASSERT_IDENTIFIER(name);
                    if (value == NULL) {
                        XS_opcode_push_const(generator->store, XS_value_new_nil(generator->context));
                    } else {
                        generator_expression(generator, value);
                    }
                    XS_opcode_set_global_property(generator->store, name->str_0);
                    
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
                }
                break;
            }
            case AST_IF: {
                ast_t* condition = node->data_0;
                ast_t* thenv = node->data_1;
                ast_t* elsev = node->data_2;

                XS_instruction* j0 = XS_opcode_pop_jump_if_false(generator->store, 0);
                XS_instruction* j1 = NULL, *j2 = NULL;

                switch (condition->type) {
                    case AST_LOG_AND: {
                        generator_expression(generator, condition->data_0);
                        j0 =
                        XS_opcode_pop_jump_if_false(generator->store, 0);
                        generator_expression(generator, condition->data_1);
                        j1 = 
                        XS_opcode_pop_jump_if_false(generator->store, 0);
                        // then
                        generator_statement(generator, thenv);
                        j2 = 
                        XS_opcode_jump_forward(generator->store, 0); // forward to endif
                        
                        XS_opcode_jump_to_current_offset(generator->store, j0);
                        XS_opcode_jump_to_current_offset(generator->store, j1);
                        // else?
                        if (elsev != NULL) {
                            generator_statement(generator, elsev);
                        }
                        XS_opcode_jump_to_current_offset(generator->store, j2);
                        break;
                    }
                    case AST_LOG_OR: {
                        generator_expression(generator, condition->data_0);
                        j0 =
                        XS_opcode_pop_jump_if_true(generator->store, 0);
                        generator_expression(generator, condition->data_1);
                        j1 = 
                        XS_opcode_pop_jump_if_false(generator->store, 0);
                        // then
                        XS_opcode_jump_to_current_offset(generator->store, j0);
                        generator_statement(generator, thenv);
                        j2 = 
                        XS_opcode_jump_forward(generator->store, 0); // forward to endif
                        XS_opcode_jump_to_current_offset(generator->store, j1);
                        // else?
                        if (elsev != NULL) {
                            generator_statement(generator, elsev);
                        }
                        XS_opcode_jump_to_current_offset(generator->store, j2);
                        break;
                    }
                    default: {
                        generator_expression(generator, condition);
                        j0 = XS_opcode_pop_jump_if_false(generator->store, 0);
                        j1 = NULL;
                        generator_statement(generator, thenv);
                        j1 = XS_opcode_jump_forward(generator->store, 0);

                        XS_opcode_jump_to_current_offset(generator->store, j0);
                        if (elsev != NULL) {
                            generator_statement(generator, elsev);
                        }
                        XS_opcode_jump_to_current_offset(generator->store, j1);
                        break;
                    }
                }

                break;
            }
            case AST_EXPR_STMNT: {
                generator_expression(generator, node->data_0);
                XS_opcode_pop_top(generator->store);
                break;
            }
            case AST_PROGRAM: {
                ast_t** statements = node->multi_0;
                while (*statements != NULL) {
                    generator_statement(generator, *statements);
                    statements++;
                }
                XS_opcode_push_const(generator->store, XS_value_new_nil(generator->context));
                XS_opcode_return(generator->store);
                break;
            }
            default: 
                ERROR_F(generator->parser->lexer->path, node->position, "[NOT IMPLEMENTED (%d)]", node->type);
        }
    }

    static
    XS_store* generator_generate(generator_t* generator) {
        generator_statement(generator, parser_parse(generator->parser));
        return generator->store;
    }
#endif


XS_value* println(XS_context* context, XS_value** argv, int argc) {
    printf("> ");
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

    XS_value* println_fn = XS_value_new_cfunction(println, false, "println", 1);

    generator_t* generator = generator_new(file, data);
    XS_store* store = generator_generate(generator);

    object_set(generator->context->global_object->value.obj_value, XS_STR(generator->context, "println"), println_fn);

    XS_runtime_execute(generator->context, store);
    printf("DONE!\n");
    return 0;
}