#include "global.h"

#ifndef CONTEXT_H
#define CONTEXT_H
    typedef struct xirius_context_struct {
        XS_runtime* runtime;
        XS_value* global_object;
        XS_value* null_obj;
    } XS_context;

    EXPORT XS_context* XS_context_new(XS_runtime* runtime);
    EXPORT XS_runtime* XS_context_get_runtime(XS_context* ctx);
    EXPORT void XS_context_free(XS_context* ctx);
#endif