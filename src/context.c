#include "context.h"

EXPORT XS_context* XS_context_new(XS_runtime* runtime) {
    XS_context* ctx = XS_malloc(sizeof(XS_context));
    assert_allocation(ctx);
    ctx->runtime = runtime;
    return ctx;
}

EXPORT XS_runtime* context_get_runtime(XS_context* ctx) {
    return ctx->runtime;
}