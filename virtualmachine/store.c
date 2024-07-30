#include "store.h"
#include "environment.h"

EXPORT XS_store* XS_store_new() {
    XS_store* store = XS_malloc(sizeof(XS_store));
    assert_allocation(store);
    store->scount = 0;
    store->stack = NULL;
    store->icount = 0;
    store->instructions = NULL;
    store->environment = XS_environment_new(NULL);
    return store;
}

EXPORT void XS_store_emit_line(XS_store* store, const char* origin, size_t line, size_t colm) {
    if (store->stack == NULL) {
        store->stack = XS_malloc(sizeof(char*));
        assert_allocation(store->stack);
    }

    store->stack[store->scount++] = str__format("at %s <%zu, %zu>", origin || "vm", line, colm);
    store->stack = XS_realloc(store->stack, sizeof(char*) * (store->scount + 1));
    assert_allocation(store->stack);
    store->stack[store->scount] = NULL;
}

EXPORT void XS_store_push(XS_store* store, XS_instruction* instruction) {
    if (store->instructions == NULL) {
        store->instructions = XS_malloc(sizeof(XS_instruction*));
        assert_allocation(store->instructions);
    }

    store->instructions[store->icount++] = instruction;
    store->instructions = XS_realloc(store->instructions, sizeof(XS_instruction*) * (store->icount + 1));
    assert_allocation(store->instructions);
    store->instructions[store->icount] = NULL;
}