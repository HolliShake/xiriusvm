#include "global.h"
#include "value.h"

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
    typedef struct xirius_environment_struct XS_environment;
    typedef struct xirius_environment_cell_struct XS_environment_cell;
    typedef struct xirius_environment_struct {
        XS_environment* parent;
        size_t elements;
        size_t capacity;
        XS_environment_cell** bucket;
    } XS_environment;
    typedef struct xirius_environment_cell_struct {
        const char* name;
        XS_value* value;
        XS_environment_cell* next;
    } XS_environment_cell;

    XS_environment* XS_environment_new(XS_environment* parent);
    void XS_environment_set(XS_environment* environment, const char* name, XS_value* value);
    XS_value* XS_environment_get(XS_environment* environment, const char* name);
    bool XS_environment_has(XS_environment* environment, const char* name);
    void XS_environment_resize(XS_environment* environment);
    void XS_environment_reset(XS_environment* environment);
    XS_environment* XS_environment_copy(XS_environment* environment);
    void XS_environment_dump(XS_environment* environment);
    void XS_environment_free(XS_environment* environment);
#endif