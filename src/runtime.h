#include "global.h"
#include "context.h"
#include "store.h"
#include "opcode.h"
#include "xsvalue.h"
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
    EXPORT void XS_execute(XS_context* context, store_t* store);
    EXPORT void XS_execute_event_loop(XS_context* context);
#endif