#include "global.h"

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
    typedef struct xirius_environment_struct {
        XS_environment* parent;
        size_t elements;
        size_t capacity;
        XS_environment_node_t** bucket;
    } XS_environment;
    typedef struct xirius_environment_node_struct {
        const char* name;
        XS_value* value;
        XS_environment_node_t* next;
    } XS_environment_node_t;

    EXPORT XS_environment* XS_environment_new(XS_environment* parent);
    EXPORT XS_value* XS_environment_set(XS_environment* environment, const char* name, XS_value* value);
    EXPORT XS_value* XS_environment_get(XS_environment* environment, const char* name);
    EXPORT bool XS_environment_has(XS_environment* environment, const char* name);
    EXPORT void XS_environment_resize(XS_environment* environment);
    EXPORT void XS_environment_reset(XS_environment* environment);
    EXPORT XS_environment* XS_environment_copy(XS_environment* environment);
    EXPORT void XS_environment_dump(XS_environment* environment);
    EXPORT void XS_environment_free(XS_environment* environment);
#endif