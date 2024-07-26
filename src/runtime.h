#include "global.h"
#include "context.h"
#include "store.h"
#include "opcode.h"
#include "value.h"
#include "object.h"

#ifndef RUNTIME_H
#define RUNTIME_H
    #define STACK_SIZE 1024

    typedef struct xirius_runtime_struct {
        // Evaluation stack
        size_t evaluation_stack_base;
        XS_value* evaluation_stack[50];
        // Last known error
        XS_value* error;
    } XS_runtime;

    EXPORT XS_runtime* XS_runtime_new();
    EXPORT void XS_runtime_execute(XS_context* context, XS_store* store);
    EXPORT void XS_runtime_execute_event_loop(XS_context* context);
    EXPORT void XS_runtime_free(XS_runtime* runtime);
#endif