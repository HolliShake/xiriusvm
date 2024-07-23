#include "global.h"
#include "inttypes.h"
#include "context.h"
#include "object.h"

#ifndef XSVALUE_H
#define XSVALUE_H
    typedef enum xirius_value_type {
        XS_INT,
        XS_FLOAT,
        XS_STRING,
        XS_BOOL,
        XS_NULL,
        XS_OBJECT,
        XS_NATIVE_FUNCTION,
        XS_FUNCTION,
        XS_ERROR
    } XS_value_type;

    typedef struct xirius_value_struct XS_value;
    typedef struct xirius_value_struct {
        bool marked;
        XS_value* next;
        
        // If named
        char* name;

        // Data
        int argc;
        bool async;

        // Type
        XS_value_type type;

        union value {
            int64_t int_value; // 64-bit integer
            double float_value; // 64-bit float|double
            char* string_value;
            bool bool_value;
            void* object;
        } value;
    } XS_value;

    typedef XS_value* (*cfunction_t)(XS_context* context, XS_value* args[], int argc);

    EXPORT XS_value* XS_value_new_cint(XS_context* context, const long long int value);
    EXPORT XS_value* XS_value_new_cfloat(XS_context* context, const double value);
    EXPORT XS_value* XS_value_new_cstring(XS_context* context, const char* value);
    EXPORT XS_value* XS_value_new_cbool(XS_context* context, const bool value);
    EXPORT XS_value* XS_value_new_cnull(XS_context* context);
    EXPORT XS_value* XS_value_new_object(XS_context* context);
        EXPORT XS_value* XS_value_get_object_property_from_cstring(XS_value* object, const char* key);
        EXPORT XS_value* XS_value_get_object_property(XS_value* object, XS_value* key);
    EXPORT const char* XS_value_to_cstring(XS_value* value);
    
    // Cfunction type
    EXPORT XS_value* XS_value_new_cfunction(cfunction_t cfunction, bool async, const char* name, int argc);

    // Type Checker
    EXPORT bool XS_value_is_int(XS_value* value);
    EXPORT bool XS_value_is_float(XS_value* value);
    EXPORT bool XS_value_is_number(XS_value* value);
    EXPORT bool XS_value_is_string(XS_value* value);
    EXPORT bool XS_value_is_bool(XS_value* value);
    EXPORT bool XS_value_is_null(XS_value* value);
    EXPORT bool XS_value_is_satisfiable(XS_value* value);
    EXPORT bool XS_value_is_object(XS_value* value);
    EXPORT bool XS_value_is_native_function(XS_value* value);
    EXPORT bool XS_value_is_function(XS_value* value);
    EXPORT bool XS_value_is_error(XS_value* value);
    // Utility
    EXPORT long long int XS_value_hash(XS_value* value);
    EXPORT bool XS_value_equals(XS_value* a, XS_value* b);
#endif