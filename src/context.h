#include "global.h"

#ifndef CONTEXT_H
#define CONTEXT_H
    #ifndef RUNTIME_H
        /*virtual*/ typedef struct xirius_runtime_struct XS_runtime;
    #endif
    #ifndef XSVALUE_H
        /*virtual*/ typedef struct xirius_value_struct XS_value;
    #endif

    typedef struct xirius_context_struct {
        XS_runtime* runtime;
        XS_value* global_object;
    } XS_context;

    EXPORT XS_context* XS_context_new(XS_runtime* runtime);
    EXPORT XS_runtime* context_get_runtime(XS_context* ctx);
#endif