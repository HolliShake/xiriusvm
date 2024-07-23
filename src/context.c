#include "context.h"

#ifndef XSVALUE_H
    /*virtual*/ typedef struct xirius_value_struct XS_value;
    /*virtual*/ extern XS_value* XS_value_new_object(XS_context* context);
#endif

EXPORT XS_context* XS_context_new(XS_runtime* runtime) {
    XS_context* ctx = XS_malloc(sizeof(XS_context));
    assert_allocation(ctx);
    ctx->runtime = runtime;

    // Create global object
    ctx->global_object = XS_value_new_object(ctx);

    return ctx;
}

EXPORT XS_runtime* XS_context_get_runtime(XS_context* ctx) {
    return ctx->runtime;
}

EXPORT void XS_context_free(XS_context* ctx) {
    XS_free(ctx);
}