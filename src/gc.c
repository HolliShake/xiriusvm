#include "gc.h"


EXPORT XS_gc* XS_gc_new() {
    XS_gc* gc = XS_malloc(sizeof(XS_gc));
    assert_allocation(gc);
    gc->root = NULL;
    return gc;
}

EXPORT void XS_gc_free(XS_gc* gc) {
    XS_free(gc);
}

EXPORT void XS_gc_mark(XS_gc* gc, XS_value* value) {
    if (value == NULL) {
        return;
    }
    value->marked = true;
    if (XS_value_is_obj(value)) {
        object_t* object = value->value.obj_value;
        for (size_t i = 0; i < object->capacity; i++) {
            object_node_t* node = object->buckets[i];
            while (node != NULL) {
                XS_gc_mark(gc, node->key);
                XS_gc_mark(gc, node->value);
                node = node->next;
            }
        }
    }
}

EXPORT void XS_gc_sweep(XS_gc* gc) {
    XS_value* value = gc->root;
    while (value != NULL) {
        if (!value->marked) {
            XS_value* next = value->next;
            XS_gc_remove_root(gc, value);
            XS_free(value);
            value = next;
        } else {
            value->marked = false;
            value = value->next;
        }
    }
}

EXPORT void XS_gc_collect(XS_gc* gc) {
    XS_gc_mark(gc, gc->root);
    XS_gc_sweep(gc);
}

EXPORT void XS_gc_add_root(XS_gc* gc, XS_value* value) {
    value->next = gc->root;
    gc->root = value;
}

EXPORT void XS_gc_remove_root(XS_gc* gc, XS_value* value) {
    if (gc->root == value) {
        gc->root = value->next;
        return;
    }
    XS_value* current = gc->root;
    while (current->next != value) {
        current = current->next;
    }
    current->next = value->next;
}