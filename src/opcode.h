#include "global.h"

#ifndef OPCODE_H
#define OPCODE_H
    typedef enum xirius_opcode_enum {
        LOAD_NAME,
        STORE_NAME_IMMEDIATE,
        STORE_NAME,
        PUSH_CONST,
        PUSH_CALLBACK,
        // Object operations
        SET_GLOBAL_PROPERTY,
        GET_GLOBAL_PROPERTY,
        SET_ATTRIBUTE,
        GET_ATTRIBUTE,
        MAKE_OBJECT,
        // Other operations
        DUP,
        DUP2,
        ROTATE,
        ROTATE3,
        ROTATE4,
        POP_TOP,
        CALL,
        // 
        INCREMENT,
        POST_INCREMENT,
        DECREMENT,
        POST_DECREMENT,
        // Binary operations
        BINARY_MUL,
        BINARY_DIV,
        BINARY_MOD,
        BINARY_ADD,
        BINARY_SUB,
        BINARY_LSHIFT,
        BINARY_RSHIFT,
        COMPARE_LESS,
        COMPARE_LESS_EQUAL,
        COMPARE_GREATER,
        COMPARE_GREATER_EQUAL,
        COMPARE_EQUAL,
        COMPARE_NOT_EQUAL,
        BINARY_AND,
        BINARY_OR,
        BINARY_XOR,
        // Control
        RETURN,
        // Jumps
        JUMP_IF_FALSE_OR_POP,
        JUMP_IF_TRUE_OR_POP,
        JUMP_IF_NOT_ERROR_OR_POP,
        POP_JUMP_IF_FALSE,
        POP_JUMP_IF_TRUE,
        JUMP_ABSOLUTE,
        JUMP_FORWARD,
        // Environment Block
        INITIALIZE_BLOCK,
        END_BLOCK,
    } XS_opcode;

    typedef struct xirius_instruction_struct {
        XS_opcode opcode;
        // If carries info
        bool is_jump_set;
        size_t data_0;
        size_t data_1;
        size_t offset_0;
        size_t offset_1;
        size_t offset_2;
        size_t offset_3;
        // If carries str
        char* str_0;
        char* str_1;
        // If carries data
        XS_value* value_0;
        XS_value* value_1;
        XS_value* value_2;
        XS_value* value_3;
    } XS_instruction;

    EXPORT XS_instruction* XS_instruction_new(XS_opcode opcode);
    EXPORT void XS_instruction_free(XS_instruction* instruction);
    // Variables
    EXPORT void XS_opcode_load_name(XS_store* store, const char* name);
    EXPORT void XS_opcode_store_name_immediate(XS_store* store, const char* name);
    EXPORT void XS_opcode_store_name(XS_store* store, const char* name);
    // Constants
    EXPORT void XS_opcode_push_const(XS_store* store, XS_value* value);
    EXPORT void XS_opcode_push_callback(XS_store* store, XS_value* callback);
    // Object operations
    EXPORT void XS_opcode_set_global_property(XS_store* store, const char* name);
    EXPORT void XS_opcode_get_global_property(XS_store* store, const char* name);
    EXPORT void XS_opcode_set_attribute(XS_store* store);
    EXPORT void XS_opcode_get_attribute(XS_store* store);
    EXPORT void XS_opcode_make_object(XS_store* store, size_t pair_count);
    // Other operations
    EXPORT void XS_opcode_dup(XS_store* store);
    EXPORT void XS_opcode_dup2(XS_store* store);
    EXPORT void XS_opcode_rotate(XS_store* store);
    EXPORT void XS_opcode_rotate3(XS_store* store);
    EXPORT void XS_opcode_rotate4(XS_store* store);
    EXPORT void XS_opcode_pop_top(XS_store* store);
    EXPORT void XS_opcode_call(XS_store* store, size_t argc);
    // 
    EXPORT void XS_opcode_increment(XS_store* store);
    EXPORT void XS_opcode_post_increment(XS_store* store);
    EXPORT void XS_opcode_decrement(XS_store* store);
    EXPORT void XS_opcode_post_decrement(XS_store* store);
    // Binary operations
    EXPORT void XS_opcode_binary_mul(XS_store* store);
    EXPORT void XS_opcode_binary_div(XS_store* store);
    EXPORT void XS_opcode_binary_mod(XS_store* store);
    EXPORT void XS_opcode_binary_add(XS_store* store);
    EXPORT void XS_opcode_binary_sub(XS_store* store);
    EXPORT void XS_opcode_binary_lshift(XS_store* store);
    EXPORT void XS_opcode_binary_rshift(XS_store* store);
    EXPORT void XS_opcode_compare_less(XS_store* store);
    EXPORT void XS_opcode_compare_less_equal(XS_store* store);
    EXPORT void XS_opcode_compare_greater(XS_store* store);
    EXPORT void XS_opcode_compare_greater_equal(XS_store* store);
    EXPORT void XS_opcode_compare_equal(XS_store* store);
    EXPORT void XS_opcode_compare_not_equal(XS_store* store);
    EXPORT void XS_opcode_binary_and(XS_store* store);
    EXPORT void XS_opcode_binary_or(XS_store* store);
    EXPORT void XS_opcode_binary_xor(XS_store* store);
    // Control
    EXPORT void XS_opcode_return(XS_store* store);
    // Jumps
    EXPORT XS_instruction* XS_opcode_jump_if_false_or_pop(XS_store* store, size_t offset);
    EXPORT XS_instruction* XS_opcode_jump_if_true_or_pop(XS_store* store, size_t offset);
    EXPORT XS_instruction* XS_opcode_jump_if_not_error_or_pop(XS_store* store, size_t offset);
    EXPORT XS_instruction* XS_opcode_pop_jump_if_false(XS_store* store, size_t offset);
    EXPORT XS_instruction* XS_opcode_pop_jump_if_true(XS_store* store, size_t offset);
    EXPORT XS_instruction* XS_opcode_jump_absolute(XS_store* store, size_t offset);
    EXPORT XS_instruction* XS_opcode_jump_forward(XS_store* store, size_t offset);
    // Environment Block
    EXPORT void XS_opcode_initialize_block(XS_store* store);
    EXPORT void XS_opcode_end_block(XS_store* store);
    // Util
    EXPORT size_t XS_opcode_get_current_jump_offset(XS_store* store);
    EXPORT void XS_opcode_set_jump_offset(XS_instruction* instruction, size_t offset);
    EXPORT void XS_opcode_jump_to_current_offset(XS_store* store, XS_instruction* instruction);
    EXPORT size_t XS_opcode_get_jump_offset(XS_instruction* instruction);
#endif