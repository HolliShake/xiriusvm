#include "opcode.h"

EXPORT XS_instruction* instruction_new(opcode_t opcode) {
    XS_instruction* instruction = XS_malloc(sizeof(XS_instruction));
    assert_allocation(instruction);
    instruction->opcode = opcode;
    // If carries info
    instruction->is_jump_set = false;
    instruction->data_0 = 0;
    instruction->data_1 = 0;
    instruction->offset_0 = 0;
    instruction->offset_1 = 0;
    instruction->offset_2 = 0;
    instruction->offset_3 = 0;
    // If carries data
    instruction->value_0 = NULL;
    instruction->value_1 = NULL;
    instruction->value_2 = NULL;
    instruction->value_3 = NULL;
    return instruction;
}

EXPORT void opcode_push_const(store_t* store, XS_value* value) {
    XS_instruction* 
        instruction = instruction_new(PUSH_CONST);
        instruction->value_0 = value;
    // Push to store
    store_push(store, instruction);
}

// Object operations
EXPORT void opcode_get_attribute(store_t* store) {
    XS_instruction* instruction = instruction_new(GET_ATTRIBUTE);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_make_object(store_t* store, size_t pair_count) {
    XS_instruction* instruction = instruction_new(MAKE_OBJECT);
        instruction->data_0 = pair_count;
    // Push to store
    store_push(store, instruction);
}

// Other operations
EXPORT void opcode_pop_top(store_t* store) {
    XS_instruction* instruction = instruction_new(POP_TOP);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_call(store_t* store, size_t argc) {
    XS_instruction* instruction = instruction_new(CALL);
        instruction->data_0 = argc;
    // Push to store
    store_push(store, instruction);
}

// Binary operations
EXPORT void opcode_binary_mul(store_t* store) {
    XS_instruction* instruction = instruction_new(BINARY_MUL);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_binary_div(store_t* store) {
    XS_instruction* instruction = instruction_new(BINARY_DIV);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_binary_mod(store_t* store) {
    XS_instruction* instruction = instruction_new(BINARY_MOD);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_binary_add(store_t* store) {
    XS_instruction* instruction = instruction_new(BINARY_ADD);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_binary_sub(store_t* store) {
    XS_instruction* instruction = instruction_new(BINARY_SUB);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_binary_lshift(store_t* store) {
    XS_instruction* instruction = instruction_new(BINARY_LSHIFT);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_binary_rshift(store_t* store) {
    XS_instruction* instruction = instruction_new(BINARY_RSHIFT);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_compare_less(store_t* store) {
    XS_instruction* instruction = instruction_new(COMPARE_LESS);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_compare_less_equal(store_t* store) {
    XS_instruction* instruction = instruction_new(COMPARE_LESS_EQUAL);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_compare_greater(store_t* store) {
    XS_instruction* instruction = instruction_new(COMPARE_GREATER);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_compare_greater_equal(store_t* store) {
    XS_instruction* instruction = instruction_new(COMPARE_GREATER_EQUAL);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_compare_equal(store_t* store) {
    XS_instruction* instruction = instruction_new(COMPARE_EQUAL);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_compare_not_equal(store_t* store) {
    XS_instruction* instruction = instruction_new(COMPARE_NOT_EQUAL);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_binary_and(store_t* store) {
    XS_instruction* instruction = instruction_new(BINARY_AND);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_binary_or(store_t* store) {
    XS_instruction* instruction = instruction_new(BINARY_OR);
    // Push to store
    store_push(store, instruction);
}

EXPORT void opcode_binary_xor(store_t* store) {
    XS_instruction* instruction = instruction_new(BINARY_XOR);
    // Push to store
    store_push(store, instruction);
}

// Jumps
EXPORT XS_instruction* opcode_jump_if_false_or_pop(store_t* store, size_t offset) {
    XS_instruction* instruction = instruction_new(JUMP_IF_FALSE_OR_POP);
        instruction->offset_0 = offset;
    // Push to store
    store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* opcode_jump_if_true_or_pop(store_t* store, size_t offset) {
    XS_instruction* instruction = instruction_new(JUMP_IF_TRUE_OR_POP);
        instruction->offset_0 = offset;
    // Push to store
    store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* opcode_jump_if_not_error(store_t* store, size_t offset) {
    XS_instruction* instruction = instruction_new(JUMP_IF_NOT_ERROR);
        instruction->offset_0 = offset;
    // Push to store
    store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* opcode_pop_jump_if_false(store_t* store, size_t offset) {
    XS_instruction* instruction = instruction_new(POP_JUMP_IF_FALSE);
        instruction->offset_0 = offset;
    // Push to store
    store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* opcode_pop_jump_if_true(store_t* store, size_t offset) {
    XS_instruction* instruction = instruction_new(POP_JUMP_IF_TRUE);
        instruction->offset_0 = offset;
    // Push to store
    store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* opcode_jump_absolute(store_t* store, size_t offset) {
    XS_instruction* instruction = instruction_new(JUMP_ABSOLUTE);
        instruction->offset_0 = offset;
    // Push to store
    store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* opcode_jump_forward(store_t* store, size_t offset) {
    XS_instruction* instruction = instruction_new(JUMP_FORWARD);
        instruction->offset_0 = offset;
    // Push to store
    store_push(store, instruction);
    return instruction;
}

// Util
#define opcode_assert_jump(instruction)\
    if (!opcode_is_jump(instruction)) {\
        fprintf(stderr, "%s::%s[%d]: instruction is not a jump instruction!!!\n", __FILE__, __func__, __LINE__);\
        exit(1);\
    }\

static
bool opcode_is_jump(XS_instruction* instruction) {
    return (
        instruction->opcode == JUMP_IF_FALSE_OR_POP ||
        instruction->opcode == JUMP_IF_TRUE_OR_POP  ||
        instruction->opcode == POP_JUMP_IF_FALSE    || 
        instruction->opcode == POP_JUMP_IF_TRUE     || 
        instruction->opcode == JUMP_FORWARD
    );
}

EXPORT size_t opcode_get_current_jump_offset(store_t* store) {
    return (store->icount * 2);
}

EXPORT void opcode_set_jump_offset(XS_instruction* instruction, size_t offset) {
    opcode_assert_jump(instruction);
    instruction->is_jump_set = true;
    instruction->offset_0 = offset;
}

EXPORT void opcode_jump_to_current_offset(store_t* store, XS_instruction* instruction) {
    opcode_set_jump_offset(instruction, opcode_get_current_jump_offset(store));
}

EXPORT size_t opcode_get_jump_offset(XS_instruction* instruction) {
    opcode_assert_jump(instruction);
    if (!instruction->is_jump_set) {
        fprintf(stderr, "%s::%s[%d]: Jump offset is not set!!!\n", __FILE__, __func__, __LINE__);
        exit(1);
    }
    return (size_t) floor(instruction->offset_0 / 2);
}