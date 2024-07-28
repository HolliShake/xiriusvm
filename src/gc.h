#include "global.h"

#ifndef XSGC_H
#define XSGC_H    
        typedef struct xirius_gc_struct {
            XS_value* root;
        } XS_gc;
    
        EXPORT XS_gc* XS_gc_new();
        EXPORT void XS_gc_free(XS_gc* gc);
        EXPORT void XS_gc_mark(XS_gc* gc, XS_value* value);
        EXPORT void XS_gc_sweep(XS_gc* gc);
        EXPORT void XS_gc_collect(XS_gc* gc);
        EXPORT void XS_gc_add_root(XS_gc* gc, XS_value* value);
        EXPORT void XS_gc_remove_root(XS_gc* gc, XS_value* value);
#endif