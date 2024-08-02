#include "global.h"

#ifndef RUNTIME_H
#define RUNTIME_H
    #define STACK_SIZE 1024

    typedef struct xirius_runtime_struct {
        // Evaluation stack
        size_t evaluation_stack_base;
        XS_value* evaluation_stack[STACK_SIZE];
        // Event queue
        size_t event_queue_base;
        XS_value* event_queue[STACK_SIZE];
        // Finalization
        XS_value* (*finalize)(XS_context* context);
    } XS_runtime;

    EXPORT XS_runtime* XS_runtime_new();
    EXPORT void XS_runtime_execute(XS_context* context, XS_value* executable);
    EXPORT void XS_runtime_free(XS_runtime* runtime);
#endif