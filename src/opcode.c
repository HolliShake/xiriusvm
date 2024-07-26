#include "opcode.h"

EXPORT XS_instruction* XS_instruction_new(XS_opcode opcode) {
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
    // If carries str
    instruction->str_0 = NULL;
    instruction->str_1 = NULL;
    // If carries data
    instruction->value_0 = NULL;
    instruction->value_1 = NULL;
    instruction->value_2 = NULL;
    instruction->value_3 = NULL;
    return instruction;
}

EXPORT void XS_instruction_free(XS_instruction* instruction) {
    // If carries str
    if (instruction->str_0 != NULL)
        XS_free(instruction->str_0);
    if (instruction->str_1 != NULL)
        XS_free(instruction->str_1);
    // Free instruction
    XS_free(instruction);
}

// Variables
EXPORT void XS_opcode_load_name(XS_store* store, size_t env_offset, size_t var_offset, const char* name) {
    XS_instruction* instruction = XS_instruction_new(LOAD_NAME);
        instruction->offset_0 = env_offset;
        instruction->offset_1 = var_offset;
        instruction->str_0 = str__new(name);
    // Push to store
    XS_store_push(store, instruction);
}

// Constants
EXPORT void XS_opcode_push_const(XS_store* store, XS_value* value) {
    XS_instruction* 
        instruction = XS_instruction_new(PUSH_CONST);
        instruction->value_0 = value;
    // Push to store
    XS_store_push(store, instruction);
}

// Object operations
EXPORT void XS_opcode_set_global_property(XS_store* store, const char* name) {
    XS_instruction* instruction = XS_instruction_new(SET_GLOBAL_PROPERTY);
        instruction->str_0 = str__new(name);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_get_global_property(XS_store* store, const char* name) {
    XS_instruction* instruction = XS_instruction_new(GET_GLOBAL_PROPERTY);
        instruction->str_0 = str__new(name);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_get_attribute(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(GET_ATTRIBUTE);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_make_object(XS_store* store, size_t pair_count) {
    XS_instruction* instruction = XS_instruction_new(MAKE_OBJECT);
        instruction->data_0 = pair_count;
    // Push to store
    XS_store_push(store, instruction);
}

// Other operations
EXPORT void XS_opcode_pop_top(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(POP_TOP);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_call(XS_store* store, size_t argc) {
    XS_instruction* instruction = XS_instruction_new(CALL);
        instruction->data_0 = argc;
    // Push to store
    XS_store_push(store, instruction);
}

// Binary operations
EXPORT void XS_opcode_binary_mul(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(BINARY_MUL);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_binary_div(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(BINARY_DIV);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_binary_mod(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(BINARY_MOD);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_binary_add(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(BINARY_ADD);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_binary_sub(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(BINARY_SUB);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_binary_lshift(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(BINARY_LSHIFT);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_binary_rshift(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(BINARY_RSHIFT);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_compare_less(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(COMPARE_LESS);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_compare_less_equal(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(COMPARE_LESS_EQUAL);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_compare_greater(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(COMPARE_GREATER);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_compare_greater_equal(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(COMPARE_GREATER_EQUAL);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_compare_equal(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(COMPARE_EQUAL);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_compare_not_equal(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(COMPARE_NOT_EQUAL);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_binary_and(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(BINARY_AND);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_binary_or(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(BINARY_OR);
    // Push to store
    XS_store_push(store, instruction);
}

EXPORT void XS_opcode_binary_xor(XS_store* store) {
    XS_instruction* instruction = XS_instruction_new(BINARY_XOR);
    // Push to store
    XS_store_push(store, instruction);
}

// Jumps
EXPORT XS_instruction* XS_opcode_jump_if_false_or_pop(XS_store* store, size_t offset) {
    XS_instruction* instruction = XS_instruction_new(JUMP_IF_FALSE_OR_POP);
        instruction->offset_0 = offset;
    // Push to store
    XS_store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* XS_opcode_jump_if_true_or_pop(XS_store* store, size_t offset) {
    XS_instruction* instruction = XS_instruction_new(JUMP_IF_TRUE_OR_POP);
        instruction->offset_0 = offset;
    // Push to store
    XS_store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* XS_opcode_jump_if_not_error_or_pop(XS_store* store, size_t offset) {
    XS_instruction* instruction = XS_instruction_new(JUMP_IF_NOT_ERROR_OR_POP);
        instruction->offset_0 = offset;
    // Push to store
    XS_store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* XS_opcode_pop_jump_if_false(XS_store* store, size_t offset) {
    XS_instruction* instruction = XS_instruction_new(POP_JUMP_IF_FALSE);
        instruction->offset_0 = offset;
    // Push to store
    XS_store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* XS_opcode_pop_jump_if_true(XS_store* store, size_t offset) {
    XS_instruction* instruction = XS_instruction_new(POP_JUMP_IF_TRUE);
        instruction->offset_0 = offset;
    // Push to store
    XS_store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* XS_opcode_jump_absolute(XS_store* store, size_t offset) {
    XS_instruction* instruction = XS_instruction_new(JUMP_ABSOLUTE);
        instruction->offset_0 = offset;
    // Push to store
    XS_store_push(store, instruction);
    return instruction;
}

EXPORT XS_instruction* XS_opcode_jump_forward(XS_store* store, size_t offset) {
    XS_instruction* instruction = XS_instruction_new(JUMP_FORWARD);
        instruction->offset_0 = offset;
    // Push to store
    XS_store_push(store, instruction);
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
        instruction->opcode == JUMP_IF_NOT_ERROR_OR_POP ||
        instruction->opcode == POP_JUMP_IF_FALSE    || 
        instruction->opcode == POP_JUMP_IF_TRUE     || 
        instruction->opcode == JUMP_FORWARD
    );
}

EXPORT size_t XS_opcode_get_current_jump_offset(XS_store* store) {
    return (store->icount * 2);
}

EXPORT void XS_opcode_set_jump_offset(XS_instruction* instruction, size_t offset) {
    opcode_assert_jump(instruction);
    instruction->is_jump_set = true;
    instruction->offset_0 = offset;
}

EXPORT void XS_opcode_jump_to_current_offset(XS_store* store, XS_instruction* instruction) {
    XS_opcode_set_jump_offset(instruction, XS_opcode_get_current_jump_offset(store));
}

EXPORT size_t XS_opcode_get_jump_offset(XS_instruction* instruction) {
    opcode_assert_jump(instruction);
    if (!instruction->is_jump_set) {
        fprintf(stderr, "%s::%s[%d]: Jump offset is not set!!!\n", __FILE__, __func__, __LINE__);
        exit(1);
    }
    return (size_t) floor(instruction->offset_0 / 2);
}