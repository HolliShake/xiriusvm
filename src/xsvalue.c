#include "xsvalue.h"

static
XS_value* XS_value_init(XS_context* context, XS_value_type type) {
    XS_value* new_value = XS_malloc(sizeof(XS_value));
    assert_allocation(new_value);
    new_value->type = type;
    new_value->name = NULL;
    new_value->argc = 0;
    new_value->async = false;
    new_value->value.object = NULL;
    return new_value;
}

EXPORT XS_value *XS_value_new_cint(XS_context* context, const long long int value) {
    XS_value* new_value = XS_value_init(context, XS_INT);
    new_value->value.int_value = value;
    return new_value;
}

EXPORT XS_value *XS_value_new_cfloat(XS_context* context, const double value) {
    XS_value* new_value = XS_value_init(context, XS_FLOAT);
    new_value->value.float_value = value;
    return new_value;
}

EXPORT XS_value *XS_value_new_cstring(XS_context* context, const char *value) {
    XS_value* new_value = XS_value_init(context, XS_STRING);
    new_value->value.string_value = str__new(value);
    return new_value;
}

EXPORT XS_value *XS_value_new_cbool(XS_context* context, const bool value) {
    XS_value* new_value = XS_value_init(context, XS_BOOL);
    new_value->value.bool_value = value;
    return new_value;
}

EXPORT XS_value *XS_value_new_cnull(XS_context* context) {
    XS_value* new_value = XS_value_init(context, XS_NULL);
    return new_value;
}

EXPORT XS_value *XS_value_new_object(XS_context* context) {
    XS_value* new_value = XS_value_init(context, XS_OBJECT);
    new_value->value.object = object_new();
    return new_value;
}
    EXPORT XS_value* XS_value_get_object_property_from_cstring(XS_value* object, const char* key) {
        return object_get_from_cstring(object->value.object, key);
    }

    EXPORT XS_value* XS_value_get_object_property(XS_value* object, XS_value* key) {
        return object_get(object->value.object, key);
    }

EXPORT const char* XS_value_to_cstring(XS_value* value) {
    switch (value->type) {
        case XS_INT:
            return (const char*) str__format("%lld", value->value.int_value);
        case XS_FLOAT:
            return (const char*) str__format("%.4lf", value->value.float_value);
        case XS_STRING:
            return (const char*) str__format("%s", value->value.string_value);
        case XS_BOOL:
            return (const char*) str__format("%s", (value->value.bool_value) ? "true" : "false");
        case XS_NULL:
            return (const char*) str__new("null");
        default:
            return (const char*) object_to_string((object_t*) value->value.object);
    }
}

EXPORT XS_value* XS_value_new_cfunction(cfunction_t cfunction, bool async, const char* name, int argc) {
    XS_value* new_value = XS_malloc(sizeof(XS_value));
    assert_allocation(new_value);
    new_value->type = XS_NATIVE_FUNCTION;
    new_value->name = str__new(name);
    new_value->argc = argc;
    new_value->async = async;
    new_value->value.object = cfunction;
    return new_value;
}

// Type Checker
EXPORT bool XS_value_is_int(XS_value *value) {
    return (value->type == XS_INT);
}

EXPORT bool XS_value_is_float(XS_value *value) {
    return (value->type == XS_FLOAT);
}

EXPORT bool XS_value_is_number(XS_value *value) {
    return XS_value_is_int(value) || XS_value_is_float(value);
}

EXPORT bool XS_value_is_string(XS_value *value) {
    return (value->type == XS_STRING);
}

EXPORT bool XS_value_is_bool(XS_value *value) {
    return (value->type == XS_BOOL);
}

EXPORT bool XS_value_is_null(XS_value *value) {
    return (value->type == XS_NULL);
}

EXPORT bool XS_value_is_satisfiable(XS_value* value) {
    if (XS_value_is_int(value))
        return (value->value.int_value != 0);
    else if (XS_value_is_float(value))
        return (value->value.float_value != 0.0);
    else if (XS_value_is_string(value))
        return (strlen(value->value.string_value) > 0);
    else if (XS_value_is_bool(value))
        return (value->value.bool_value == true);
    else if (XS_value_is_null(value))
        return false;
    else
        printf("%s::%s[%d]:warning: [Not Implemented]!!!\n", __FILE__, __func__, __LINE__);
    return false;
}

EXPORT bool XS_value_is_object(XS_value* value) {
    return value->type == XS_OBJECT;
}

EXPORT bool XS_value_is_native_function(XS_value* value) {
    return value->type == XS_NATIVE_FUNCTION;
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
    else if (XS_value_is_float(value))
        return (long long int) hash_double(value->value.float_value);
    else if (XS_value_is_string(value))
        return (long long int) hash_string((const char*) value->value.string_value);
    else if (XS_value_is_bool(value))
        return (unsigned long long int) value->value.bool_value;
    else if (XS_value_is_null(value))
        return 0;
    else
        printf("%s::%s[%d]:warning: [Not Implemented]!!!\n", __FILE__, __func__, __LINE__);
    return 0;
}

EXPORT bool XS_value_equals(XS_value* a, XS_value* b) {
    if (XS_value_is_int(a) && XS_value_is_int(b))
        return (a->value.int_value == b->value.int_value);
    else if (XS_value_is_float(a) && XS_value_is_float(b))
        return (a->value.float_value == b->value.float_value);
    else if (XS_value_is_int(a) && XS_value_is_float(b))
        return (a->value.int_value == b->value.float_value);
    else if (XS_value_is_float(a) && XS_value_is_int(b))
        return (a->value.float_value == b->value.int_value);
    else if (XS_value_is_string(a) && XS_value_is_string(b))
        return str__equals((const char*) a->value.string_value, (const char*) b->value.string_value);
    else if (XS_value_is_bool(a) && XS_value_is_bool(b))
        return (a->value.bool_value == b->value.bool_value);
    else if (XS_value_is_null(a) && XS_value_is_null(b))
        return true;
    else
        printf("%s::%s[%d]:warning: [Not Implemented]!!!\n", __FILE__, __func__, __LINE__);
    return (a == b);
}