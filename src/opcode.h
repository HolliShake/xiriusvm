#include "global.h"
#include "store.h"
#include "xsvalue.h"

#ifndef OPCODE_H
#define OPCODE_H
    typedef enum opcode_enum {
        PUSH_CONST,
        // Object operations
        MAKE_OBJECT,
        // Other operations
        POP_TOP,
        CALL,
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
        // Jumps
        JUMP_IF_FALSE_OR_POP,
        JUMP_IF_TRUE_OR_POP,
        POP_JUMP_IF_FALSE,
        POP_JUMP_IF_TRUE,
        JUMP_ABSOLUTE,
        JUMP_FORWARD
    } opcode_t;

    typedef struct xirius_instruction_struct {
        opcode_t opcode;
        // If carries info
        bool is_jump_set;
        size_t data_0;
        size_t data_1;
        size_t offset_0;
        size_t offset_1;
        size_t offset_2;
        size_t offset_3;
        // If carries data
        XS_value* value_0;
        XS_value* value_1;
        XS_value* value_2;
        XS_value* value_3;
    } XS_instruction;

    EXPORT XS_instruction* instruction_new(opcode_t opcode);
    // Push constant
    EXPORT void opcode_push_const(store_t* store, XS_value* value);
    // Object operations
    EXPORT void opcode_make_object(store_t* store, size_t pair_count);
    // Other operations
    EXPORT void opcode_pop_top(store_t* store);
    EXPORT void opcode_call(store_t* store, size_t argc);
    // Binary operations
    EXPORT void opcode_binary_mul(store_t* store);
    EXPORT void opcode_binary_div(store_t* store);
    EXPORT void opcode_binary_mod(store_t* store);
    EXPORT void opcode_binary_add(store_t* store);
    EXPORT void opcode_binary_sub(store_t* store);
    EXPORT void opcode_binary_lshift(store_t* store);
    EXPORT void opcode_binary_rshift(store_t* store);
    EXPORT void opcode_compare_less(store_t* store);
    EXPORT void opcode_compare_less_equal(store_t* store);
    EXPORT void opcode_compare_greater(store_t* store);
    EXPORT void opcode_compare_greater_equal(store_t* store);
    EXPORT void opcode_compare_equal(store_t* store);
    EXPORT void opcode_compare_not_equal(store_t* store);
    EXPORT void opcode_binary_and(store_t* store);
    EXPORT void opcode_binary_or(store_t* store);
    EXPORT void opcode_binary_xor(store_t* store);
    // Jumps
    EXPORT XS_instruction* opcode_jump_if_false_or_pop(store_t* store, size_t offset);
    EXPORT XS_instruction* opcode_jump_if_true_or_pop(store_t* store, size_t offset);
    EXPORT XS_instruction* opcode_pop_jump_if_false(store_t* store, size_t offset);
    EXPORT XS_instruction* opcode_pop_jump_if_true(store_t* store, size_t offset);
    EXPORT XS_instruction* opcode_jump_absolute(store_t* store, size_t offset);
    EXPORT XS_instruction* opcode_jump_forward(store_t* store, size_t offset);
    // Util
    EXPORT size_t opcode_get_current_jump_offset(store_t* store);
    EXPORT void opcode_set_jump_offset(XS_instruction* instruction, size_t offset);
    EXPORT size_t opcode_get_jump_offset(XS_instruction* instruction);
#endif