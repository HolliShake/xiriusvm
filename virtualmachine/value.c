#include "value.h"
#include "context.h"
#include "object.h"

/*
// Type
XS_value_type type;

// Store
XS_store* store;

// If named
char* name;

// Data
bool variadict;
int argc;
XS_value* argv[255];

// GC
bool marked;
XS_value* next;
*/

static
XS_value* XS_value_init(XS_context* context, XS_value_type type) {
    XS_value* new_value = XS_malloc(sizeof(XS_value));
    assert_allocation(new_value);
    new_value->type = type;
    new_value->store = NULL;
    new_value->name = NULL;
    new_value->variadict = false;
    new_value->argc = 0;
    new_value->marked = false;
    new_value->next = NULL;
    new_value->value.int_value = 0;
    new_value->value.flt_value = 0;
    new_value->value.str_value = NULL;
    new_value->value.bit_value = false;
    new_value->value.obj_value = NULL;
    return new_value;
}

EXPORT XS_value *XS_value_new_int(XS_context* context, const long long int value) {
    XS_value* new_value = XS_value_init(context, XS_INT);
    new_value->value.int_value = value;
    return new_value;
}

EXPORT XS_value *XS_value_new_flt(XS_context* context, const double value) {
    XS_value* new_value = XS_value_init(context, XS_FLT);
    new_value->value.flt_value = value;
    return new_value;
}

EXPORT XS_value *XS_value_new_str(XS_context* context, const char *value) {
    XS_value* new_value = XS_value_init(context, XS_STR);
    new_value->value.str_value = str__new(value);
    return new_value;
}

EXPORT XS_value *XS_value_new_bit(XS_context* context, const bool value) {
    XS_value* new_value = XS_value_init(context, XS_BIT);
    new_value->value.bit_value = value;
    return new_value;  
}

EXPORT XS_value *XS_value_new_nil(XS_context* context) {
    if (context->null_obj != NULL) {
        return context->null_obj;
    }
    XS_value* new_value = context->null_obj = XS_value_init(context, XS_NIL);
    return new_value;
}

EXPORT XS_value* XS_value_new_err(XS_context* context, const char* message) {
    XS_value* new_value = XS_value_init(context, XS_ERR);
    new_value->value.str_value = str__new(message);
    return new_value;
}

EXPORT XS_value* XS_value_new_code(XS_context* context, XS_store* store) {
    XS_value* new_value = XS_value_init(context, XS_CODE);
    new_value->store = store;
    return new_value;
}

EXPORT XS_value *XS_value_new_obj(XS_context* context) {
    XS_value* new_value = XS_value_init(context, XS_OBJ);
    new_value->value.obj_value = object_new();
    return new_value;
}
    EXPORT void XS_value_set_object_property(XS_value* object, XS_value* key, XS_value* value) {
        object_set(object->value.obj_value, key, value);
    }
    EXPORT XS_value* XS_value_get_object_property(XS_value* object, XS_value* key) {
        return object_get(object->value.obj_value, key);
    }

EXPORT XS_value* XS_value_new_cfunction(XS_context* context, cfunction_t cfunction, bool variadict, const char* name, int argc) {
    XS_value* new_value = XS_value_init(context, XS_NATIVE_FUNCTION);
    if (variadict && argc <= 0) {
        fprintf(stderr, "%s::%s[%d]: variadic functions must have atleast one argument!\n", __FILE__, __func__, __LINE__);
        exit(1);
    }
    new_value->name = str__new(name);
    new_value->argc = argc;
    new_value->variadict = variadict;
    new_value->value.obj_value = cfunction;
    return new_value;
}

EXPORT XS_value* XS_value_new_function(XS_context* context, XS_store* store, bool variadict, const char* name, int argc) {
    XS_value* new_value = XS_value_init(context,  XS_DEFINE_FUNCTION);
    if (variadict && argc <= 0) {
        fprintf(stderr, "%s::%s[%d]: variadic functions must have atleast one argument!\n", __FILE__, __func__, __LINE__);
        exit(1);
    }
    new_value->store = store;
    new_value->name = str__new(name);
    new_value->argc = argc;
    new_value->variadict = variadict;
    return new_value;
}

// Type Checker
EXPORT bool XS_value_is_int(XS_value* value) {
    return XS_IS_INT(value);
}

EXPORT bool XS_value_is_flt(XS_value* value) {
    return XS_IS_FLT(value);
}

EXPORT bool XS_value_is_num(XS_value* value) {
    return XS_IS_NUM(value);
}

EXPORT bool XS_value_is_str(XS_value* value) {
    return XS_IS_STR(value);
}

EXPORT bool XS_value_is_bit(XS_value* value) {
    return XS_IS_BIT(value);
}

EXPORT bool XS_value_is_nil(XS_value* value) {
    return XS_IS_NIL(value);
}

EXPORT bool XS_value_is_err(XS_value* value) {
    return XS_IS_ERR(value);
}

EXPORT bool XS_value_is_obj(XS_value* value) {
    return XS_IS_OBJ(value);
}

EXPORT bool XS_value_is_code(XS_value* value) {
    return XS_IS_CODE(value);
}

EXPORT bool XS_value_is_native_function(XS_value* value) {
    return XS_IS_NATIVE_FUNCTION(value);
}

EXPORT bool XS_value_is_define_function(XS_value* value) {
    return XS_IS_DEFINE_FUNCTION(value);
}

EXPORT bool XS_value_is_callable(XS_value* value) {
    return (XS_IS_NATIVE_FUNCTION(value) || XS_IS_DEFINE_FUNCTION(value));
}

EXPORT bool XS_value_is_satisfiable(XS_value* value) {
    if (XS_IS_INT(value))
        return (XS_GET_INT(value) != 0);
    else if (XS_IS_FLT(value))
        return (XS_GET_FLT(value) != 0.0);
    else if (XS_value_is_str(value))
        return (strlen(XS_GET_STR(value)) > 0);
    else if (XS_IS_BIT(value))
        return (XS_GET_BIT(value) == true);
    else if (XS_IS_NIL(value))
        return false;
    else
        printf("%s::%s[%d]:warning: [Not Implemented]!!!\n", __FILE__, __func__, __LINE__);
    return false;
}

// Utility
EXPORT long long int XS_value_hash(XS_value* value) {
    if (XS_IS_INT(value))
        return XS_GET_INT(value);
    else if (XS_IS_FLT(value))
        return (long long int) hash_double(XS_GET_FLT(value));
    else if (XS_IS_STR(value))
        return (long long int) hash_string(XS_GET_STR(value));
    else if (XS_IS_BIT(value))
        return XS_GET_BIT(value);
    else if (XS_IS_NIL(value))
        return 0;
    else
        printf("%s::%s[%d]:warning: [Not Implemented]!!!\n", __FILE__, __func__, __LINE__);
    return 0;
}

EXPORT bool XS_value_equals(XS_value* a, XS_value* b) {
    if (XS_IS_INT(a) && XS_IS_INT(b))
        return (XS_GET_INT(a) == XS_GET_INT(b));
    else if (XS_IS_FLT(a) && XS_IS_FLT(b))
        return (XS_GET_FLT(a) == XS_GET_FLT(b));
    else if (XS_IS_NUM(a) && XS_IS_NUM(b))
        return (XS_GET_NUM(a) == XS_GET_NUM(b));
    else if (XS_IS_STR(a) && XS_IS_STR(b))
        return str__equals(XS_GET_STR(a), XS_GET_STR(b));
    else if (XS_IS_BIT(a) && XS_IS_BIT(b))
        return (XS_GET_BIT(a) == XS_GET_BIT(b));
    else if (XS_IS_NIL(a) && XS_IS_NIL(b))
        return true;
    return (a == b);
}

EXPORT const char* XS_value_to_const_string(XS_value* value) {
    if (!value) {
        return str__new("null");
    }
    switch (value->type) {
        case XS_INT:
            return (const char*) str__format("%lld", value->value.int_value);
        case XS_FLT:
            return (const char*) str__format("%.4lf", value->value.flt_value);
        case XS_STR:
            return (const char*) str__format("%s", value->value.str_value);
        case XS_BIT:
            return (const char*) str__format("%s", (value->value.bit_value) ? "true" : "false");
        case XS_NIL:
            return (const char*) str__new("null");
        case XS_ERR:
            return (const char*) str__format("%s", value->value.str_value);
        case XS_OBJ:
            return (const char*) object_to_string((object_t*) value->value.obj_value);
        default:
            return str__new("[NOT IMPLEMENTED]");
    }
}