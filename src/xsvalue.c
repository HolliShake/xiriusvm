#include "xsvalue.h"

static
XS_value* XS_value_init(XS_context* context, XS_value_type type) {
    XS_value* new_value = XS_malloc(sizeof(XS_value));
    assert_allocation(new_value);
    new_value->type = type;
    new_value->name = NULL;
    new_value->argc = 0;
    new_value->async = false;
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

EXPORT XS_value *XS_value_new_obj(XS_context* context) {
    XS_value* new_value = XS_value_init(context, XS_OBJ);
    new_value->value.obj_value = object_new();
    return new_value;
}
    EXPORT XS_value* XS_value_get_object_property_from_const_string(XS_value* object, const char* key) {
        return object_get_from_cstring(object->value.obj_value, key);
    }

    EXPORT XS_value* XS_value_get_object_property(XS_value* object, XS_value* key) {
        return object_get(object->value.obj_value, key);
    }

EXPORT XS_value* XS_value_new_cfunction(cfunction_t cfunction, bool async, const char* name, int argc) {
    XS_value* new_value = XS_malloc(sizeof(XS_value));
    assert_allocation(new_value);
    new_value->type = XS_NATIVE_FUNCTION;
    new_value->name = str__new(name);
    new_value->argc = argc;
    new_value->async = async;
    new_value->value.obj_value = cfunction;
    return new_value;
}

// Type Checker
EXPORT bool XS_value_is_int(XS_value *value) {
    return (value->type == XS_INT);
}

EXPORT bool XS_value_is_flt(XS_value *value) {
    return (value->type == XS_FLT);
}

EXPORT bool XS_value_is_num(XS_value *value) {
    return XS_value_is_int(value) || XS_value_is_flt(value);
}

EXPORT bool XS_value_is_str(XS_value *value) {
    return (value->type == XS_STR);
}

EXPORT bool XS_value_is_bit(XS_value *value) {
    return (value->type == XS_BIT);
}

EXPORT bool XS_value_is_nil(XS_value *value) {
    return (value->type == XS_NIL);
}

EXPORT bool XS_value_is_err(XS_value* value) {
    return value->type == XS_ERR;
}

EXPORT bool XS_value_is_obj(XS_value* value) {
    return value->type == XS_OBJ;
}

EXPORT bool XS_value_is_native_function(XS_value* value) {
    return value->type == XS_NATIVE_FUNCTION;
}

EXPORT bool XS_value_is_define_function(XS_value* value) {
    return value->type == XS_DEFINE_FUNCTION;
}

EXPORT bool XS_value_is_satisfiable(XS_value* value) {
    if (XS_value_is_int(value))
        return (value->value.int_value != 0);
    else if (XS_value_is_flt(value))
        return (value->value.flt_value != 0.0);
    else if (XS_value_is_str(value))
        return (strlen(value->value.str_value) > 0);
    else if (XS_value_is_bit(value))
        return (value->value.bit_value == true);
    else if (XS_value_is_nil(value))
        return false;
    else
        printf("%s::%s[%d]:warning: [Not Implemented]!!!\n", __FILE__, __func__, __LINE__);
    return false;
}

// Utility
static
int64_t hash_double(double d) {
    // Use type punning to interpret the bits of the double as an integer
    union {
        double d;
        int64_t i;
    } u;
    
    u.d = d;

    // Hash the integer representation of the double
    // A simple example: combine the bits using bit shifts and XOR
    uint64_t hash = u.i;
    hash ^= (hash >> 32);
    hash *= 0xd6e8feb86659fd93LL;
    hash ^= (hash >> 32);
    hash *= 0xa5a5a5a5a5a5a5a5LL;
    hash ^= (hash >> 32);
    return hash;
}

int64_t hash_string(const char* str) {
    int64_t hash = 0;
    for (size_t i = 0; i < strlen(str); i++) {
        hash = (hash * 31) + str[i];
    }
    return hash;
}

EXPORT long long int XS_value_hash(XS_value* value) {
    if (XS_value_is_int(value))
        return (long long int) value->value.int_value;
    else if (XS_value_is_flt(value))
        return (long long int) hash_double(value->value.flt_value);
    else if (XS_value_is_str(value))
        return (long long int) hash_string((const char*) value->value.str_value);
    else if (XS_value_is_bit(value))
        return (unsigned long long int) value->value.bit_value;
    else if (XS_value_is_nil(value))
        return 0;
    else
        printf("%s::%s[%d]:warning: [Not Implemented]!!!\n", __FILE__, __func__, __LINE__);
    return 0;
}

EXPORT bool XS_value_equals(XS_value* a, XS_value* b) {
    if (XS_value_is_int(a) && XS_value_is_int(b))
        return (a->value.int_value == b->value.int_value);
    else if (XS_value_is_flt(a) && XS_value_is_flt(b))
        return (a->value.flt_value == b->value.flt_value);
    else if (XS_value_is_int(a) && XS_value_is_flt(b))
        return (a->value.int_value == b->value.flt_value);
    else if (XS_value_is_flt(a) && XS_value_is_int(b))
        return (a->value.flt_value == b->value.int_value);
    else if (XS_value_is_str(a) && XS_value_is_str(b))
        return str__equals((const char*) a->value.str_value, (const char*) b->value.str_value);
    else if (XS_value_is_bit(a) && XS_value_is_bit(b))
        return (a->value.bit_value == b->value.bit_value);
    else if (XS_value_is_nil(a) && XS_value_is_nil(b))
        return true;
    else
        printf("%s::%s[%d]:warning: [Not Implemented]!!!\n", __FILE__, __func__, __LINE__);
    return (a == b);
}

EXPORT const char* XS_value_to_const_string(XS_value* value) {
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