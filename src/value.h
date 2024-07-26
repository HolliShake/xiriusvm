#include "global.h"
#include "inttypes.h"
#include "context.h"
#include "object.h"

#ifndef XSVALUE_H
#define XSVALUE_H
    typedef enum xirius_value_type_enum {
        XS_INT,
        XS_FLT,
        XS_STR,
        XS_BIT,
        XS_NIL,
        XS_ERR,
        XS_OBJ,
        XS_NATIVE_FUNCTION,
        XS_DEFINE_FUNCTION
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
            double flt_value; // 64-bit float|double
            char* str_value;
            bool bit_value;
            void* obj_value;
        } value;
    } XS_value;

    typedef XS_value* (*cfunction_t)(XS_context* context, XS_value* args[], int argc);

    #define XS_INT(context, i) XS_value_new_int(context, (const long long int) (i))
    #define XS_FLT(context, f) XS_value_new_flt(context, (const double) (f))
    #define XS_STR(context, s) XS_value_new_str(context, (const char* ) (s))
    #define XS_BIT(context, b) XS_value_new_bit(context, (const bool  ) (b))
    #define XS_NIL(context   ) XS_value_new_nil(context)
    #define XS_ERR(context, m) XS_value_new_err(context, (const char* ) (m))
    #define XS_OBJ(context) XS_value_new_obj(context)

    #define XS_GET_INT(_value) (_value->value.int_value)
    #define XS_GET_FLT(_value) (_value->value.flt_value)
    #define XS_GET_NUM(_value) (XS_IS_INT(_value) ? XS_GET_INT(_value) : XS_GET_FLT(_value))
    #define XS_GET_STR(_value) (_value->value.str_value)
    #define XS_GET_BIT(_value) (_value->value.bit_value)
    #define XS_GET_OBJ(_value) (_value->value.obj_value)

    #define XS_IS_INT(_value) (_value->type == XS_INT)
    #define XS_IS_FLT(_value) (_value->type == XS_FLT)
    #define XS_IS_NUM(_value) (XS_IS_INT(_value) || XS_IS_FLT(_value))
    #define XS_IS_STR(_value) (_value->type == XS_STR)
    #define XS_IS_BIT(_value) (_value->type == XS_BIT)
    #define XS_IS_NIL(_value) (_value->type == XS_NIL)
    #define XS_IS_ERR(_value) (_value->type == XS_ERR)
    #define XS_IS_OBJ(_value) (_value->type == XS_OBJ)
    #define XS_IS_NATIVE_FUNCTION(_value) (_value->type == XS_NATIVE_FUNCTION)
    #define XS_IS_DEFINE_FUNCTION(_value) (_value->type == XS_DEFINE_FUNCTION)

    EXPORT XS_value* XS_value_new_int(XS_context* context, const long long int value);
    EXPORT XS_value* XS_value_new_flt(XS_context* context, const double value);
    EXPORT XS_value* XS_value_new_str(XS_context* context, const char* value);
    EXPORT XS_value* XS_value_new_bit(XS_context* context, const bool value);
    EXPORT XS_value* XS_value_new_nil(XS_context* context);
    EXPORT XS_value* XS_value_new_err(XS_context* context, const char* message);
    EXPORT XS_value* XS_value_new_obj(XS_context* context);
        EXPORT XS_value* XS_value_get_object_property(XS_value* object, XS_value* key);
    
    // Cfunction type
    EXPORT XS_value* XS_value_new_cfunction(cfunction_t cfunction, bool async, const char* name, int argc);

    // Type Checker
    EXPORT bool XS_value_is_int(XS_value* value);
    EXPORT bool XS_value_is_flt(XS_value* value);
    EXPORT bool XS_value_is_num(XS_value* value);
    EXPORT bool XS_value_is_str(XS_value* value);
    EXPORT bool XS_value_is_bit(XS_value* value);
    EXPORT bool XS_value_is_nil(XS_value* value);
    EXPORT bool XS_value_is_err(XS_value* value);
    EXPORT bool XS_value_is_obj(XS_value* value);
    EXPORT bool XS_value_is_native_function(XS_value* value);
    EXPORT bool XS_value_is_define_function(XS_value* value);
    EXPORT bool XS_value_is_satisfiable(XS_value* value);
    // Utility
    EXPORT long long int XS_value_hash(XS_value* value);
    EXPORT bool XS_value_equals(XS_value* a, XS_value* b);
    EXPORT const char* XS_value_to_const_string(XS_value* value);
#endif