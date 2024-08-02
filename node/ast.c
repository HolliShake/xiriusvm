#include "ast.h"

static
char* xs_ast_str_copy(const char* str) {
    char* new_str = XS_malloc(strlen(str) + 1);
    if (new_str == NULL) {
        fprintf(stderr, "%s::%s[%d]: failed to allocate memory for AST string!!!\n", __FILE__, __func__, __LINE__);
        exit(1);
    }
    new_str[0] = '\0';
    strcpy(new_str, str);
    return new_str;
}

EXPORT XS_position* XS_position_new(size_t start_line, size_t start_colm, size_t ended_line, size_t ended_colm) {
    XS_position* pos = XS_malloc(sizeof(XS_position));
    if (pos == NULL) {
        fprintf(stderr, "%s::%s[%d]: failed to allocate memory for AST position!!!\n", __FILE__, __func__, __LINE__);
        exit(1);
    }
    pos->start_line = start_line;
    pos->start_colm = start_colm;
    pos->ended_line = ended_line;
    pos->ended_colm = ended_colm;
    return pos;
}

EXPORT XS_position* XS_position_from_line_and_colm(size_t start_line, size_t start_colm) {
    return XS_position_new(start_line, start_colm, start_line, start_colm);
}

EXPORT XS_position* XS_position_merge(XS_position* pos0, XS_position* pos1) {
    return XS_position_new(pos0->start_line, pos0->start_colm, pos1->ended_line, pos0->ended_colm);
}

// 

static
XS_ast* XS_ast_init(XS_ast_type type, XS_position* pos) {
    XS_ast* ast = XS_malloc(sizeof(XS_ast));
    if (ast == NULL) {
        fprintf(stderr, "%s::%s[%d]: failed to allocate memory for AST node!!!\n", __FILE__, __func__, __LINE__);
        exit(1);
    }
    ast->type = type;
    ast->str = NULL;
    ast->len = 0;
    /**************/ 
    ast->data_0 = NULL;
    ast->data_1 = NULL;
    ast->data_2 = NULL;
    /**************/ 
    ast->multi_0 = NULL;
    ast->multi_1 = NULL;
    /**************/
    ast->pos = pos;
    return ast;
}

EXPORT XS_ast* XS_ast_int(string str, XS_position* pos) {
    XS_ast* ast = XS_ast_init(XS_AST_INT, pos);
    ast->str = xs_ast_str_copy((const char*) str);
    return ast;
}

EXPORT XS_ast* XS_ast_flt(string str, XS_position* pos) {
    XS_ast* ast = XS_ast_init(XS_AST_FLT, pos);
    ast->str = xs_ast_str_copy((const char*) str);
    return ast;
}

EXPORT XS_ast* XS_ast_str(string str, XS_position* pos) {
    XS_ast* ast = XS_ast_init(XS_AST_STR, pos);
    ast->str = xs_ast_str_copy((const char*) str);
    return ast;
}

EXPORT XS_ast* XS_ast_bit(string str, XS_position* pos) {
    XS_ast* ast = XS_ast_init(XS_AST_BIT, pos);
    ast->str = xs_ast_str_copy((const char*) str);
    return ast;
}

EXPORT XS_ast* XS_ast_nil(string str, XS_position* pos) {
    XS_ast* ast = XS_ast_init(XS_AST_NIL, pos);
    ast->str = xs_ast_str_copy((const char*) str);
    return ast;
}

EXPORT XS_ast* XS_ast_postfix_expression(XS_ast_type variant, string opt, XS_ast* expr, XS_position* pos) {
    XS_ast* ast = XS_ast_init(variant, pos);
    ast->str = xs_ast_str_copy((const char*) opt);
    ast->data_0 = expr;
    return ast;
}

EXPORT XS_ast* XS_ast_unary_expression(XS_ast_type variant, string opt, XS_ast* expr, XS_position* pos) {
    XS_ast* ast = XS_ast_init(variant, pos);
    ast->str = xs_ast_str_copy((const char*) opt);
    ast->data_0 = expr;
    return ast;
}

EXPORT XS_ast* XS_ast_binary_expression(XS_ast_type variant, string opt, XS_ast* lhs, XS_ast* rhs, XS_position* pos) {
    XS_ast* ast = XS_ast_init(variant, pos);
    ast->str = xs_ast_str_copy((const char*) opt);
    ast->data_0 = lhs;
    ast->data_1 = rhs;
    return ast;
}

EXPORT XS_ast* XS_ast_function(const char* name, XS_ast** parameters, XS_ast** body, XS_position* pos) {
    XS_ast* ast = XS_ast_init(XS_AST_FUNCTION, pos);
    ast->str = xs_ast_str_copy(name);
    ast->multi_0 = parameters;
    ast->multi_1 = body;
    return ast;
}