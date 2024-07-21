#include "store.h"

#ifndef OPCODE_H
    /*virtual*/ typedef struct xirius_instruction_struct XS_instruction;
#endif

EXPORT store_t* store_new() {
    store_t* store = XS_malloc(sizeof(store_t));
    assert_allocation(store);
    store->icount = 0;
    store->instructions = XS_malloc(sizeof(XS_instruction*));
    assert_allocation(store->instructions);
    return store;
}

EXPORT void store_push(store_t* store, XS_instruction* instruction) {
    store->instructions[store->icount++] = instruction;
    store->instructions = XS_realloc(store->instructions, sizeof(XS_instruction*) * (store->icount + 1));
    assert_allocation(store->instructions);
    store->instructions[store->icount] = NULL;
}