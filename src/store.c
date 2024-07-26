#include "store.h"

#ifndef OPCODE_H
    /*virtual*/ typedef struct xirius_instruction_struct XS_instruction;
#endif

EXPORT store_t* store_new() {
    store_t* store = XS_malloc(sizeof(store_t));
    assert_allocation(store);
    store->scount = 0;
    store->stack = NULL;
    store->icount = 0;
    store->instructions = NULL;
    return store;
}

EXPORT void store_emit_line(store_t* store, char* origin, size_t line, size_t colm) {
    if (store->stack == NULL) {
        store->stack = XS_malloc(sizeof(char*));
        assert_allocation(store->stack);
    }

    store->stack[store->scount++] = str__format("at %s <%zu, %zu>", origin || "vm", line, colm);
    store->stack = XS_realloc(store->stack, sizeof(char*) * (store->scount + 1));
    assert_allocation(store->stack);
    store->stack[store->scount] = NULL;
}

EXPORT void store_push(store_t* store, XS_instruction* instruction) {
    if (store->instructions == NULL) {
        store->instructions = XS_malloc(sizeof(XS_instruction*));
        assert_allocation(store->instructions);
    }

    store->instructions[store->icount++] = instruction;
    store->instructions = XS_realloc(store->instructions, sizeof(XS_instruction*) * (store->icount + 1));
    assert_allocation(store->instructions);
    store->instructions[store->icount] = NULL;
}