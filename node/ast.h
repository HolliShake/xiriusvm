#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NODE_H
#define NODE_H
    #ifndef IMPEXPTHEADERS
        #if defined(_WIN32) || defined(_WIN64)
            #define EXPORT __declspec(dllexport)
            #define IMPORT __declspec(dllimport)
        #else
            #define EXPORT
            #define IMPORT
        #endif
    #endif

    typedef struct xirius_position_struct {
        size_t start_line;
        size_t start_colm;
        size_t ended_line;
        size_t ended_colm;
    } XS_position;

    EXPORT XS_position* XS_position_new(size_t start_line, size_t start_colm, size_t ended_line, size_t ended_colm);
    EXPORT XS_position* XS_position_from_line_and_colm(size_t start_line, size_t start_colm);
    EXPORT XS_position* XS_position_merge(XS_position* pos0, XS_position* pos1);

    typedef enum xirius_ast_type_enum {
        XS_AST_INT,
        XS_AST_FLT,
        XS_AST_STR,
        XS_AST_BIT,
        XS_AST_NIL,
        XS_AST_BINARY_MUL,
        XS_AST_BINARY_DIV,
        XS_AST_BINARY_MOD,
        XS_AST_BINARY_ADD,
        XS_AST_BINARY_SUB,
        XS_AST_BINARY_SHL,
        XS_AST_BINARY_SHR,
        XS_AST_COMPARE_LT,
        XS_AST_COMPARE_LTE,
        XS_AST_COMPARE_GT,
        XS_AST_COMPARE_GTE,
        XS_AST_COMPARE_EQT,
        XS_AST_COMPARE_NEQ,
        XS_AST_BINARY_AND,
        XS_AST_BINARY_OR,
        XS_AST_BINARY_XOR,
        // 
        XS_AST_FUNCTION,
    } XS_ast_type;

    #define string char*
    typedef struct xirius_ast_struct XS_ast;
    typedef struct xirius_ast_struct {
        XS_ast_type type;
        string str;
        size_t len;
        /**************/ 
        XS_ast* data_0;        
        XS_ast* data_1;        
        XS_ast* data_2;    
        /**************/ 
        XS_ast** multi_0;        
        XS_ast** multi_1;
        /**************/ 
        XS_position* pos;
    } XS_ast;

    EXPORT XS_ast* XS_ast_int(string str, XS_position* pos);
    EXPORT XS_ast* XS_ast_flt(string str, XS_position* pos);
    EXPORT XS_ast* XS_ast_str(string str, XS_position* pos);
    EXPORT XS_ast* XS_ast_bit(string str, XS_position* pos);
    EXPORT XS_ast* XS_ast_nil(string str, XS_position* pos);
    EXPORT XS_ast* XS_ast_postfix_expression(XS_ast_type variant, string opt, XS_ast* expr, XS_position* pos);
    EXPORT XS_ast* XS_ast_unary_expression(XS_ast_type variant, string opt, XS_ast* expr, XS_position* pos);
    EXPORT XS_ast* XS_ast_binary_expression(XS_ast_type variant, string opt, XS_ast* lhs, XS_ast* rhs, XS_position* pos);
#endif