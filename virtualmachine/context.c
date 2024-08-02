#include "context.h"
#include "value.h"

EXPORT XS_context* XS_context_new(XS_runtime* runtime) {
    XS_context* ctx = XS_malloc(sizeof(XS_context));
    assert_allocation(ctx);
    ctx->runtime = runtime;

    // Create global object
    ctx->global_object = XS_value_new_obj(ctx);
    ctx->null_obj = NULL;
    // Garbage collector

    return ctx;
}

EXPORT XS_runtime* XS_context_get_runtime(XS_context* ctx) {
    return ctx->runtime;
}

EXPORT void XS_context_free(XS_context* ctx) {
    XS_free(ctx);
}