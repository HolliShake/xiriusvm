#include "global.h"

#ifndef OBJECT_H
#define OBJECT_H
    #ifndef XSVALUE_H
        /*virtual*/ typedef struct xirius_value_struct XS_value;
    #endif

    typedef struct object_node_struct object_node_t;
    typedef struct object_node_struct {
        XS_value* key;
        XS_value* value;
        object_node_t* next;
    } object_node_t;

    typedef struct object_struct {
        object_node_t** buckets;
        size_t element_count;
        size_t capacity;
    } object_t;

    EXPORT object_t* object_new();
    EXPORT void object_set(object_t* object, XS_value* key, XS_value* value);
    EXPORT XS_value* object_get(object_t* object, XS_value* key);
    EXPORT void object_resize(object_t* object, size_t new_capacity);
    EXPORT char* object_to_string(object_t* object);
#endif